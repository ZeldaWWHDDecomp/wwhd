/* LibMessageStudio project binary (MsgPrjBn) reader: colors, attributes, styles and contents.
 * Reconstructed from this game's instructions only; no external reference used.
 * Local game-derived source; */
#include "lms/MessageBinary.h"

using namespace lms;

/* 0273A5C8 */
ProjectBinary* lms::ProjectBinary::open(void* file) {
    gabi::AutoFrame frame{gabi::current_function()};
    ProjectBinary* binary = static_cast<ProjectBinary*>(sead::chunk_resource::allocate(sizeof(ProjectBinary)));
    binary->mData = static_cast<const be<u8>*>(file);
    binary->initialize();
    s32 section = binary->findTag("CLR1");
    binary->mColorSection = section;
    section = binary->findTag("CLB1");
    binary->mColorLabelSection = section;
    section = binary->findTag("ATI2");
    binary->mAttributeInfoSection = section;
    section = binary->findTag("ALB1");
    binary->mAttributeLabelSection = section;
    section = binary->findTag("ALI2");
    binary->mAttributeListSection = section;
    section = binary->findTag("TGG2");
    binary->mTagGroupSection = section;
    section = binary->findTag("TAG2");
    binary->mTagSection = section;
    section = binary->findTag("TGP2");
    binary->mTagParameterSection = section;
    section = binary->findTag("TGL2");
    binary->mTagListSection = section;
    section = binary->findTag("SYL3");
    binary->mStyleSection = section;
    section = binary->findTag("SLB1");
    binary->mStyleLabelSection = section;
    section = binary->findTag("CTI1");
    binary->mContentSection = section;
    return binary;
}

/* 0273A700 */
s32 lms::ProjectBinary::contentCount() {
    s32 section = mContentSection;
    if (section == cNoSection) return -1;
    const be<s32>* body = reinterpret_cast<const be<s32>*>((const be<u8>*)mEntries[section].mData);
    return body[0];
}

/* 0273A728 */
const be<u8>* lms::ProjectBinary::content(s32 index) {
    s32 section = mContentSection;
    if (section == cNoSection) return nullptr;
    const be<u8>* body = mEntries[section].mData;
    const be<s32>* words = reinterpret_cast<const be<s32>*>(body);
    if (words[0] <= index) return nullptr;
    return body + (u32)words[(u32)index + 1];
}

/* 0273A768 */
u32 lms::ProjectBinary::colorCount() {
    s32 section = mColorSection;
    if (section == cNoSection) return 0;
    const be<u32>* body = reinterpret_cast<const be<u32>*>((const be<u8>*)mEntries[section].mData);
    return body[0];
}

/* 0273A790 */
s32 lms::ProjectBinary::readColor(s32 index, be<u8>* output) {
    s32 section = mColorSection;
    if (section == cNoSection) return cResult_NoColorSection;
    const be<u8>* body = mEntries[section].mData;
    const be<s32>* words = reinterpret_cast<const be<s32>*>(body);
    if (words[0] <= index) return -1;
    const be<u8>* color = body + (u32)index * 4 + 4;
    output[0] = color[0];
    output[1] = color[1];
    output[2] = color[2];
    output[3] = color[3];
    return 0;
}

/* 0273A7F4 */
AttributeInfo* lms::ProjectBinary::attributeInfo(s32 index) {
    s32 section = mAttributeInfoSection;
    if (section == cNoSection) return nullptr;
    const be<u8>* body = mEntries[section].mData;
    const be<s32>* words = reinterpret_cast<const be<s32>*>(body);
    if (words[0] <= index) return nullptr;
    return reinterpret_cast<AttributeInfo*>(const_cast<be<u8>*>(body) + (u32)index * 8 + 4);
}

/* 0273A830 */
s32 lms::ProjectBinary::attributeType(s32 index) {
    gabi::AutoFrame frame{gabi::current_function()};
    AttributeInfo* info = attributeInfo(index);
    return info == nullptr ? 255 : (s32)info->mType;
}

/* 0273A868 */
s32 lms::ProjectBinary::attributeListIndex(s32 index) {
    gabi::AutoFrame frame{gabi::current_function()};
    AttributeInfo* info = attributeInfo(index);
    return info == nullptr ? -1 : (s32)info->mListIndex;
}

/* 0273A898 */
u32 lms::ProjectBinary::attributeCount() {
    s32 section = mAttributeInfoSection;
    if (section == cNoSection) return 0;
    const be<u32>* body = reinterpret_cast<const be<u32>*>((const be<u8>*)mEntries[section].mData);
    return body[0];
}

/* 0273A8C0 */
u32 lms::ProjectBinary::styleCount() {
    s32 section = mStyleSection;
    if (section == cNoSection) return 0;
    const be<u32>* body = reinterpret_cast<const be<u32>*>((const be<u8>*)mEntries[section].mData);
    return body[0];
}

/* 0273A8E8 */
const StyleInfo* lms::ProjectBinary::style(u32 index) {
    s32 section = mStyleSection;
    if (section == cNoSection) return nullptr;
    const be<u8>* body = mEntries[section].mData;
    const be<u32>* words = reinterpret_cast<const be<u32>*>(body);
    if (words[0] <= index) return nullptr;
    return reinterpret_cast<const StyleInfo*>(body + index * 16 + 4);
}

/* 0273A924 */
s32 lms::ProjectBinary::styleRegionWidth(u32 index) {
    gabi::AutoFrame frame{gabi::current_function()};
    const StyleInfo* info = style(index);
    return info == nullptr ? -1 : (s32)info->mRegionWidth;
}

/* 0273A954 */
s32 lms::ProjectBinary::styleLineCount(u32 index) {
    gabi::AutoFrame frame{gabi::current_function()};
    const StyleInfo* info = style(index);
    return info == nullptr ? -1 : (s32)info->mLineCount;
}

/* 0273A984 */
s32 lms::ProjectBinary::styleFontIndex(u32 index) {
    gabi::AutoFrame frame{gabi::current_function()};
    const StyleInfo* info = style(index);
    if (info == nullptr) return -1;
    s32 font = info->mFontIndex;
    return font == -1 ? cResult_Unset : font;
}

/* 0273A9D4 */
s32 lms::ProjectBinary::styleBaseColor(u32 index) {
    gabi::AutoFrame frame{gabi::current_function()};
    const StyleInfo* info = style(index);
    if (info == nullptr) return -1;
    s32 color = info->mBaseColor;
    return color == -1 ? cResult_Unset : color;
}

/* WWHD glue
str 10142F38 "CLR1"
str 10142F40 "CLB1"
str 10142F48 "ATI2"
str 10142F50 "ALB1"
str 10142F58 "ALI2"
str 10142F60 "TGG2"
str 10142F68 "TAG2"
str 10142F70 "TGP2"
str 10142F78 "TGL2"
str 10142F80 "SYL3"
str 10142F88 "SLB1"
str 10142F90 "CTI1"
str 10142F98 "MsgPrjBn"
*/
