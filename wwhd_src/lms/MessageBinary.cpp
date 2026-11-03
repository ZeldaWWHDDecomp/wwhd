/* LibMessageStudio message binary (MsgStdBn) reader: labels, texts, attributes and text styles.
 * Reconstructed from this game's instructions only; no external reference used.
 * Local game-derived source; */
#include "lms/MessageBinary.h"

using namespace lms;

void readerDelegateDelete(void* storage);  // global operator delete

/* The delegate classes of this translation unit (vtables at 10142E88 / 10142EA8): their deleting
 * destructor and an empty virtual. */

/* 02739FF4 */
static void deleteReaderDelegate(void* delegate, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (delegate != nullptr && (flags & 1)) readerDelegateDelete(delegate);
}

/* 0273A008 */
static void readerDelegateNoOp() {}

/* 0273A0B4 */
static void unusedNoOp() {}

/* 0273A0C4 */
MessageBinary* lms::MessageBinary::open(void* file) {
    gabi::AutoFrame frame{gabi::current_function()};
    MessageBinary* binary = static_cast<MessageBinary*>(sead::chunk_resource::allocate(sizeof(MessageBinary)));
    binary->mData = static_cast<const be<u8>*>(file);
    binary->initialize();
    s32 section = binary->findTag("LBL1");
    binary->mLabelSection = section;
    section = binary->findTag("TXT2");
    binary->mTextSection = section;
    section = binary->findTag("ATR1");
    binary->mAttributeSection = section;
    section = binary->findTag("TSY1");
    binary->mStyleSection = section;
    return binary;
}

/* 0273A15C */
void lms::MessageBinary::close() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mEntries != nullptr) sead::chunk_resource::release(mEntries);
    sead::chunk_resource::release(this);
}

/* 0273A1A0 */
s32 lms::MessageBinary::textCount() {
    if (mTextSection == cNoSection) return -1;
    const be<u32>* offsets = reinterpret_cast<const be<u32>*>((const be<u8>*)mEntries[mTextSection].mData);
    return offsets[0];
}

/* Label section: a bucket count, then one {label count, first label offset} pair per bucket;
 * each label is {u8 length, the characters, u32 text index}. */

/* 0273A1CC */
s32 lms::MessageBinary::findLabel(const be<u8>* label) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mLabelSection == cNoSection) return cResult_NoLabelSection;
    u32 length = 0;
    while (label[length] != 0) ++length;
    ChunkEntry* section = &mEntries[mLabelSection];
    const be<u32>* buckets = reinterpret_cast<const be<u32>*>((const be<u8>*)section->mData);
    u32 bucket = sead::chunk_resource::hashName(label, buckets[0]);
    buckets = reinterpret_cast<const be<u32>*>((const be<u8>*)section->mData);
    u32 count = buckets[2 * bucket + 1];
    u32 offset = buckets[2 * bucket + 2];
    while (count != 0) {
        const be<u8>* body = section->mData;
        u32 labelLength = body[offset];
        if (labelLength == length && sead::chunk_resource::bytesEqual(label, body + offset + 1, labelLength) != 0) {
            body = section->mData;
            return *reinterpret_cast<const be<u32>*>(body + offset + labelLength + 1);
        }
        offset += labelLength + 5;
        --count;
    }
    return cResult_NotFound;
}

/* 0273A2E8 */
const be<u8>* lms::MessageBinary::text(s32 index) {
    if (mTextSection == cNoSection) return nullptr;
    const be<u8>* body = mEntries[mTextSection].mData;
    const be<s32>* offsets = reinterpret_cast<const be<s32>*>(body);
    if (offsets[0] <= index) return nullptr;
    return body + (u32)offsets[(u32)index + 1];
}

/* 0273A32C */
s32 lms::MessageBinary::textByteLength(s32 index) {
    gabi::AutoFrame frame{gabi::current_function()};
    const be<u8>* start = text(index);
    if (start == nullptr) return -1;
    u32 width;
    switch (mFormat) {
    case cTextEncoding_8bit: width = 1; break;
    case cTextEncoding_16bit: width = 2; break;
    case cTextEncoding_32bit: width = 4; break;
    default: return -1;
    }
    u32 offset = 0;
    for (;;) {
        u32 code;
        if (width == 1) code = start[offset];
        else if (width == 2) code = *reinterpret_cast<const be<u16>*>(start + offset);
        else code = *reinterpret_cast<const be<u32>*>(start + offset);
        if (code == cTextControl_End) return offset;
        if (code == cTextControl_TagBegin) {
            // code, u16 group, u16 type, u16 parameter size, parameters
            u16 parameterSize = *reinterpret_cast<const be<u16>*>(start + offset + width + 4);
            offset += parameterSize + width + 6;
        } else if (code == cTextControl_TagEnd) {
            offset += 6;
        } else {
            offset += width;
        }
    }
}

/* 0273A4B4 */
u32 lms::MessageBinary::copyLabel(u32 index, be<u8>* output) {
    gabi::AutoFrame frame{gabi::current_function()};
    // no missing-section check here, unlike the other readers
    ChunkEntry* section = &mEntries[mLabelSection];
    const be<u8>* body = section->mData;
    const be<u32>* buckets = reinterpret_cast<const be<u32>*>(body);
    u32 offset = buckets[0] * 8 + 4;  // labels follow the bucket table
    while (offset < section->mSize) {
        u32 length = body[offset];
        u32 labelIndex = *reinterpret_cast<const be<u32>*>(body + offset + length + 1);
        if (labelIndex == index) {
            sead::chunk_resource::copyBytes(output, body + offset + 1, length);
            output[length] = 0;
            return 1;
        }
        offset += length + 5;
    }
    return 0;
}

/* 0273A574 */
const be<u8>* lms::MessageBinary::attributes(u32 index) {
    // attribute section: u32 count, u32 record size, then the records
    const be<u8>* body = mEntries[mAttributeSection].mData;
    const be<u32>* header = reinterpret_cast<const be<u32>*>(body);
    return body + header[1] * index + 8;
}

/* 0273A598 */
s32 lms::MessageBinary::textStyle(u32 index) {
    if (mStyleSection == cNoSection) return cResult_NoStyleSection;
    const be<s32>* styles = reinterpret_cast<const be<s32>*>((const be<u8>*)mEntries[mStyleSection].mData);
    return styles[index];
}

/* WWHD glue
str 10142F0C "LBL1"
str 10142F14 "TXT2"
str 10142F1C "ATR1"
str 10142F24 "TSY1"
str 10142F2C "MsgStdBn"
ext 027672CC void* sead::chunk_resource::allocate(u32)
ext 027672DC void sead::chunk_resource::release(void* storage)
ext 027671BC void sead::chunk_resource::Index::initialize()
ext 027671F0 s32 sead::chunk_resource::Index::findTag(const char*)
ext 0276727C u32 sead::chunk_resource::hashName(const be<u8>* name, u32 bucketCount)
ext 027672EC u32 sead::chunk_resource::bytesEqual(const be<u8>* left, const be<u8>* right, s32 size)
ext 02767324 void sead::chunk_resource::copyBytes(be<u8>* destination, const be<u8>* source, s32 size)
ext 0273AF40 void readerDelegateDelete(void* storage)
*/
