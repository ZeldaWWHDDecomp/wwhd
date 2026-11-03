// Small parameter and text methods, reconstructed from this unit's WWHD instructions.
#include "agl/ParameterHooks.h"
/* 027A8C80 */
static void terminateNameBuffer(sead::SafeTextBuffer* buffer) {
    buffer->mText[u32(buffer->mCapacity) - 1] = 0;
}
/* 027AE4B4 */
static void terminateAlternateNameBuffer(sead::SafeTextBuffer* buffer) {
    buffer->mText[u32(buffer->mCapacity) - 1] = 0;
}
/* 027AE520 */
u32 agl::ByteParameter::kindZero() { return 0; }
/* 027AE528 */
void* agl::ByteParameter::data() { return &mValue; }
/* 027AE530 */
void* agl::ByteParameter::dataConst() { return &mValue; }
/* 027AE538 */
u32 agl::ByteParameter::byteSize() { return 1; }
/* 027AE540 */
u32 agl::WordParameter::kindWord() { return 2; }
/* 027AE548 */
void* agl::WordParameter::data() { return &mValue; }
/* 027AE550 */
void* agl::WordParameter::dataConst() { return &mValue; }
/* 027AE558 */
u32 agl::WordParameter::byteSize() { return 4; }
/* 027AE560 */
u32 agl::FloatParameter::kindFloat() { return 1; }
/* 027AE568 */
void* agl::FloatParameter::data() { return &mValue; }
/* 027AE570 */
void* agl::FloatParameter::dataConst() { return &mValue; }
/* 027AE578 */
u32 agl::FloatParameter::byteSize() { return 4; }
/* 027AE580 */
u32 agl::PairParameter::kindPair() { return 3; }
/* 027AE588 */
void* agl::PairParameter::data() { return &mValue; }
/* 027AE590 */
void* agl::PairParameter::dataConst() { return &mValue; }
/* 027AE598 */
u32 agl::PairParameter::byteSize() { return 8; }
/* WWHD glue
*/
