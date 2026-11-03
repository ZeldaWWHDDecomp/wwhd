/* WWHD sead formatter, companion of sead/RuntimeIntegerFormat.cpp (SEAD#15, same translation unit): the
 * %s / %ls field writers that append a byte or UTF-16 string to the formatter's UTF-16 output, and the
 * translation unit's copies of the text-class virtual functions.
 * Reconstructed from this game's instructions. */
#include "sead/RuntimeIntegerFormat_2.h"

using namespace sead;
using namespace sead::format;

static void releaseObject(void* object);

static constexpr u32 kDestroyRelease = 1;

/* the empty texts a NULL argument prints */
typedef be<u8> ByteUnit;
typedef be<u16> WideUnit;
static ByteUnit sEmptyByteText;
static WideUnit sEmptyWideText;

/* appends one unit; a full output only records the overflow */
static bool appendUnit(DigitBuffer* output, u16 unit) {
    s32 count = output->mCount;
    if (count >= static_cast<s32>(output->mCapacity)) {
        output->mOverflow = 1;
        return false;
    }
    output->mText[count] = unit;
    output->mCount = static_cast<s32>(output->mCount) + 1;
    return true;
}

/* the shared body of both writers: Unit is the source character type */
template <class Unit>
static void writeTextField(const FormatState* state, DigitBuffer* output, const be<Unit>* text) {
    u32 flags = state->mFlags;
    bool limited = static_cast<u8>(state->mHasPrecision) != 0;
    u32 length = 0;
    if (flags & FormatState::LeftAligned) {
        /* text first (up to the precision), then blanks up to the width */
        if (limited) {
            while (static_cast<Unit>(text[length]) != 0 && length < static_cast<u32>(state->mPrecision)) {
                appendUnit(output, text[length]);
                ++length;
            }
        } else {
            while (static_cast<Unit>(text[length]) != 0) {
                appendUnit(output, text[length]);
                ++length;
            }
        }
        u32 width = state->mWidth;
        if (length >= width) return;
        for (u32 remaining = width - length; remaining != 0; --remaining) {
            if (!appendUnit(output, ' ')) return;
        }
        return;
    }

    /* right aligned: measure, pad (zeros with the 0 flag), then the text */
    if (limited) {
        while (static_cast<Unit>(text[length]) != 0 && length < static_cast<u32>(state->mPrecision)) ++length;
    } else {
        while (static_cast<Unit>(text[length]) != 0) ++length;
    }
    u32 width = state->mWidth;
    if (length < width) {
        u16 padding = (flags & FormatState::ZeroPadded) ? '0' : ' ';
        for (u32 remaining = width - length; remaining != 0; --remaining) {
            if (!appendUnit(output, padding)) break;
        }
    }
    for (u32 index = 0; index < length; ++index) appendUnit(output, text[index]);
}

/* 0275C724 */
void sead::format::writeByteTextField(const FormatState* state, DigitBuffer* output, const be<u8>* text) {
    if (text == NULL) text = &sEmptyByteText;
    writeTextField(state, output, text);
}

/* 0275C998 */
void sead::format::writeWideTextField(const FormatState* state, DigitBuffer* output, const be<u16>* text) {
    if (text == NULL) text = &sEmptyWideText;
    writeTextField(state, output, text);
}

/* ---- per-TU copies of the text classes ---- */

/* 0275CC08 */
void sead::format_text::Text::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 0275CC1C */
void sead::format_text::Text::assureTerminationImpl() {}

/* 0275CC20 */
void sead::format_text::TextBuffer::assureTerminationImpl() {
    be<u8>* text = mText;
    s32 capacity = mCapacity;
    text[capacity - 1] = 0;
}

/* 0275CC38 */
void sead::format_text::TextBuffer::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 0275CC4C */
void sead::format_text::FixedTextA::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 0275CC60 */
void sead::format_text::FixedTextC::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 0275CC74 */
void sead::format_text::FixedTextB::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 0275CC88 */
void sead::format_text::FixedTextD::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* WWHD glue
 data sEmptyByteText 1015DDB0
 data sEmptyWideText 1015DDB2
 ext 0273AF40 void releaseObject(void*)
*/
