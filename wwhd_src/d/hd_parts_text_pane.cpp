/** HD text pane: message conversion, fitting and metrics. Private binary-derived source. */
#include "d/hd_parts_text_pane.h"
static f32 Pane_floatBits(u32 bits) {
    f32 value;
    __builtin_memcpy(&value, &bits, 4);
    return value;
}
extern PaneFont_l *Pane_getFont(hdTextPane_c *);

extern s32 Pane_getCapacity(hdTextPane_c *);

extern void Pane_getRect(hdTextPane_c *, void *);

extern void Pane_setFont(hdTextPane_c *, u32);

extern u32 Pane_getMaximumWidth();

extern void Pane_prepareProcessor(PaneProcessor_l *);

extern u32 Pane_convert(PaneProcessor_l *, PaneString_l *, void *, PaneWork_l *, hdTextPane_c *);

extern void *Pane_lookupLabel(void *, u32);

extern void Pane_loadMessage(void *, PaneWork_l *, PaneLayout_l *, void *, hdTextPane_c *);

extern void Pane_prepareArguments(void *, PaneWork_l *, void *, PaneString_l *);

extern void Pane_resetWork(PaneWork_l *, hdUiSafeString *, hdUiSafeString *);

extern void Pane_selectWork(PaneWork_l *, u32, u32);

extern void Pane_clearWork(PaneWork_l *);

extern void Pane_configureWork(PaneWork_l *, u32, void *);

extern void Pane_initializeBase(hdTextPane_c *);

extern void Pane_initializeStyle(hdTextPane_c *);

extern void Pane_formatWarning(PaneString_l *, const char *, void *, void *, u32, s32);

extern void Pane_formatLayout(PaneString_l *, const char *, const char *);

extern void Pane_ensureString(PaneString_l *);

using PaneManagerPointer = gptr<void>;

static PaneManagerPointer sMessageManager;

static u32 sProcessorTypeGuard;

static u32 sBaseTypeGuard;

static PaneType_l sProcessorType;

static PaneType_l sBaseType;

static u32 sByteStringVTable;

static u32 sWideStringVTable;

static u32 sSafeStringVTable;

struct PaneRect_l {
    be<f32> mLeft, mTop, mRight, mBottom;
};

struct PaneSize_l {
    be<f32> mWidth, mHeight;
};

struct Warning_l {
    PaneString_l mString;
    be<u8> mBuffer[0x100];
};

struct Buffer_l {
    PaneString_l mString;
    be<u16> mBuffer[0x200];
};

/* 02703C04 */
void hdTextPane_c::setCapacity(u32 capacity) {
    if (mpText)
        gabi::call_ptr<void>(mpVTable[0xBC / 4].get(), this);

    gabi::call_ptr<void>(mpVTable[0xB4 / 4].get(), this, u32(u16(capacity)));

    mCapacity = capacity;
}
/* 02703C74 */
void hdTextPane_c::setAutomaticCapacity() {
    PaneFont_l *font = Pane_getFont(this);
    s32 capacity = Pane_getCapacity(this);

    if (font) {
        PaneRect_l rect;
        Pane_getRect(this, &rect);

        s32 width = gabi::ftoi(gabi::fsubs_ppc(rect.mRight, rect.mLeft));

        s32 glyph = gabi::call_ptr<s32>(font->mpVTable[0x7C / 4].get(), font, 0x69);

        s32 divisor = gabi::ftoi(gabi::fmuls_ppc(f32(glyph), mGlyphScale));

        capacity =
            divisor && !(width == s32(0x80000000) && divisor == -1) ? width / divisor : (width < 0 ? -1 : 0);
    }
    setCapacity(u32(capacity) * 3);
}
/* 02703D4C */
void hdTextPane_c::setFontPreservingSize(u32 font) {
    f32 height = mFontHeight, width = mFontWidth;
    Pane_setFont(this, font);

    bool changed = !(f32(mFontWidth) == width && f32(mFontHeight) == height);

    mTextFlags = u8(mTextFlags) | (changed ? 0x20 : 0);

    if (changed) {
        mFontHeight = height;
        mFontWidth = width;
    }
}
/* 02703DF4 */
void hdTextPane_c::fitText() {
    f32 width = mWidth, height = mHeight;
    u32 maximum = Pane_getMaximumWidth();

    u8 flags = mTextFlags;
    mPaneFlags = u8(mPaneFlags) | 0x10;
    mHeight = height;

    mTextFlags = flags & ~2;
    u8 saved = flags & 2;
    mWidth = f32(maximum);

    PaneRect_l rect;
    Pane_getRect(this, &rect);

    if (width < gabi::fsubs_ppc(rect.mRight, rect.mLeft)) {
        f32 spacing = mCharSpace;

        bool changed = spacing != 0.0f;
        mTextFlags = u8(mTextFlags) | (changed ? 0x20 : 0);

        if (changed)
            mCharSpace = 0.0f;

        PaneRect_l unspaced;
        Pane_getRect(this, &unspaced);

        changed = f32(mCharSpace) != spacing;
        mTextFlags = u8(mTextFlags) | (changed ? 0x20 : 0);

        if (changed)
            mCharSpace = spacing;

        f32 textWidth = gabi::fsubs_ppc(unspaced.mRight, unspaced.mLeft);

        if (textWidth > 0.0f) {
            f32 difference = gabi::fsubs_ppc(gabi::fsubs_ppc(rect.mRight, rect.mLeft), textWidth);

            f32 scale = gabi::fsubs_ppc(gabi::fsubs_ppc(width, difference), 4.0f) / textWidth;

            f32 defaultWidth = Pane_floatBits(u32(mDefaultWidthBits));

            f32 newWidth = gabi::fmuls_ppc(defaultWidth, scale), minimum = mMinimumWidth;

            f32 newHeight = Pane_floatBits(u32(mDefaultHeightBits));

            if (newWidth < minimum)
                newWidth = minimum;

            if (newWidth < defaultWidth) {
                PaneProcessor_l *processor = mpProcessor;

                if (!sProcessorTypeGuard) {
                    sProcessorTypeGuard = 1;
                    if (!sBaseTypeGuard) {
                        sBaseTypeGuard = 1;
                        sBaseType.mpParent = NULL;
                    }
                    sProcessorType.mpParent = &sBaseType;
                }
                bool kind = false;

                if (processor) {
                    PaneType_l *type = gabi::call_ptr<PaneType_l *>(processor->mpVTable[3].get(), processor);

                    while (type) {
                        if (type == &sProcessorType) {
                            kind = true;
                            break;
                        }
                        type = type->mpParent;
                    }
                }
                Pane_prepareProcessor(kind ? processor : NULL);

                changed = !(f32(mFontWidth) == newWidth && f32(mFontHeight) == newHeight);

                mTextFlags = u8(mTextFlags) | (changed ? 0x20 : 0);

                if (changed) {
                    mFontHeight = newHeight;
                    mFontWidth = newWidth;
                }
            }
        }
    }
    u8 finalFlags = mTextFlags;
    mWidth = width;
    mTextFlags = (finalFlags & ~2) | saved;

    u8 paneFlags = mPaneFlags;
    mHeight = height;
    mPaneFlags = paneFlags | 0x10;
}
/* 02704100 */
void hdTextPane_c::applyText(PaneString_l *text, u32 length, u32 unused, PaneWork_l *work, u32 grow) {
    if (Pane_getCapacity(this) < s32(length)) {
        if (grow) {
            mCapacity = length;
            setCapacity(length);
        } else {
            Warning_l warning;

            warning.mBuffer[0] = 0;
            warning.mBuffer[255] = 0;
            warning.mString.mpBuffer = warning.mBuffer;

            warning.mString.mpVTable = reinterpret_cast<gfn *>(&sByteStringVTable);
            warning.mString.mCapacity = 0x100;

            gabi::call_ptr<void>(work->mLabelName.mpVTable[5].get(), &work->mLabelName);

            void *label = work->mLabelName.mpBuffer;

            s32 capacity = Pane_getCapacity(this);

            Pane_formatWarning(&warning.mString, "[WARNING!]\tTPane:%s, TextLabel:%s, %d/%d\n", mName, label,
                               length, capacity);

            gabi::call_ptr<void>(warning.mString.mpVTable[5].get(), &warning.mString);

            Pane_formatLayout(&warning.mString, "[WARNING!]\tLayout:%s, Text Buffer Over\n",
                              mpLayout->mpName.get());

            gabi::call_ptr<void>(warning.mString.mpVTable[5].get(), &warning.mString);
        }
    }
    f32 defaultWidth = Pane_floatBits(u32(mDefaultWidthBits));
    f32 defaultHeight = Pane_floatBits(u32(mDefaultHeightBits));

    bool changed = !(f32(mFontWidth) == defaultWidth && f32(mFontHeight) == defaultHeight);

    mTextFlags = u8(mTextFlags) | (changed ? 0x20 : 0);

    if (changed) {
        mFontWidth = Pane_floatBits(u32(mDefaultWidthBits));
        mFontHeight = Pane_floatBits(u32(mDefaultHeightBits));
    }
    gfn *vtable = mpVTable.get();
    gabi::call_ptr<void>(text->mpVTable[5].get(), text);

    gabi::call_ptr<void>(vtable[0xCC / 4].get(), this, text->mpBuffer.get(), 0, u32(u16(length)));

    if (mShrink)
        fitText();
}
/* 027042D0 */
void hdTextPane_c::convertText(PaneString_l *text, void *label, u32 grow, PaneWork_l *work) {
    PaneProcessor_l *processor = mpProcessor;
    *static_cast<be<u16> *>(work->mText.mpBuffer.get()) = 0;

    PaneSize_l size;
    size.mWidth = mFontWidth;
    size.mHeight = mFontHeight;

    Pane_configureWork(work, mFont, &size);
    u8 shrink = mShrink;
    u32 font = work->mFont;
    work->mShrink = shrink;

    setFontPreservingSize(font);
    u32 length = Pane_convert(processor, text, label, work, this);

    u32 message = work->mLabel;
    work->mLength = length;
    applyText(text, length, message, work, grow);
}
/* 02704384 */
void hdTextPane_c::setLabel(PaneWork_l *work, u32 grow) {
    PaneLabelTable_l *table = work->mpLabels;
    if (table) {
        u32 label = work->mLabel;
        void *found = NULL;

        if (label < u32(table->mCount))
            found = Pane_lookupLabel(table->mpData, label);

        convertText(&work->mText, found, grow, work);
    }
}
/* 02704404 */
u32 hdTextPane_c::loadMessage() {
    Pane_loadMessage(sMessageManager, &mWork, mpLayout, mMessageParameters, this);

    if (!mWork.mpLabels)
        return 0;
    mWork.mMessage = 0x14F;
    mWork.mForce = 1;
    setLabel(&mWork, 1);
    return 1;
}
/* 02704484 */
void hdTextPane_c::initializeMessage() {
    Pane_initializeBase(this);
    Pane_initializeStyle(this);
    setAutomaticCapacity();

    if (!loadMessage() && mpFallback)
        gabi::call_ptr<void>(mpVTable[0xCC / 4].get(), this, mpFallback.get(), 0,
                             u32(u16((u32(mFallbackSize) >> 1) - 1)));
}
/* 02704504 */
void hdTextPane_c::prepareMessage(PaneWork_l *work, u32 force) {
    Pane_prepareArguments(sMessageManager, work, work->mArguments, &work->mLabelName);

    if (force)
        work->mForce = 1;
    work->mColor = mColor;
    setLabel(work, mCapacity == 0 ? 1 : 0);
}
/* 027045B0 */
void hdTextPane_c::resetMessage(hdUiSafeString* message, hdUiSafeString* label) {
    // HD: the message and label contexts go on to the work reset (025F6D70 reads r3-r5).
    Pane_resetWork(&mWork, message, label);
    prepareMessage(&mWork, 1);
}
/* 027045F0 */
void hdTextPane_c::selectMessage(u32 label) {
    Pane_selectWork(&mWork, label, 0);
    prepareMessage(&mWork, 1);
}
/* 02704634 */
void hdTextPane_c::setString(PaneString_l *text, u32 grow, PaneWork_l *work) {
    Buffer_l buffer;

    buffer.mBuffer[0] = 0;
    buffer.mBuffer[0x1FF] = 0;
    buffer.mString.mpVTable = reinterpret_cast<gfn *>(&sWideStringVTable);

    buffer.mString.mCapacity = 0x200;
    buffer.mString.mpBuffer = buffer.mBuffer;

    gabi::call_ptr<void>(text->mpVTable[5].get(), text);

    convertText(&buffer.mString, text->mpBuffer, grow, work);
}
/* 027046D4 */
void hdTextPane_c::setStringPrepared(PaneString_l *text, PaneWork_l *work) {
    if (!text)
        return;

    if (!work) {
        work = &mWork;
        Pane_clearWork(work);
    }
    work->mForce = 1;
    work->mColor = mColor;
    setString(text, mCapacity == 0 ? 1 : 0, work);
}
/* 0270477C */
void hdTextPane_c::refresh(PaneWork_l *work) {
    PaneSize_l size;
    size.mWidth = mFontWidth;
    size.mHeight = mFontHeight;

    Pane_configureWork(work, mFont, &size);
    work->mColor = mColor;
    setFontPreservingSize(work->mFont);

    applyText(&work->mText, work->mLength, work->mLabel, work, 0);
}
/* 02704820 */
f32 hdTextPane_c::getTextWidth(be<u16> *text) {
    if (!text)
        return 0.0f;
    f32 line = 0.0f, maximum = 0.0f;

    while (u16 ch = *text) {
        if (ch == 14) {
            text = reinterpret_cast<be<u16> *>(reinterpret_cast<u8 *>(text) + u16(text[3]) + 8);
            continue;
        }
        if (ch == 15) {
            text += 3;
            continue;
        }
        if (ch == 10) {
            if (!(line <= maximum))
                maximum = line;
            ++text;
            line = 0.0f;
            continue;
        }
        PaneFont_l *font = Pane_getFont(this);
        s32 advance = gabi::call_ptr<s32>(font->mpVTable[0x7C / 4].get(), font, u32(u16(*text)));

        ++text;
        line = gabi::fadds_ppc(line, f32(advance));
    }
    if (!(line <= maximum))
        maximum = line;
    PaneFont_l *font = Pane_getFont(this);

    s32 width = gabi::call_ptr<s32>(font->mpVTable[0x1C / 4].get(), font);

    return gabi::fmuls_ppc(maximum, f32(mFontWidth) / f32(width));
}
/* 02704A08 */
f32 hdTextPane_c::getTextWidth() {
    return getTextWidth(mpText);
}
/* 02704A10 */
f32 hdTextPane_c::getTextHeight(be<u16> *text, s32 count) {
    f32 height = mFontHeight;

    if (count < 0) {
        PaneString_l string;
        string.mpBuffer = text;
        string.mpVTable = reinterpret_cast<gfn *>(&sSafeStringVTable);

        Pane_ensureString(&string);
        be<u16> *cursor = static_cast<be<u16> *>(string.mpBuffer.get());
        count = 0;

        while (u16(*cursor)) {
            ++count;
            ++cursor;
            if (count > 0x40000) {
                count = 0;
                break;
            }
        }
    }
    for (s32 i = 0; i < count; ++i)
        if (u16(text[i]) == 10) {
            height = gabi::fadds_ppc(height, mFontHeight);
            height = gabi::fadds_ppc(height, mLineSpace);
        }
    return height;
}
/* 02704AF0 */
f32 hdTextPane_c::getTextHeight(s32 count) {
    return getTextHeight(mpText, count);
}
/* 02704AFC */
void hdTextPane_c::setCharacterCapacity(u32 count) {
    setCapacity(count * 3);
}
struct PaneHeader_l {
    be<u32> mWords[4];
};
struct PaneRange_l {
    be<f32> mLow, mHigh;
};
struct PaneRegistration_l {
    u8 mBytes[12];
};

static PaneHeader_l sHeader;

static PaneRange_l sRange;

static f32 sTextPaneLow;

static f32 sTextPaneHigh;

static u8 sFirst;

static u8 sSecond;

static PaneRegistration_l sTextPaneRegistrationA;

static PaneRegistration_l sTextPaneRegistrationB;

static PaneRegistration_l sTextPaneRegistrationC;

extern void Pane_register(PaneRegistration_l *);
extern void Pane_constructFirst(be<u8> *);
extern void Pane_constructSecond(be<u8> *);

/* 02704B04 */
static void __sinit_hd_parts_text_pane_cpp() {
    sHeader.mWords[2] = 0;
    sHeader.mWords[0] = 0;
    sHeader.mWords[3] = 0;
    sHeader.mWords[1] = 0;

    Pane_register(&sTextPaneRegistrationA);
    f32 low = sTextPaneLow, high = sTextPaneHigh;
    sRange.mLow = low;
    sRange.mHigh = high;

    Pane_constructFirst(&sFirst);
    Pane_register(&sTextPaneRegistrationB);
    Pane_constructSecond(&sSecond);
    Pane_register(&sTextPaneRegistrationC);
}
/* WWHD glue
 data sMessageManager 101F4AE8
 data sProcessorTypeGuard 101FDD54
 data sProcessorType 101FDD58
 data sBaseTypeGuard 101FDD5C
 data sBaseType 101FDD60
 data sByteStringVTable 1010BE50
 data sWideStringVTable 1010BE68
 data sSafeStringVTable 1010BE20
 data sHeader 1049E798
 data sRange 1049E78C
 data sTextPaneLow 1010BFF0
 data sTextPaneHigh 1010BFF4
 data sFirst 1049E794
 data sSecond 1049E795
 data sTextPaneRegistrationA 101F73CC
 data sTextPaneRegistrationB 101F73D8
 data sTextPaneRegistrationC 101F73E4
 str 1010BFC4
 str 1010BF9C
 ext 02878098 PaneFont_l* Pane_getFont(hdTextPane_c*)
 ext 02877834 s32 Pane_getCapacity(hdTextPane_c*)
 ext 028782A8 void Pane_getRect(hdTextPane_c*,void*)
 ext 02877668 void Pane_setFont(hdTextPane_c*,u32)
 ext 02739398 u32 Pane_getMaximumWidth()
 ext 026026E8 void Pane_prepareProcessor(PaneProcessor_l*)
 ext 02602720 u32 Pane_convert(PaneProcessor_l*,PaneString_l*,void*,PaneWork_l*,hdTextPane_c*)
 ext 0273A2E8 void* Pane_lookupLabel(void*,u32)
 ext 025F5444 void Pane_loadMessage(void*,PaneWork_l*,PaneLayout_l*,void*,hdTextPane_c*)
 ext 025F50E8 void Pane_prepareArguments(void*,PaneWork_l*,void*,PaneString_l*)
 ext 025F6D70 void Pane_resetWork(PaneWork_l*, hdUiSafeString*, hdUiSafeString*)
 ext 025F6B18 void Pane_selectWork(PaneWork_l*,u32,u32)
 ext 025F6EC8 void Pane_clearWork(PaneWork_l*)
 ext 025F70CC void Pane_configureWork(PaneWork_l*,u32,void*)
 ext 02703764 void Pane_initializeBase(hdTextPane_c*)
 ext 02703B00 void Pane_initializeStyle(hdTextPane_c*)
 ext 02759C28 void Pane_formatWarning(PaneString_l*,const char*,void*,void*,u32,s32)
 ext 02759C28 void Pane_formatLayout(PaneString_l*,const char*,const char*)
 ext 02704BDC void Pane_ensureString(PaneString_l*)
 ext 028F026C void Pane_register(PaneRegistration_l*)
 ext 028ED6F8 void Pane_constructFirst(be<u8>*)
 ext 028EAB2C void Pane_constructSecond(be<u8>*)
*/
