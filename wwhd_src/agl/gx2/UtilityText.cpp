/* Per-translation-unit text virtual bodies. */
#include "agl/gx2/UtilityText.h"
using namespace agl::utility_text;
static sead::SafeTextVtable sOwnedTextDispatchA;
static sead::SafeTextVtable sOwnedTextDispatchB;
namespace agl::utility_text {
/* 027E9F98 */
void TextA::destroy(TextA* text,u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (text && (flags & 1)) utilityTextDeleteObject(text);
}
/* 027E9FB0 */
void TextB::destroy(TextB* text,u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (text && (flags & 1)) utilityTextDeleteObject(text);
}
/* 027E9FC8 */
void FixedTextA::destroy(FixedTextA* text,u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (text && (flags & 1)) utilityTextDeleteObject(text);
}
/* 027EA058 */
void FixedTextB::destroy(FixedTextB* text,u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (text && (flags & 1)) utilityTextDeleteObject(text);
}
/* 027EA06C */
void FixedTextC::destroy(FixedTextC* text,u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (text && (flags & 1)) utilityTextDeleteObject(text);
}
/* 027EA080 */
void FixedTextD::destroy(FixedTextD* text,u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (text && (flags & 1)) utilityTextDeleteObject(text);
}
/* 027EA094 */
void FixedTextE::destroy(FixedTextE* text,u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (text && (flags & 1)) utilityTextDeleteObject(text);
}
/* 027EA0A8 */
void FixedTextH::destroy(FixedTextH* text,u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (text && (flags & 1)) utilityTextDeleteObject(text);
}
/* 027EA0BC */
void FixedTextF::destroy(FixedTextF* text,u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (text && (flags & 1)) utilityTextDeleteObject(text);
}
/* 027EA0D0 */
void FixedTextG::destroy(FixedTextG* text,u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (text && (flags & 1)) utilityTextDeleteObject(text);
}
/* 027EA0E4 */
void FixedTextI::destroy(FixedTextI* text,u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (text && (flags & 1)) utilityTextDeleteObject(text);
}
/* 027EA0F8 */
void TextC::destroy(TextC* text,u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (text && (flags & 1)) utilityTextDeleteObject(text);
}
/* 027EA110 */
void FixedTextJ::destroy(FixedTextJ* text,u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (text && (flags & 1)) utilityTextDeleteObject(text);
}
/* 027EA1A0 */
void FixedTextM::destroy(FixedTextM* text,u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (text && (flags & 1)) utilityTextDeleteObject(text);
}
/* 027EA1B4 */
void FixedTextK::destroy(FixedTextK* text,u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (text && (flags & 1)) utilityTextDeleteObject(text);
}
/* 027EA1C8 */
void FixedTextL::destroy(FixedTextL* text,u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (text && (flags & 1)) utilityTextDeleteObject(text);
}
/* 027EA1DC */
void FixedTextN::destroy(FixedTextN* text,u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (text && (flags & 1)) utilityTextDeleteObject(text);
}
/* 027E9FAC */
void TextA::assureTerminationImpl(TextA*) {}
/* 027E9FC4 */
void TextB::assureTerminationImpl(TextB*) {}
/* 027EA10C */
void TextC::assureTerminationImpl(TextC*) {}
/* 027E9FF4 */
void OwnedTextA::destroy(OwnedTextA* text,u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!text) return;
    be<u8>* storage = text->mText;
    text->mVtable = &sOwnedTextDispatchA;
    if (storage) utilityTextDeleteArray(storage);
    if (flags & 1) utilityTextDeleteObject(text);
}
/* 027EA13C */
void OwnedTextB::destroy(OwnedTextB* text,u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!text) return;
    be<u8>* storage = text->mText;
    text->mVtable = &sOwnedTextDispatchB;
    if (storage) utilityTextDeleteArray(storage);
    if (flags & 1) utilityTextDeleteObject(text);
}
/* 027E9FDC */
void FixedTextA::assureTerminationImpl(FixedTextA* text) {
    be<u8>* storage = text->mText;
    u32 capacity = text->mCapacity;
    be<u8>* last = gabi::at<be<u8>>(gabi::ea(storage) + capacity - 1);
    *last = 0;
}
/* 027EA124 */
void FixedTextJ::assureTerminationImpl(FixedTextJ* text) {
    be<u8>* storage = text->mText;
    u32 capacity = text->mCapacity;
    be<u8>* last = gabi::at<be<u8>>(gabi::ea(storage) + capacity - 1);
    *last = 0;
}
}
/* WWHD glue
data sOwnedTextDispatchA 1016D8F8
data sOwnedTextDispatchB 1016DA0C
ext 0273AF40 void utilityTextDeleteObject(void*)
ext 0273AFC8 void utilityTextDeleteArray(void*)
*/
