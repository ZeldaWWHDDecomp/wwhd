#pragma once
#include "wwhd.h"
struct linklistStaticBlock { be<u32> mWords[4]; };
WWHD_SIZE(linklistStaticBlock,16);
void linklistConstructRegister(void*);


namespace JGadget {
struct TLinkListNode { gptr<TLinkListNode> mNext; gptr<TLinkListNode> mPrev; };
struct TNodeLinkList {
    TLinkListNode* Erase(TLinkListNode*);
    TLinkListNode* Insert(gptr<TLinkListNode>*,TLinkListNode*);
    be<u32> count;
    TLinkListNode sentinel;
};
}
WWHD_OFFSET(JGadget::TLinkListNode,mNext,0);
WWHD_OFFSET(JGadget::TLinkListNode,mPrev,4);
WWHD_SIZE(JGadget::TLinkListNode,8);
WWHD_OFFSET(JGadget::TNodeLinkList,count,0);
WWHD_OFFSET(JGadget::TNodeLinkList,sentinel,4);
WWHD_SIZE(JGadget::TNodeLinkList,12);
