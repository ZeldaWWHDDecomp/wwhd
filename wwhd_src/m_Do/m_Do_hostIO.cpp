#include "gabi.h"
// Adjacent empty HD entry points; exact source-level roles remain unspecified.
void mDoHIO_Empty025F0A08() { WWHD_FUNC(0x025F0A08,void); }
VERIFY(0x025F0A08,mDoHIO_Empty025F0A08);
void mDoHIO_Empty025F0A0C() { WWHD_FUNC(0x025F0A0C,void); }
VERIFY(0x025F0A0C,mDoHIO_Empty025F0A0C);
s8 mDoHIO_createChild(void* root,const char* name,void* child) {
 WWHD_FUNC(0x025F0A10,s8,root,name,child);
 return 0;
}
VERIFY(0x025F0A10,mDoHIO_createChild);
void mDoHIO_deleteChild(void* root,s8 index) {
 WWHD_FUNC(0x025F0A18,void,root,index);
}
VERIFY(0x025F0A18,mDoHIO_deleteChild);
