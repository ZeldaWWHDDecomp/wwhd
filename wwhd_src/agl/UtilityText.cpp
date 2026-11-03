/* Compiler-emitted text lifetime adapters; identities are local to this TU. */
#include "agl/UtilityText.h"
namespace agl { namespace utility_text {
/* 027C49A4 */
static void destroyText(Text* text, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (text && (flags & 1)) aglUtilityTextStorageDelete(text);
}
/* 027C49B8 */
static void assureTermination(Text*) {
    gabi::AutoFrame frame{gabi::current_function()};
}
/* 027C49BC */
static void assureBufferTermination(TextBuffer* text) {
    gabi::AutoFrame frame{gabi::current_function()};
    be<u8>* storage = text->mText;
    u32 capacity = text->mCapacity;
    storage[capacity - 1] = 0;
}
/* 027C49D4 */
static void destroyTextBuffer(TextBuffer* text, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (text && (flags & 1)) aglUtilityTextStorageDelete(text);
}
/* 027C49E8 */
static void destroyFixedTextA(FixedTextA* text, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (text && (flags & 1)) aglUtilityTextStorageDelete(text);
}
/* 027C49FC */
static void destroyFixedTextB(FixedTextB* text, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (text && (flags & 1)) aglUtilityTextStorageDelete(text);
}
/* 027C4A10 */
static void destroyFixedTextC(FixedTextC* text, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (text && (flags & 1)) aglUtilityTextStorageDelete(text);
}
/* 027C4A24 */
static void destroyFixedTextD(FixedTextD* text, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (text && (flags & 1)) aglUtilityTextStorageDelete(text);
}
/* 027C4B58 */
static void destroyFixedTextE(FixedTextE* text, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (text && (flags & 1)) aglUtilityTextStorageDelete(text);
}
/* 027C4B6C */
static void destroyFixedTextG(FixedTextG* text, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (text && (flags & 1)) aglUtilityTextStorageDelete(text);
}
/* 027C4B80 */
static void destroyFixedTextH(FixedTextH* text, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (text && (flags & 1)) aglUtilityTextStorageDelete(text);
}
/* 027C4B94 */
static void destroyFixedTextF(FixedTextF* text, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (text && (flags & 1)) aglUtilityTextStorageDelete(text);
}
} }
/* WWHD glue
ext 0273AF40 void aglUtilityTextStorageDelete(void*)
*/
