/* HD wide-string line wrapping; private game-derived source. */
#include "d/hd_text_break.h"
#include "bindings.h"
#include "d/hd_unicode_class.h"
static u32 sTextBreakHeaderStatics;
static u32 sTextBreakHeaderDescriptors;
static u32 sTextBreakHeaderZero;
static u32 sTextBreakPiConstants;
struct hdBreakClass_c {
    be<u32> mValue;
};
struct hdBreakSafeString_c {
    gptr<be<u16>> mText;
    be<u32> mVtable;
};
struct hdBreakData_c {
    u8 mBytes[8];
};
static hdBreakData_c sIteratorVtable;
static hdBreakData_c sStringVtable;
struct hdBreakEmpty_c {
    be<u16> mCode;
};
static hdBreakEmpty_c sEmpty;
extern void *hdTextBreak_allocate(u32 size);
extern void hdTextBreak_copy(be<u16> *destination, be<u16> *source, u32 size, u32 flags);

/* inlined into hdTextBreak_find, hdTextBreak_wrap */
static s32 textLength(be<u16> *text) {
    s32 length = 0;
    while (text[length] != 0) {
        if (++length > 0x40000)
            return 0;
    }
    return length;
}
/* inlined into hdTextBreak_find, hdTextBreak_wrap */
static be<u16> *textAt(hdBreakString_c *string, s32 index) {
    string->assureTermination();
    be<u16> *text = string->mText;
    s32 length = textLength(text);
    return index < 0 || index > length ? &sEmpty.mCode : text + index;
}
/* inlined into hdTextBreak_find */
static hdBreakIterator_c *makeIterator(hdBreakIterator_c *result, hdBreakString_c *string, s32 index) {
    if (!result) {
        result = static_cast<hdBreakIterator_c *>(hdTextBreak_allocate(sizeof(hdBreakIterator_c)));
        if (!result)
            return NULL;
    }
    result->mString = string;
    result->mVtable = gabi::ea(&sIteratorVtable);
    result->mIndex = index;
    return result;
}
/* inlined into hdTextBreak_find */
static hdBreakIterator_c *copyIterator(hdBreakIterator_c *result, hdBreakIterator_c *source) {
    if (!result) {
        result = static_cast<hdBreakIterator_c *>(hdTextBreak_allocate(sizeof(hdBreakIterator_c)));
        if (!result)
            return NULL;
    }
    result->mVtable = gabi::ea(&sIteratorVtable);
    result->mString = source->mString;
    result->mIndex = source->mIndex;
    return result;
}
/* 027060C0 */
u32 hdTextBreak_getClass(u32 code, void *) {
    u32 category = hdUnicode_getClass(code);
    if (category - 33 <= 2)
        return 30;
    if (category == 32)
        return 5;
    if (category == 29 || category == 36 || category == 37)
        return 12;
    if (category == 31)
        return 14;
    return category;
}

/* 02706130 */
hdBreakIterator_c *hdTextBreak_find(hdBreakIterator_c *result, hdBreakIterator_c *begin,
                                    hdBreakIterator_c *end, hdBreakMeasure_c *measure, void *options) {
    if (begin->mString == end->mString && begin->mIndex == end->mIndex)
        return copyIterator(result, begin);
    hdBreakIterator_c fallback;
    fallback.mString = end->mString;
    fallback.mIndex = end->mIndex;
    fallback.mVtable = gabi::ea(&sIteratorVtable);
    hdBreakString_c *string = begin->mString;
    s32 index = begin->mIndex;
    u32 previous = hdTextBreak_getClass(*textAt(string, index), options);
    if (previous == 30)
        return copyIterator(result, begin);
    if (previous == 38)
        previous = 22;
    string = begin->mString;
    index = begin->mIndex;
    hdBreakIterator_c accepted;
    accepted.mString = string;
    accepted.mIndex = index;
    accepted.mVtable = gabi::ea(&sIteratorVtable);
    s32 next = index + 1;
    u32 inSpaces = 0;
    while (string != end->mString || next != end->mIndex) {
        s32 count = next - begin->mIndex;
        u32 category = hdTextBreak_getClass(*textAt(string, next), options);
        bool mandatory = category == 30;
        bool space = category == 38;
        bool permit = false;
        bool direct = false;
        if (mandatory)
            permit = true;
        else if (!space) {
            hdBreakClass_c nextClass;
            nextClass.mValue = category;
            u32 rule = hdUnicode_getBreakRule(previous, &nextClass.mValue);
            if (rule == 0) {
                permit = true;
                direct = true;
            } else if (rule <= 2)
                permit = inSpaces != 0;
        }
        hdBreakString_c *initialString = begin->mString;
        hdBreakMeasureVtable_c *measureTable = measure->mVtable;
        s32 initialIndex = begin->mIndex;
        be<u16> *text = textAt(initialString, initialIndex);
        hdBreakSafeString_c slice;
        slice.mText = text;
        slice.mVtable = gabi::ea(&sStringVtable);
        u32 target = measureTable->mMeasure;
        s32 measured = gabi::call_ptr<s32>(target, measure, &slice, count);
        if (space) {
            if (!measured) {
                accepted.mString = string;
                accepted.mIndex = index;
            }
            inSpaces = 1;
            index = next++;
            continue;
        }
        if (permit) {
            if (measured) {
                if (inSpaces && direct)
                    return copyIterator(result, &accepted);
                hdBreakIterator_c *choice = fallback.mString == end->mString && fallback.mIndex == end->mIndex
                                                ? &accepted
                                                : &fallback;
                return copyIterator(result, choice);
            }
            fallback.mString = string;
            fallback.mIndex = index;
        }
        if (mandatory)
            return makeIterator(result, string, next);
        if (!measured) {
            accepted.mString = string;
            accepted.mIndex = index;
            inSpaces = 0;
            index = next++;
            previous = category;
            continue;
        }
        hdBreakIterator_c *choice =
            fallback.mString == end->mString && fallback.mIndex == end->mIndex ? &accepted : &fallback;
        return copyIterator(result, choice);
    }
    hdBreakString_c *initialString = begin->mString;
    hdBreakMeasureVtable_c *measureTable = measure->mVtable;
    s32 initialIndex = begin->mIndex;
    be<u16> *text = textAt(initialString, initialIndex);
    hdBreakSafeString_c slice;
    slice.mText = text;
    slice.mVtable = gabi::ea(&sStringVtable);
    s32 count = end->mIndex - begin->mIndex;
    u32 target = measureTable->mMeasure;
    if (!gabi::call_ptr<s32>(target, measure, &slice, count))
        return makeIterator(result, string, index);
    if (inSpaces)
        return copyIterator(result, &accepted);
    hdBreakIterator_c *choice =
        fallback.mString == end->mString && fallback.mIndex == end->mIndex ? &accepted : &fallback;
    return copyIterator(result, choice);
}

/* 02706A68 */
u32 hdTextBreak_wrap(hdBreakBuffer_c *output, hdBreakString_c *input, hdBreakMeasure_c *measure,
                     hdBreakOptions_c *options) {
    output->mText[0] = 0;
    if (input->mText[0] == 0)
        return 1;
    input->assureTermination();
    s32 endIndex = textLength(input->mText) + 1;
    bool limited = options->mMaxLines > 0;
    hdBreakString_c *string = input;
    s32 index = 0;
    s32 lines = 0;
    s32 written = 0;
    hdBreakIterator_c next;
    for (;;) {
        hdBreakIterator_c begin;
        begin.mString = string;
        begin.mIndex = index;
        begin.mVtable = gabi::ea(&sIteratorVtable);
        hdBreakIterator_c end;
        end.mString = input;
        end.mIndex = endIndex;
        end.mVtable = gabi::ea(&sIteratorVtable);
        hdTextBreak_find(&next, &begin, &end, measure, &options->mClassOptions);
        s32 last = next.mIndex;
        s32 count = last - index + 1;
        hdBreakString_c *nextString = next.mString;
        be<u16> *source = textAt(string, index);
        be<u16> *destination = output->mText;
        output->assureTermination();
        s32 oldLength = textLength(output->mText);
        s32 position = written;
        if (position < 0) {
            position = oldLength + position + 1;
            if (position < 0)
                position = 0;
        }
        s32 copied = count;
        hdBreakSafeString_c slice;
        slice.mText = source;
        slice.mVtable = gabi::ea(&sStringVtable);
        if (copied < 0) {
            // HD: SafeString termination is virtual even for a local slice.
            gabi::call_ptr<void>(gabi::load<u32>(slice.mVtable + 0x14), &slice);
            copied = textLength(slice.mText);
        }
        s32 available = output->mCapacity - position;
        if (copied >= available)
            copied = available - 1;
        if (copied <= 0) {
            if (static_cast<u32>(count) != 0)
                return 0;
            copied = 0;
        } else {
            be<u16> *start = destination + position;
            gabi::call_ptr<void>(gabi::load<u32>(slice.mVtable + 0x14), &slice);
            hdTextBreak_copy(start, slice.mText, static_cast<u32>(copied) * 2, 0);
            if (position + copied > oldLength)
                destination[position + copied] = 0;
            if (static_cast<u32>(count) != static_cast<u32>(copied))
                return 0;
        }
        written += copied;
        string = nextString;
        ++lines;
        index = last + 1;
        if (nextString == input && index == endIndex)
            return 1;
        if (limited && lines >= options->mMaxLines)
            return 0;
        be<u16> *lineEnd = textAt(nextString, last);
        if (*lineEnd != 10) {
            output->assureTermination();
            be<u16> *text = output->mText;
            s32 length = textLength(text);
            if (length >= output->mCapacity - 1)
                return 0;
            text[length] = 10;
            ++written;
            text[length + 1] = 0;
        }
        if (!options->mSkipSpaces)
            continue;
        while (hdUnicode_getClass(*textAt(string, index)) == 38) {
            ++index;
            if (string == input && index == endIndex) {
                output->assureTermination();
                be<u16> *text = output->mText;
                s32 length = textLength(text);
                text[length - (length > 0 ? 1 : 0)] = 0;
                return 1;
            }
        }
    }
}

/* 02707038 */
static void __sinit_hd_text_break_cpp() {
    sinit_header_statics_rodata(gabi::ea(&sTextBreakHeaderStatics), gabi::ea(&sTextBreakHeaderDescriptors), gabi::ea(&sTextBreakHeaderZero), gabi::ea(&sTextBreakPiConstants));
}

/* WWHD glue
data sTextBreakHeaderStatics 1049E808 nocheck
data sTextBreakHeaderDescriptors 101F7490 nocheck
data sTextBreakHeaderZero 1049E814 nocheck
data sTextBreakPiConstants 1010C120 nocheck
 data sIteratorVtable 1010C110
 data sStringVtable 1010C0F8
 data sEmpty 10000162
 ext 0273AD10 void* hdTextBreak_allocate(u32)
 ext C0009988 void hdTextBreak_copy(be<u16>*, be<u16>*, u32, u32)
*/
