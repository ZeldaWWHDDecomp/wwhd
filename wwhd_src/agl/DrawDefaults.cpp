// WWHD draw-vector static initializer, from the unit's own stores.
#include "wwhd.h"
static f32 sDrawZero;
static f32 sDrawOne;
static f32 sDrawNegative;
static f32 sDrawSpecial;
static Mtx44 sDrawVectors;
/* 027AAE98 */
static void initializeDrawVectors() {
    f32 zero = sDrawZero;
    f32 one = sDrawOne;
    f32 negative = sDrawNegative;
    f32 special = sDrawSpecial;
    sDrawVectors.m[0][0] = one;
    sDrawVectors.m[0][1] = zero;
    sDrawVectors.m[0][2] = zero;
    sDrawVectors.m[0][3] = one;
    sDrawVectors.m[1][0] = zero;
    sDrawVectors.m[1][1] = special;
    sDrawVectors.m[1][2] = zero;
    sDrawVectors.m[1][3] = one;
    sDrawVectors.m[2][0] = zero;
    sDrawVectors.m[2][1] = zero;
    sDrawVectors.m[2][2] = one;
    sDrawVectors.m[2][3] = one;
    sDrawVectors.m[3][0] = zero;
    sDrawVectors.m[3][1] = zero;
    sDrawVectors.m[3][2] = zero;
    sDrawVectors.m[3][3] = negative;
}
static f32 sPrimitiveOne;
static f32 sPrimitiveZero;
static f32 sPrimitiveFirst;
static f32 sPrimitiveSecond;
static f32 sPrimitiveThird;
static f32 sPrimitiveFourth;
static f32 sPrimitiveFifth;
struct DrawVector { be<f32> mValues[4]; };
static DrawVector sPrimitiveFirstVector;
static DrawVector sPrimitiveSecondVector;
static DrawVector sPrimitiveThirdVector;
/* 027A9CAC */
static void initializePrimitiveVectors() {
    f32 one = sPrimitiveOne;
    f32 second = sPrimitiveSecond;
    f32 third = sPrimitiveThird;
    f32 first = sPrimitiveFirst;
    f32 zero = sPrimitiveZero;
    sPrimitiveSecondVector.mValues[0] = first;
    sPrimitiveSecondVector.mValues[1] = second;
    sPrimitiveSecondVector.mValues[2] = third;
    sPrimitiveSecondVector.mValues[3] = zero;
    f32 fourth = sPrimitiveFourth;
    f32 fifth = sPrimitiveFifth;
    sPrimitiveThirdVector.mValues[0] = fourth;
    sPrimitiveThirdVector.mValues[1] = fifth;
    sPrimitiveThirdVector.mValues[2] = one;
    sPrimitiveFirstVector.mValues[0] = one;
    sPrimitiveFirstVector.mValues[1] = zero;
    sPrimitiveFirstVector.mValues[2] = zero;
    sPrimitiveFirstVector.mValues[3] = one;
    sPrimitiveThirdVector.mValues[3] = zero;
}
/* WWHD glue
 data sPrimitiveOne 101684CC
 data sPrimitiveZero 101684C8
 data sPrimitiveFirst 101684E8
 data sPrimitiveSecond 101684EC
 data sPrimitiveThird 101684F0
 data sPrimitiveFourth 101684F4
 data sPrimitiveFifth 101684F8
 data sPrimitiveFirstVector 104A2010
 data sPrimitiveSecondVector 104A2020
 data sPrimitiveThirdVector 104A2030
 data sDrawZero 1016854C
 data sDrawOne 10168500
 data sDrawNegative 10168508
 data sDrawSpecial 10168558
 data sDrawVectors 104A2040
*/
