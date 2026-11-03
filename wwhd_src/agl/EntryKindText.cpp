// WWHD render-entry kind labels, preserving their temporary text-object layout.
#include "agl/EntryConstruction.h"
static sead::SafeTextVtable sKindTextVtable;
static EntryLabel sKindZero;
static EntryLabel sKindOne;
static EntryLabel sKindTwo;
static EntryLabel sKindThree;
static EntryLabel sKindFour;
static EntryLabel sKindFive;
static EntryLabel sKindSix;
struct KindTextArray { sead::SafeText mTexts[7]; };
void releaseEffectName(void*, u32);
void destroyAglTextArray(sead::SafeText*, u32, u32, u32, u32, u32);
/* 027ABF8C */
static void constructEntryKindText(sead::SafeText* destination, u32 kind) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<KindTextArray> texts(0x8);
    EntryLabel* labels[] = {&sKindZero, &sKindOne, &sKindTwo, &sKindThree, &sKindFour, &sKindFive, &sKindSix};
    texts->mTexts[0].mVtable = &sKindTextVtable;
    texts->mTexts[0].mText = &sKindZero.mFirst;
    for (u32 index = 1; index < 7; ++index) {
        texts->mTexts[index].mVtable = &sKindTextVtable;
        texts->mTexts[index].mText = &labels[index]->mFirst;
    }
    sead::SafeText* selected = &texts->mTexts[kind];
    if (destination == NULL) destination = (sead::SafeText*)allocateAglStorage(sizeof(sead::SafeText));
    if (destination != NULL) {
        destination->mVtable = &sKindTextVtable;
        destination->mText = selected->mText;
    }
    destroyAglTextArray(texts->mTexts, 7, sizeof(sead::SafeText),
        gabi::guest_fn((const void*)releaseEffectName), 0, 0);
}
/* WWHD glue
 data sKindTextVtable 1016858C
 data sKindZero 10168A10
 data sKindOne 10168A18
 data sKindTwo 10168A20
 data sKindThree 10168A44
 data sKindFour 10168A34
 data sKindFive 10168A28
 data sKindSix 10168A58
 ext 028F0164 void destroyAglTextArray(sead::SafeText*, u32, u32, u32, u32, u32)
*/
