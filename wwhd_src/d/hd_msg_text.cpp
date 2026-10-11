/* HD message text state. */
// HD: message text layout, safe-string storage and five-line metrics have no GC counterpart.
#include "d/hd_msg_text.h"
using namespace gabi;
static u32 l_hdMsgStringVTable;
static u32 l_hdMsgBufferedVTable;
static u32 l_hdMsgFixedVTable;
static u32 l_hdMsgRubyVTable;
static u32 l_hdMsgTextVTable;
static f32 l_hdMsgZero;
static f32 l_hdMsgOne;
static f32 l_hdMsgMinusOne;
static u32 l_hdMsgSoundX;
static u32 l_hdMsgSoundY;
static u32 l_hdMsgSoundZ;
static HdMsgVoidPointer l_hdMsgManager;
static HdMsgVoidPointer l_hdMsgFontManager;
static HdMsgSystemPointer l_hdMsgSystem;
static HdMsgStaticWords l_hdMsgWords;
static HdMsgRange l_hdMsgRange;
static f32 l_hdMsgLow;
static f32 l_hdMsgHigh;
static HdMsgRegistration l_hdMsgRegisterA;
static HdMsgRegistration l_hdMsgRegisterB;
static HdMsgRegistration l_hdMsgRegisterC;
static u8 l_hdMsgObjectA;
static u8 l_hdMsgObjectB;
extern void *hdMsgText_allocate(u32);
extern void hdMsgText_copyBytes(char *, const char *, u32, u32);
extern void hdMsgText_format(HdMsgString *, const char *, u32);
extern HdMsgPlayState *hdMsgText_getPlay();
extern s32 hdMsgText_getStyle(void *, u32);
extern HdMsgProject *hdMsgText_getProject(void *);
extern HdMsgFont *hdMsgText_getFont(void *, u32);
extern void hdMsgText_register(HdMsgRegistration *);
extern void hdMsgText_constructFirst(be<u8> *);
extern void hdMsgText_constructSecond(be<u8> *);
// HD: inline safe-string construction preserves the emitted wrapping placement checks.
static s32 textLength(const char *text) {
    s32 length = 0;
    if (!*gabi::at<be<u8>>(gabi::ea(text)))
        return 0;
    while (true) {
        ++length;
        if (length > 0x40000)
            return 0;
        ++text;
        if (!*gabi::at<be<u8>>(gabi::ea(text)))
            return length;
    }
}
static void assureTermination(HdMsgString *string) {
    gabi::call_ptr<void>(string->mpVTable[5].get(), string);
}
static void copyString(HdMsgString *destination, char *buffer, HdMsgString *source) {
    assureTermination(source);
    s32 length = textLength(source->mpBuffer.get());
    const s32 capacity = destination->mCapacity;
    const u32 terminate = source->mpVTable[5].get();
    if (length >= capacity)
        length = capacity - 1;
    gabi::call_ptr<void>(terminate, source);
    hdMsgText_copyBytes(buffer, source->mpBuffer.get(), u32(length), 0);
    *gabi::at<be<u8>>(gabi::ea(buffer) + u32(length)) = 0;
}
static void fixed256(HdMsgFixedString *string) {
    if (!string) {
        string = static_cast<HdMsgFixedString *>(hdMsgText_allocate(sizeof(HdMsgFixedString)));
        if (!string)
            return;
    }
    HdMsgString *base = gabi::at<HdMsgString>(gabi::ea(string));
    if (!base)
        base = static_cast<HdMsgString *>(hdMsgText_allocate(sizeof(HdMsgString)));
    if (base) {
        base->mpBuffer = gabi::at<char>(gabi::ea(string) + 12);
        base->mpVTable = reinterpret_cast<gabi::gfn *>(&l_hdMsgStringVTable);
        base->mCapacity = 256;
        string->mBuffer[255] = 0;
    }
    char *top = string->mString.mpBuffer.get();
    string->mString.mpVTable = reinterpret_cast<gabi::gfn *>(&l_hdMsgBufferedVTable);
    *gabi::at<be<u8>>(gabi::ea(top)) = 0;
    string->mString.mpVTable = reinterpret_cast<gabi::gfn *>(&l_hdMsgFixedVTable);
}
static void wide128(HdMsgWideRuby *string) {
    if (!string) {
        string = static_cast<HdMsgWideRuby *>(hdMsgText_allocate(sizeof(HdMsgWideRuby)));
        if (!string)
            return;
    }
    string->mString.mpVTable = reinterpret_cast<gabi::gfn *>(&l_hdMsgRubyVTable);
    string->mBuffer[127] = 0;
    string->mBuffer[0] = 0;
    string->mString.mCapacity = 128;
    string->mString.mpBuffer = reinterpret_cast<char *>(&string->mBuffer[0]);
}
static void fillLines(gabi::be<u32> *lines, u32 value) {
    for (u32 i = 0; i < 5; ++i)
        lines[i] = value;
}
/* 025F65F4: constructor */
/* 025F65F4 */
u32 hdMsgText_ctor(HdMsgText *p) {
    if (!p) {
        p = static_cast<HdMsgText *>(hdMsgText_allocate(sizeof(HdMsgText)));
        if (!p)
            return 0;
    }
    p->mNextState = HD_MSG_TEXT_WAIT;
    p->mpMessageSet = 0;
    p->mState = HD_MSG_TEXT_WAIT;
    fixed256(gabi::at<HdMsgFixedString>(gabi::ea(p) + 0xC));
    p->mMessageNumber = 0;
    p->mLabelIndex = 0xFFFFFFFF;
    fixed256(gabi::at<HdMsgFixedString>(gabi::ea(p) + 0x120));
    p->mpEntry = 0;
    {
        HdMsgWideText *text = gabi::at<HdMsgWideText>(gabi::ea(p) + 0x230);
        if (!text)
            text = static_cast<HdMsgWideText *>(hdMsgText_allocate(sizeof(HdMsgWideText)));
        if (text) {
            text->mBuffer[511] = 0;
            text->mString.mpBuffer = reinterpret_cast<char *>(&text->mBuffer[0]);
            text->mBuffer[0] = 0;
            text->mString.mpVTable = reinterpret_cast<gabi::gfn *>(&l_hdMsgTextVTable);
            text->mString.mCapacity = 512;
        }
    }
    p->mLineReady = 0;
    p->mLayoutProgress = 0;
    p->mLayoutMode = 0;
    p->mCurrentTag = 0xFFFFFFFF;
    p->mTextStyle = 0;
    p->mWaitComplete = 0;
    p->mCharacterIndex = 0;
    p->mLayoutComplete = 0;
    p->mLayoutLimit = 0x14F;
    p->mLineCount = 0;
    if (!(gabi::ea(p) + 0x660))
        hdMsgText_allocate(0x14u);
    if (!(gabi::ea(p) + 0x674))
        hdMsgText_allocate(0x14u);
    p->mRubyState = 0;
    p->mFlag698 = 0;
    p->mSoundIndex = 0;
    p->mSoundMode = 1;
    p->mFlag695 = 0;
    p->mDrawIndex = 0;
    p->mFlag696 = 0;
    p->mDrawEnabled = 0;
    p->mFlag69A = 0;
    p->mFlag69B = 0;
    p->mSoundState = 0;
    p->mpFont = 0;
    p->mCurrentRuby = 0;
    p->mFlag69C = 0;
    p->mFlag697 = 0;
    const f32 zero = l_hdMsgZero;
    p->mFlag699 = 0;
    p->mLayoutWidth = zero;
    wide128(gabi::at<HdMsgWideRuby>(gabi::ea(p) + 0x6C0));
    wide128(gabi::at<HdMsgWideRuby>(gabi::ea(p) + 0x7CC));
    p->mScaleX = zero;
    p->mFontState = 0;
    const f32 one = l_hdMsgOne;
    p->mScaleY = zero;
    const f32 minus1 = l_hdMsgMinusOne;
    p->mCurrentLine = 0xFFFFFFFF;
    p->mFontScale = one;
    p->mCurrentLineWidth = zero;
    p->mPictographFont = 0;
    p->mPreviousLineWidth = minus1;
    if (!(gabi::ea(p) + 0x8F8))
        hdMsgText_allocate(0x14u);
    p->mFlag90C = 0;
    p->mFlag90D = 0;
    p->mPackedValue3 = 0xFFFFFFFF;
    p->mPackedValue1 = 0xFFFFFFFF;
    p->mFlag90F = 0;
    p->mPackedValue2 = 0xFFFFFFFF;
    p->mFlag922 = 0;
    p->mWaitState = 0;
    p->mFlag914 = 0;
    p->mFlag90E = 0;
    p->mFlag923 = 0;
    char *s1 = p->mSetName.mString.mpBuffer.get();
    p->mFlag921 = 0;
    *gabi::at<be<u8>>(gabi::ea(s1)) = 0;
    *gabi::at<be<u8>>(gabi::ea(p->mLabel.mString.mpBuffer.get())) = 0;
    *gabi::at<gabi::be<u16>>(gabi::ea(p->mText.mString.mpBuffer.get())) = 0;
    fillLines(p->mLineValues, 0xFFFFFFFF);
    for (u32 i = 0; i < 5; i++)
        p->mLineWidths[i] = minus1;
    *gabi::at<gabi::be<u16>>(gabi::ea(p->mRubyText.mString.mpBuffer.get())) = 0;
    *gabi::at<gabi::be<u16>>(gabi::ea(p->mRubyReading.mString.mpBuffer.get())) = 0;
    fillLines(p->mLineFlags, 0);
    return gabi::ea(p);
}

/* 025F69A4: clears the message (strings, ids, layout state) */
/* 025F69A4 */
void hdMsgText_clear(HdMsgText *p) {
    char *s1 = p->mSetName.mString.mpBuffer.get();
    p->mpMessageSet = 0;
    *gabi::at<be<u8>>(gabi::ea(s1)) = 0;
    char *s2 = p->mLabel.mString.mpBuffer.get();
    p->mMessageNumber = 0;
    p->mLabelIndex = 0xFFFFFFFF;
    *gabi::at<be<u8>>(gabi::ea(s2)) = 0;
    char *s3 = p->mText.mString.mpBuffer.get();
    p->mpEntry = 0;
    *gabi::at<gabi::be<u16>>(gabi::ea(s3)) = 0;
    p->mDrawEnabled = 0;
    p->mLineReady = 0;
    p->mFlag69C = 0;
    p->mLayoutComplete = 0;
    p->mLineCount = 0;
    p->mTextStyle = 4;
    p->mFlag69A = 0;
    p->mFlag698 = 0;
    p->mFlag699 = 0;
    p->mFlag697 = 0;
    p->mWaitComplete = 0;
    p->mCurrentTag = 0xFFFFFFFF;
    p->mLayoutProgress = 0;
    p->mFlag696 = 0;
    p->mFlag695 = 0;
    p->mCharacterIndex = 0;
    p->mLayoutMode = 0;
    p->mLayoutLimit = 0x14F;
    p->mFlag69B = 0;
    p->mSoundPosition[0] = l_hdMsgSoundX;
    p->mSoundPosition[1] = l_hdMsgSoundY;
    const u32 c2 = l_hdMsgSoundZ;
    p->mSoundState = 0;
    p->mSoundPosition[2] = c2;
    p->mSoundMode = 1;
    p->mSoundIndex = 0;
    p->mRubyState = 0;
    char *w1 = p->mRubyText.mString.mpBuffer.get();
    p->mCurrentRuby = 0;
    *gabi::at<gabi::be<u16>>(gabi::ea(w1)) = 0;
    p->mFontState = 0;
    p->mFlag90D = 0;
    const f32 zero = l_hdMsgZero, one = l_hdMsgOne;
    p->mScaleY = zero;
    p->mScaleX = zero;
    p->mCurrentLineWidth = zero;
    p->mFlag90F = 0;
    const f32 minus1 = l_hdMsgMinusOne;
    p->mFontScale = one;
    p->mPreviousLineWidth = minus1;
    p->mFlag90C = 0;
    p->mCurrentLine = 0xFFFFFFFF;
    p->mFlag90E = 0;
    p->mFlag914 = 0;
    fillLines(p->mLineValues, 0xFFFFFFFF);
    for (u32 i = 0; i < 5; i++)
        p->mLineWidths[i] = minus1;
    fillLines(p->mLineFlags, 0);
    p->mFlag922 = 0;
    p->mFlag921 = 0;
    p->mFlag923 = 0;
    p->mPictographFont = 0;
}

/* 025F6B04 / 025F6C40: clear with the two state words set */
/* 025F6B04 */
void hdMsgText_reset(HdMsgText *p) {
    p->mState = HD_MSG_TEXT_WAIT;
    p->mNextState = HD_MSG_TEXT_IDLE;
    hdMsgText_clear(p);
}

/* 025F6C40 */
void hdMsgText_resetIdle(HdMsgText *p) {
    p->mState = HD_MSG_TEXT_IDLE;
    p->mNextState = HD_MSG_TEXT_WAIT;
    hdMsgText_clear(p);
}

/* label = "%05d" of the message number */
static void setNumberLabel(HdMsgText *p, u32 number, const char *format) {
    // HD: the formatter receives the same 0x110-byte stack allocation and string header.
    gabi::Local<HdMsgLabelFrame> storage;
    HdMsgLabelFrame &label = *gabi::at<HdMsgLabelFrame>(storage.a);
    label.mString.mBuffer[255] = 0;
    label.mString.mString.mpBuffer = reinterpret_cast<char *>(&label.mString.mBuffer[0]);
    label.mString.mString.mCapacity = 256;
    label.mString.mBuffer[0] = 0;
    label.mString.mString.mpVTable = reinterpret_cast<gabi::gfn *>(&l_hdMsgFixedVTable);
    hdMsgText_format(&label.mString.mString, format, number & 0xFFFF);
    p->mMessageNumber = number;
    char *destination = p->mLabel.mString.mpBuffer.get();
    copyString(&p->mLabel.mString, destination, &label.mString.mString);
}

/* 025F6B18: message by number, with an optional sound position */
/* 025F6B18 */
void hdMsgText_setNumber(HdMsgText *p, u32 num, const HdMsgSoundPosition *sePos) {
    hdMsgText_reset(p);
    setNumberLabel(p, num, "message number format");
    if (sePos) {
        p->mSoundPosition[0] = sePos->mBits[0];
        p->mSoundPosition[1] = sePos->mBits[1];
        p->mSoundPosition[2] = sePos->mBits[2];
    }
}

/* 025F6C54: message by number (idle state) */
/* 025F6C54 */
void hdMsgText_setNumberIdle(HdMsgText *p, u32 num) {
    hdMsgText_resetIdle(p);
    setNumberLabel(p, num, "idle message number format");
}

/* 025F6D70: message by set name and label */
/* 025F6D70 */
void hdMsgText_setLabel(HdMsgText *p, HdMsgString *name, HdMsgString *label) {
    hdMsgText_reset(p);
    copyString(&p->mSetName.mString, p->mSetName.mString.mpBuffer.get(), name);
    copyString(&p->mLabel.mString, p->mLabel.mString.mpBuffer.get(), label);
}

/* 025F6EC8: reset */
/* 025F6EC8 */
void hdMsgText_reset2(HdMsgText *p) { hdMsgText_reset(p); }

/* 025F6ECC: restarts the text layout (keeps the message) */
/* 025F6ECC */
u32 hdMsgText_restartLayout(HdMsgText *p, u32 layoutMode, u32 layoutLimit) {
    p->mLayoutLimit = layoutLimit;
    p->mLayoutProgress = 0;
    const f32 zero = l_hdMsgZero;
    p->mFlag90F = 0;
    p->mScaleX = zero;
    p->mLayoutMode = layoutMode;
    p->mScaleY = zero;
    p->mLineReady = 0;
    p->mLayoutComplete = 0;
    p->mFlag90E = 0;
    p->mWaitComplete = 0;
    p->mFlag914 = 0;
    p->mFlag90D = 0;
    p->mLineCount = 0;
    p->mFlag90C = 0;
    fillLines(p->mLineValues, 0xFFFFFFFF);
    const f32 minus1 = l_hdMsgMinusOne;
    for (u32 i = 0; i < 5; i++)
        p->mLineWidths[i] = minus1;
    fillLines(p->mLineFlags, 0);
    p->mFlag922 = 0;
    p->mFlag923 = 0;
    return gabi::ea(p);
}

/* 025F6F68: restarts the layout and the font/scale state */
/* 025F6F68 */
void hdMsgText_restart(HdMsgText *self, u32 layoutMode, u32 layoutLimit) {
    HdMsgText *p = gabi::at<HdMsgText>(hdMsgText_restartLayout(self, layoutMode, layoutLimit));
    char *w = p->mRubyText.mString.mpBuffer.get();
    const f32 minus1 = l_hdMsgMinusOne;
    p->mCurrentRuby = 0;
    p->mRubyState = 0;
    *gabi::at<gabi::be<u16>>(gabi::ea(w)) = 0;
    p->mFontState = 0;
    p->mPreviousLineWidth = minus1;
    const f32 zero = l_hdMsgZero, one = l_hdMsgOne;
    p->mCurrentLineWidth = zero;
    p->mFontScale = one;
    p->mCurrentLine = 0xFFFFFFFF;
}

/* 025F6FCC: restart in state 6 */
/* 025F6FCC */
void hdMsgText_restart6(HdMsgText *p, u32 layoutMode, u32 layoutLimit) {
    p->mState = HD_MSG_TEXT_WAIT;
    hdMsgText_restart(p, layoutMode, layoutLimit);
}

/* 025F6FD8: restart, also setting the play object's message state byte (+0x5BB3) */
/* 025F6FD8 */
void hdMsgText_restartPlay(HdMsgText *p, u32 layoutMode, u32 layoutLimit) {
    p->mNextState = HD_MSG_TEXT_WAIT;
    hdMsgText_getPlay()->mMessageState = 6;
    hdMsgText_restart(p, layoutMode, layoutLimit);
}

/* 025F7040: restart the layout in state 6 */
/* 025F7040 */
void hdMsgText_restartLayout6(HdMsgText *p, u32 layoutMode, u32 layoutLimit) {
    p->mState = HD_MSG_TEXT_WAIT;
    hdMsgText_restartLayout(p, layoutMode, layoutLimit);
}

/* 025F704C: the message's text style from its MSBT attribute (project style table) */
/* 025F704C */
void hdMsgText_updateStyle(HdMsgText *p) {
    HdMsgSet *set = p->mpMessageSet.get();
    const u32 index = u32(s32(p->mLabelIndex));
    if (index >= set->mEntryCount)
        return;
    const s32 style = hdMsgText_getStyle(set->mpData.get(), index);
    if (style < 0)
        return;
    HdMsgProject *project = hdMsgText_getProject(l_hdMsgManager.get());
    u32 entry = gabi::ea(project->mpStyles.get());
    if (u32(style) < project->mStyleCount)
        entry += u32(style) << 4;
    p->mTextStyle = gabi::at<HdMsgStyle>(entry)->mStyle;
}

/* 025F70CC: starts drawing: font (the pictograph font for 0x0C-tag entries), style, scale */
/* 025F70CC */
void hdMsgText_beginDraw(HdMsgText *p, HdMsgFont *font, const HdMsgScale *scale) {
    const s32 idx = s32(p->mLabelIndex);
    p->mDrawIndex = 0;
    p->mpFont = font;
    if (idx >= 0) {
        HdMsgEntry *e = p->mpEntry.get();
        if (e && e->mTag == 0xC && !l_hdMsgSystem.get()->mNoPictographFont) {
            p->mpFont = hdMsgText_getFont(l_hdMsgFontManager.get(), 3);
            p->mPictographFont = 1;
        }
        hdMsgText_updateStyle(p);
    }
    p->mScaleX = scale->mX;
    p->mScaleY = scale->mY;
}

/* 025F7178: advances the current line width by a character of the given font */
/* 025F7178 */
void hdMsgText_advance(HdMsgText *p, u32 ch, HdMsgFont *font) {
    const s32 w = s32(call_ptr<u32>(font->mpVTable[31].get(), font, ch));
    HdMsgFont *cur = p->mpFont.get();
    const u32 fn = cur->mpVTable[7].get();
    const f32 sc = fmuls_ppc(p->mScaleX, p->mFontScale);
    const s32 h = s32(call_ptr<u32>(fn, cur));
    const f32 hf = f32(f64(h)), wf = f32(f64(w));
    const f32 q = sc / hf;
    p->mCurrentLineWidth = fmadds(wf, q, p->mCurrentLineWidth);
}

/* 025F7240: advance by a character of the current font */
/* 025F7240 */
void hdMsgText_advanceCur(HdMsgText *p, const gabi::be<u16> *chp) {
    hdMsgText_advance(p, *chp, p->mpFont.get());
}

/* 025F724C: advance by a character of the pictograph font */
/* 025F724C */
void hdMsgText_advancePic(HdMsgText *p, const gabi::be<u16> *chp) {
    HdMsgFont *f = hdMsgText_getFont(l_hdMsgFontManager.get(), 4);
    hdMsgText_advance(p, *chp, f);
}

/* 025F72A0: ends layoutMode line: records its value and width (at most 5 lines) */
/* 025F72A0 */
void hdMsgText_endLine(HdMsgText *p, u32 v) {
    u32 i = p->mLineCount;
    p->mLineValues[i < 5 ? i : 0] = v;
    i = p->mLineCount;
    const f32 w = p->mCurrentLineWidth;
    p->mLineWidths[i < 5 ? i : 0] = w;
    s32 n = s32(p->mLineCount) + 1;
    if (n > 5)
        n = 5;
    const f32 zero = l_hdMsgZero;
    p->mLineCount = u32(n);
    p->mCurrentLineWidth = zero;
}

/* 025F7304 */
void hdMsgText_staticInit() {
    l_hdMsgWords.mWords[2] = 0;
    l_hdMsgWords.mWords[0] = 0;
    l_hdMsgWords.mWords[3] = 0;
    l_hdMsgWords.mWords[1] = 0;
    hdMsgText_register(&l_hdMsgRegisterA);
    const f32 low = l_hdMsgLow, high = l_hdMsgHigh;
    l_hdMsgRange.mLow = low;
    l_hdMsgRange.mHigh = high;
    hdMsgText_constructFirst(&l_hdMsgObjectA);
    hdMsgText_register(&l_hdMsgRegisterB);
    hdMsgText_constructSecond(&l_hdMsgObjectB);
    hdMsgText_register(&l_hdMsgRegisterC);
}
/* WWHD glue (tools/verify/autoglue.py)
data l_hdMsgStringVTable 100E0A20
data l_hdMsgBufferedVTable 100E0A50
data l_hdMsgFixedVTable 100E0A68
data l_hdMsgRubyVTable 100E0A98
data l_hdMsgTextVTable 100E0A80
data l_hdMsgZero 100E0AB0
data l_hdMsgOne 100E0A00
data l_hdMsgMinusOne 100E0AB4
data l_hdMsgSoundX 101FFBA8
data l_hdMsgSoundY 101FFBAC
data l_hdMsgSoundZ 101FFBB0
data l_hdMsgManager 101F4AE8
data l_hdMsgFontManager 101F4A50
data l_hdMsgSystem 101F84DC
data l_hdMsgWords 1048D70C
data l_hdMsgRange 1048D700
data l_hdMsgLow 100E0AD0
data l_hdMsgHigh 100E0AD4
data l_hdMsgRegisterA 101F4AF0
data l_hdMsgRegisterB 101F4AFC
data l_hdMsgRegisterC 101F4B08
data l_hdMsgObjectA 1048D708
data l_hdMsgObjectB 1048D709
str 100E0AB8 "message number format" renamed
str 100E0AC0 "idle message number format" renamed
ext 0273AD10 void* hdMsgText_allocate(u32)
ext C0009988 void hdMsgText_copyBytes(char*, const char*, u32, u32)
ext 02759C28 void hdMsgText_format(HdMsgString*, const char*, u32)
ext 025200D4 HdMsgPlayState* hdMsgText_getPlay()
ext 0273A598 s32 hdMsgText_getStyle(void*, u32)
ext 025F4BC0 HdMsgProject* hdMsgText_getProject(void*)
ext 025F36B8 HdMsgFont* hdMsgText_getFont(void*, u32)
ext 028F026C void hdMsgText_register(HdMsgRegistration*)
ext 028ED6F8 void hdMsgText_constructFirst(be<u8>*)
ext 028EAB2C void hdMsgText_constructSecond(be<u8>*)
*/
