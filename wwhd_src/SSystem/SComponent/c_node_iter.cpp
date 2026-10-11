/**
 * c_node_iter.cpp
 * System component - node iteration and search
 *
 * WWHD, written in the style of zeldaret/tww and verified function by function against the
 * recompiled original (tools/verify, unit c_node_iter).
 */

#include "SSystem/SComponent/c_node_iter.h"

/* 02019D50 */
BOOL cNdIt_Method(node_class* pNode, cNdIt_MethodFunc pMethod, void* pUserData) {
    BOOL ret = TRUE;
    node_class* pNext = pNode ? (node_class*)pNode->mpNextNode : NULL;

    while (pNode) {
        // HD: dispatch the guest callback through the verification ABI.
        int methodRet = gabi::call_ptr<int>(gabi::ea((void*)pMethod), pNode, pUserData);
        if (!methodRet) {
            ret = FALSE;
        }
        pNode = pNext;
        pNext = pNext ? (node_class*)pNext->mpNextNode : NULL;
    }

    return ret;
}

/* 02019DE8 */
void* cNdIt_Judge(node_class* pNode, cNdIt_JudgeFunc pJudge, void* pUserData) {
    node_class* pNext = pNode ? (node_class*)pNode->mpNextNode : NULL;

    while (pNode) {
        // HD: dispatch the guest callback through the verification ABI.
        void* pJudgeRet = gabi::call_ptr<void*>(gabi::ea((void*)pJudge), pNode, pUserData);
        if (pJudgeRet != NULL) {
            return pJudgeRet;
        }
        pNode = pNext;
        pNext = pNext ? (node_class*)pNext->mpNextNode : NULL;
    }

    return NULL;
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */

/* 02019E6C */
static void __sinit_c_node_iter_cpp() {
    sinit_header_statics_rodata(0x101FFA34, 0x1018D2B0, 0x101FFA40, 0x10003648);
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
*/
