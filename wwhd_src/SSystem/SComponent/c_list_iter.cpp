/** List iteration. WWHD, verified unit c_list_iter. */
#include "SSystem/SComponent/c_list_iter.h"

/* 020100A0 */
s32 cLsIt_Method(node_list_class* pList, u32 pMethod, void* pUserData) {
    if (pList->mSize > 0) {
        return cNdIt_Method(pList->mpHead, pMethod, pUserData);
    }
    return TRUE;
}

/* 020100BC */
void* cLsIt_Judge(node_list_class* pList, u32 pJudge, void* pUserData) {
    if (pList->mSize > 0) {
        return cNdIt_Judge(pList->mpHead, pJudge, pUserData);
    }
    return NULL;
}

/* 020100D8 */
// HD: header statics and their destructor registrations.
static void __sinit_c_list_iter_cpp() {
    sinit_header_statics_rodata(0x101FF7EC, 0x1018C7D8, 0x101FF7F8, 0x10001EE0);
}

/* 0201016C */
// HD: unidentified adjacent translation unit, allocating constructor form.
hd_word3* hd_word3_ctor(hd_word3* pObject, u32 first, u32 second, u32 third) {
    if (pObject == NULL) {
        pObject = static_cast<hd_word3*>(hd_allocate(sizeof(hd_word3)));
        if (pObject == NULL) {
            return NULL;
        }
    }
    pObject->second = second;
    pObject->first = first;
    pObject->third = third;
    return pObject;
}

static hd_word3 defaultWords;

/* 020101D0 */
// HD: adjacent translation unit's header statics and three-word instance.
static void __sinit_adjacent_word3_cpp() {
    sinit_header_statics_rodata(0x101FF808, 0x1018C7FC, 0x101FF814, 0x10001EEC);
    hd_word3_ctor(&defaultWords, 0, 0, 0);
}

/* WWHD glue (tools/verify/autoglue.py): external node iteration and HD allocation
ext 02019D50 s32 cNdIt_Method(node_class*, u32, void*)
ext 02019DE8 void* cNdIt_Judge(node_class*, u32, void*)
ext 0273AD10 void* hd_allocate(u32)
data defaultWords 101FF824
*/
