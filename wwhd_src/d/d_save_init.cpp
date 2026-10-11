/** d_save_init: HD compiler statics. */
#include "d/d_save_init.h"

static dHeaderStatics_c l_saveHeaderStatics;
static dHeaderStaticRecords_c l_saveRecords;
static f32 l_saveInitialFirst;
static f32 l_saveInitialSecond;
void save_registerStatic(void*);
void save_initFlag(void*);
void save_initByte(void*);


// HD: Compiler-generated initialization and destructor registration order.
/* 025BAF90 */
void d_save_init_staticInit() {
    l_saveHeaderStatics.mZeroWords[2] = 0;
    l_saveHeaderStatics.mZeroWords[0] = 0;
    l_saveHeaderStatics.mZeroWords[3] = 0;
    l_saveHeaderStatics.mZeroWords[1] = 0;
    save_registerStatic(&l_saveRecords.mFirst);
    f32 first = l_saveInitialFirst, second = l_saveInitialSecond;
    l_saveHeaderStatics.mFloatPair[0] = first;
    l_saveHeaderStatics.mFloatPair[1] = second;
    save_initFlag(&l_saveHeaderStatics.mFlag);
    save_registerStatic(&l_saveRecords.mSecond);
    save_initByte(&l_saveHeaderStatics.mByte);
    save_registerStatic(&l_saveRecords.mThird);
}

/* WWHD glue
ext 028F026C void save_registerStatic(void*)
ext 028ED6F8 void save_initFlag(void*)
ext 028EAB2C void save_initByte(void*)
data l_saveHeaderStatics 1047C92C
data l_saveRecords 101EADFC
data l_saveInitialFirst 10054554
data l_saveInitialSecond 10054558
*/
