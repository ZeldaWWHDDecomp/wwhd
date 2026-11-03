/* JAudio byte and word transfers; Cafe omits debug alignment assertions. */
#pragma once
#include "bindings.h"
namespace JASystem { namespace Calc {
void bcopyfast(const be<u32>* source, be<u32>* destination, u32 size);
void bcopy(const void* source, void* destination, u32 size);
void bzerofast(void* destination, u32 size);
void bzero(void* destination, u32 size);
}}
void clearAudioCacheRange(void* destination, u32 size);
struct CalcHeaderStatics { be<u32> mWords[4]; };
struct CalcDestructorRecord { be<u32> mWords[3]; };
void registerCalcStatics(CalcDestructorRecord*);
