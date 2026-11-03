/* LibMessageStudio readers: the message binary (MsgStdBn) and the project binary (MsgPrjBn).
 * Layouts reconstructed from this game's instructions only; no external reference used.
 * Both readers sit on sead's chunked-resource index (sead/ChunkResources.h). */
#pragma once
#include "wwhd.h"
#include "sead/ChunkResources.h"

namespace lms {

using ChunkEntry = sead::chunk_resource::Entry;
using ChunkIndex = sead::chunk_resource::Index;

enum { cNoSection = -1 };

/* code-unit width of the text in a message binary (ChunkIndex::mFormat) */
enum TextEncoding {
    cTextEncoding_8bit = 0,
    cTextEncoding_16bit = 1,
    cTextEncoding_32bit = 2,
};

/* control codes inside message text */
enum TextControl {
    cTextControl_End = 0x00,       // terminator
    cTextControl_TagBegin = 0x0E,  // tag: group, type, parameter size, parameters
    cTextControl_TagEnd = 0x0F,    // closing tag: group, type
};

/* error results of the readers */
enum Result {
    cResult_NotFound = -1,
    cResult_NoLabelSection = -2,
    cResult_NoStyleSection = -3,
    cResult_NoColorSection = -5,
    cResult_Unset = -11,  // a style's font or color index left unset (-1 in the file)
};

struct MessageBinary : ChunkIndex {
    /* 0x10 */ be<s32> mLabelSection;      // "LBL1": label hash table
    /* 0x14 */ be<s32> mTextSection;       // "TXT2": text offsets
    /* 0x18 */ be<s32> mAttributeSection;  // "ATR1": per-text attribute records
    /* 0x1C */ be<s32> mStyleSection;      // "TSY1": per-text style index

    static MessageBinary* open(void* file);
    void close();
    s32 textCount();
    s32 findLabel(const be<u8>* label);
    const be<u8>* text(s32 index);
    s32 textByteLength(s32 index);
    u32 copyLabel(u32 index, be<u8>* output);
    const be<u8>* attributes(u32 index);
    s32 textStyle(u32 index);
};
WWHD_OFFSET(MessageBinary, mLabelSection, 0x10);
WWHD_OFFSET(MessageBinary, mTextSection, 0x14);
WWHD_OFFSET(MessageBinary, mAttributeSection, 0x18);
WWHD_OFFSET(MessageBinary, mStyleSection, 0x1C);
WWHD_SIZE(MessageBinary, 0x20);

/* one record of the project's attribute-info section */
struct AttributeInfo {
    /* 0x00 */ be<s8> mType;
    /* 0x01 */ u8 mReserved[3];
    /* 0x04 */ be<u32> mListIndex;
};
WWHD_OFFSET(AttributeInfo, mListIndex, 4);
WWHD_SIZE(AttributeInfo, 8);

/* one record of the project's style section */
struct StyleInfo {
    /* 0x00 */ be<s32> mRegionWidth;
    /* 0x04 */ be<s32> mLineCount;
    /* 0x08 */ be<s32> mFontIndex;
    /* 0x0C */ be<s32> mBaseColor;
};
WWHD_OFFSET(StyleInfo, mFontIndex, 8);
WWHD_SIZE(StyleInfo, 16);

struct ProjectBinary : ChunkIndex {
    /* 0x10 */ be<s32> mColorLabelSection;      // "CLB1"
    /* 0x14 */ be<s32> mColorSection;           // "CLR1"
    /* 0x18 */ be<s32> mAttributeLabelSection;  // "ALB1"
    /* 0x1C */ be<s32> mAttributeInfoSection;   // "ATI2"
    /* 0x20 */ be<s32> mAttributeListSection;   // "ALI2"
    /* 0x24 */ be<s32> mTagGroupSection;        // "TGG2"
    /* 0x28 */ be<s32> mTagSection;             // "TAG2"
    /* 0x2C */ be<s32> mTagParameterSection;    // "TGP2"
    /* 0x30 */ be<s32> mTagListSection;         // "TGL2"
    /* 0x34 */ be<s32> mStyleSection;           // "SYL3"
    /* 0x38 */ be<s32> mStyleLabelSection;      // "SLB1"
    /* 0x3C */ be<s32> mContentSection;         // "CTI1"

    static ProjectBinary* open(void* file);
    s32 contentCount();
    const be<u8>* content(s32 index);
    u32 colorCount();
    s32 readColor(s32 index, be<u8>* output);
    AttributeInfo* attributeInfo(s32 index);
    s32 attributeType(s32 index);
    s32 attributeListIndex(s32 index);
    u32 attributeCount();
    u32 styleCount();
    const StyleInfo* style(u32 index);
    s32 styleRegionWidth(u32 index);
    s32 styleLineCount(u32 index);
    s32 styleFontIndex(u32 index);
    s32 styleBaseColor(u32 index);
};
WWHD_OFFSET(ProjectBinary, mColorSection, 0x14);
WWHD_OFFSET(ProjectBinary, mAttributeInfoSection, 0x1C);
WWHD_OFFSET(ProjectBinary, mStyleSection, 0x34);
WWHD_OFFSET(ProjectBinary, mContentSection, 0x3C);
WWHD_SIZE(ProjectBinary, 0x40);

}  // namespace lms
