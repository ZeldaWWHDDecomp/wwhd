/** linklist platform initialization; private decomp source. */
#include "bindings.h"
#include "JSystem/JGadget/linklist.h"
static linklistStaticBlock sStaticBlock;
static u32 sStaticRegister;

/* 02843480 */
static void __sinit_linklist_cpp() {
    sStaticBlock.mWords[2] = 0;
    sStaticBlock.mWords[0] = 0;
    sStaticBlock.mWords[3] = 0;
    sStaticBlock.mWords[1] = 0;
    linklistConstructRegister(&sStaticRegister);
}


/* 0284341C */
static void TNodeLinkList__dt(JGadget::TNodeLinkList* object,u32 flags) {
    if (object != NULL && (flags & 1)) operator_delete(object);
}
/* 02843430 */
JGadget::TLinkListNode* JGadget::TNodeLinkList::Erase(TLinkListNode* node) {
    TLinkListNode* next = node->mNext;
    TLinkListNode* previous = node->mPrev;
    next->mPrev = previous;
    previous->mNext = next;
    count = (u32)count - 1;
    return next;
}
/* 02843454 */
JGadget::TLinkListNode* JGadget::TNodeLinkList::Insert(gptr<TLinkListNode>* iterator,TLinkListNode* node) {
    TLinkListNode* next = *iterator;
    TLinkListNode* previous = next->mPrev;
    node->mNext = next;
    node->mPrev = previous;
    next->mPrev = node;
    previous->mNext = node;
    count = (u32)count + 1;
    return node;
}

/* WWHD glue

 data sStaticBlock 104B6554
 data sStaticRegister 101FAFF0
 ext 028F026C void linklistConstructRegister(void*)
*/
