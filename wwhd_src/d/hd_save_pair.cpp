/** HD save helpers; local decomp */
#include "d/hd_save_pair.h"
static u32 sSavePairHeaderStatics;
static u32 sSavePairHeaderDescriptors;
static u32 sSavePairHeaderZero;
static u32 sSavePairPiConstants;

/* 0271FA58 */
void hdSavePair_c::init() {
    mPrimary = 0; mReserved[1] = 0; mSecondary = 0; mReserved[0] = 0;
}

/* 0271FAA8 */
void hdSavePair_c::clear() {
    mPrimary = 0; mReserved[1] = 0; mSecondary = 0; mReserved[0] = 0;
}

/* 0271FA70 */
hdSavePair_c* createSavePair(hdSavePair_c* record) {
    if (record == NULL) { record = (hdSavePair_c*)operator_new(sizeof(hdSavePair_c)); if (record == NULL) return NULL; }
    record->init(); return record;
}

/* 0271FAC0 */
void hdSavePair_c::copyFrom(hdSavePair_c* source) {
    mPrimary = source->mPrimary; mSecondary = source->mSecondary;
}

/* 0271FAD4 */
static void __sinit_hd_save_pair_cpp() {
    sinit_header_statics_rodata(gabi::ea(&sSavePairHeaderStatics), gabi::ea(&sSavePairHeaderDescriptors), gabi::ea(&sSavePairHeaderZero), gabi::ea(&sSavePairPiConstants));
}

/* WWHD glue
data sSavePairHeaderStatics 1049F714 nocheck
data sSavePairHeaderDescriptors 101F844C nocheck
data sSavePairHeaderZero 1049F720 nocheck
data sSavePairPiConstants 10140740 nocheck
*/
