/** d_scope: HD compiler statics. */
#include "d/d_scope.h"

static dHeaderStatics_c l_scopeHeaderStatics;
static dHeaderStaticRecords_c l_scopeRecords;
static f32 l_scopeInitialFirst;
static f32 l_scopeInitialSecond;
void scope_registerStatic(void*);
void scope_initFlag(void*);
void scope_initByte(void*);

// HD: The telescope screen implementation is replaced by constant process methods.

/* 025BB024 */
s32 dScp_Create(void* i_this) { return cPhs_COMPLEATE_e; }

/* 025BB02C */
s32 dScp_Delete(void* i_this) { return TRUE; }

/* 025BB034 */
s32 dScp_Execute(void* i_this) { return TRUE; }

/* 025BB03C */
s32 dScp_Draw(void* i_this) { return TRUE; }

/* 025BB044 */
s32 dScp_IsDelete(void* i_this) { return TRUE; }

// HD: Compiler-generated initialization and destructor registration order.
/* 025BB04C */
void d_scope_staticInit() {
    l_scopeHeaderStatics.mZeroWords[2] = 0;
    l_scopeHeaderStatics.mZeroWords[0] = 0;
    l_scopeHeaderStatics.mZeroWords[3] = 0;
    l_scopeHeaderStatics.mZeroWords[1] = 0;
    scope_registerStatic(&l_scopeRecords.mFirst);
    f32 first = l_scopeInitialFirst, second = l_scopeInitialSecond;
    l_scopeHeaderStatics.mFloatPair[0] = first;
    l_scopeHeaderStatics.mFloatPair[1] = second;
    scope_initFlag(&l_scopeHeaderStatics.mFlag);
    scope_registerStatic(&l_scopeRecords.mSecond);
    scope_initByte(&l_scopeHeaderStatics.mByte);
    scope_registerStatic(&l_scopeRecords.mThird);
}

/* WWHD glue
ext 028F026C void scope_registerStatic(void*)
ext 028ED6F8 void scope_initFlag(void*)
ext 028EAB2C void scope_initByte(void*)
data l_scopeHeaderStatics 1047C948
data l_scopeRecords 101EAE34
data l_scopeInitialFirst 10054560
data l_scopeInitialSecond 10054564
*/
