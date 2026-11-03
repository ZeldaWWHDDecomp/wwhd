/* Optional sound values are packed in flag order; widths and defaults follow WWHD. */
#include "nw/snd/SoundOptionalParameters.h"
using namespace nw::snd;
static EnvelopeBytes sSoundDefaultEnvelope;
static EnvelopeBytes sNoteDefaultEnvelope;
template<class T> static const T* optionalResourceAt(const void* base,u32 offset) { return gabi::at<T>(gabi::ea(base)+offset); }
static const be<u32>* optionalValue(const void* base,u32 flagOffset,u32 bit) {
    u32 flags=*optionalResourceAt<be<u32>>(base,flagOffset);
    u32 rank=0;
    bool present=false;
    for (u32 i=0;i<=bit;++i) {
        if (flags&(1u<<i)) { ++rank; if (i==bit) present=true; }
    }
    if (!present || !rank) return nullptr;
    return optionalResourceAt<be<u32>>(base,flagOffset+rank*sizeof(be<u32>));
}
/* 028B1000 */
u32 SoundOptionalParameters::byteValue() const {
    if ((static_cast<u32>(mFlags)&1)==0) return 64;
    return *optionalResourceAt<be<u8>>(this,7);
}
/* 028B1320 */
u32 TrackOptionalParameters::byteValue() const {
    if ((static_cast<u32>(mFlags)&1)==0) return 64;
    return *optionalResourceAt<be<u8>>(this,11);
}
/* 028B178C */
u32 NoteOptionalParameters::keyValue() const {
    if ((static_cast<u32>(mFlags)&1)==0) return 60;
    return *optionalResourceAt<be<u8>>(this,11);
}
/* 028B1020 */
s32 SoundOptionalParameters::signedByteValue() const {
    if ((static_cast<u32>(mFlags)&1)==0) return 0;
    u32 value=*optionalResourceAt<be<u32>>(this,4);
    return static_cast<s8>((value>>8)&255);
}
/* 028B11E8 */
u32 SoundOptionalParameters::alternateByteValue() const {
    const be<u32>* word=optionalValue(this,0,2);
    if (!word) return 64;
    return (static_cast<u32>(*word)>>0)&255;
}
/* 028B1250 */
u32 SoundOptionalParameters::highByteValue() const {
    const be<u32>* word=optionalValue(this,0,2);
    if (!word) return 0;
    return (static_cast<u32>(*word)>>8)&255;
}
/* 028B12B8 */
u32 SoundOptionalParameters::upperByteValue() const {
    const be<u32>* word=optionalValue(this,0,2);
    if (!word) return 0;
    return (static_cast<u32>(*word)>>16)&255;
}
/* 028B1340 */
u32 TrackOptionalParameters::alternateByteValue() const {
    const be<u32>* word=optionalValue(this,4,1);
    if (!word) return 96;
    return (static_cast<u32>(*word)>>0)&255;
}
/* 028B13AC */
u32 TrackOptionalParameters::trailingByteValue() const {
    const be<u32>* word=optionalValue(this,4,2);
    if (!word) return 64;
    return (static_cast<u32>(*word)>>0)&255;
}
/* 028B1418 */
u32 TrackOptionalParameters::highByteValue() const {
    const be<u32>* word=optionalValue(this,4,2);
    if (!word) return 0;
    return (static_cast<u32>(*word)>>8)&255;
}
/* 028B17AC */
u32 NoteOptionalParameters::byteValue() const {
    const be<u32>* word=optionalValue(this,4,1);
    if (!word) return 127;
    return (static_cast<u32>(*word)>>0)&255;
}
/* 028B1818 */
u32 NoteOptionalParameters::alternateByteValue() const {
    const be<u32>* word=optionalValue(this,4,2);
    if (!word) return 64;
    return (static_cast<u32>(*word)>>0)&255;
}
/* 028B1964 */
u32 NoteOptionalParameters::highByteValue() const {
    const be<u32>* word=optionalValue(this,4,4);
    if (!word) return 0;
    return (static_cast<u32>(*word)>>8)&255;
}
/* 028B19D0 */
u32 NoteOptionalParameters::upperByteValue() const {
    const be<u32>* word=optionalValue(this,4,4);
    if (!word) return 0;
    return (static_cast<u32>(*word)>>16)&255;
}
/* 028B1044 */
f32 SoundOptionalParameters::scalarValue() const {
    const be<u32>* word=optionalValue(this,0,1);
    if (!word) return 1.0f;
    return *reinterpret_cast<const be<f32>*>(word);
}
/* 028B1484 */
f32 TrackOptionalParameters::scalarValue() const {
    const be<u32>* word=optionalValue(this,4,3);
    if (!word) return 1.0f;
    return *reinterpret_cast<const be<f32>*>(word);
}
/* 028B1884 */
f32 NoteOptionalParameters::scalarValue() const {
    const be<u32>* word=optionalValue(this,4,3);
    if (!word) return 1.0f;
    return *reinterpret_cast<const be<f32>*>(word);
}
/* 028B18F0 */
u32 NoteOptionalParameters::booleanValue() const {
    const be<u32>* word=optionalValue(this,4,4);
    if (!word) return 0;
    return (static_cast<u32>(*word)&255)!=0;
}
/* 028B1174 */
const EnvelopeBytes* SoundOptionalParameters::envelope() const {
    const be<u32>* word=optionalValue(this,0,9);
    if (!word) return &sSoundDefaultEnvelope;
    const void* reference=optionalResourceAt<be<u8>>(this,*word);
    u32 offset=*optionalResourceAt<be<u32>>(reference,4);
    return optionalResourceAt<EnvelopeBytes>(reference,offset);
}
/* 028B14F0 */
const EnvelopeBytes* TrackOptionalParameters::envelope() const {
    const be<u32>* word=optionalValue(this,4,9);
    if (!word) return &sSoundDefaultEnvelope;
    const void* reference=optionalResourceAt<be<u8>>(this,*word);
    u32 offset=*optionalResourceAt<be<u32>>(reference,4);
    return optionalResourceAt<EnvelopeBytes>(reference,offset);
}
/* 028B1A3C */
const EnvelopeBytes* NoteOptionalParameters::envelope() const {
    const be<u32>* word=optionalValue(this,4,9);
    if (!word) return &sNoteDefaultEnvelope;
    const void* reference=optionalResourceAt<be<u8>>(this,*word);
    u32 offset=*optionalResourceAt<be<u32>>(reference,4);
    return optionalResourceAt<EnvelopeBytes>(reference,offset);
}
/* 028B10AC */
void SoundOptionalParameters::readRouting(be<u8>* control,be<u8>* outputs,s32 outputCapacity) const {
    const be<u32>* word=optionalValue(this,0,8);
    if (!word) {
        *control=127;
        for (s32 i=0;i<outputCapacity;++i) outputs[i]=0;
        return;
    }
    const be<u8>* routing=optionalResourceAt<be<u8>>(this,*word);
    *control=routing[0];
    u32 count=routing[1];
    if (count>3) count=3;
    for (u32 i=0;i<count;++i) outputs[i]=routing[i+2];
}
/* 028B1AB4 */
const EnvelopeBytes* NoteEnvelopeReference::envelope() const {
    if (static_cast<u32>(mKind)!=0x21F) return nullptr;
    return &mEnvelope;
}
/* 028B1568 */
void nw::snd::initializeSoundEnvelopeDefaults() {
    for (u32 i=0;i<5;++i) sSoundDefaultEnvelope.values[i]=127;
}
/* 028B1AD0 */
void nw::snd::initializeNoteEnvelopeDefaults() {
    for (u32 i=0;i<5;++i) sNoteDefaultEnvelope.values[i]=127;
}
/* WWHD glue
data sSoundDefaultEnvelope 104D9B00
data sNoteDefaultEnvelope 104D9B08
*/
