/* Node iteration, WWHD layout (unchanged from GameCube). */
#pragma once
#include "bindings.h"

struct node_class {
    /* 0x00 */ gptr<node_class> mpPrevNode;
    /* 0x04 */ gptr<void> mpData;
    /* 0x08 */ gptr<node_class> mpNextNode;
};
WWHD_OFFSET(node_class, mpPrevNode, 0x00);
WWHD_OFFSET(node_class, mpData, 0x04);
WWHD_OFFSET(node_class, mpNextNode, 0x08);
WWHD_SIZE(node_class, 0x0C);

typedef int (*cNdIt_MethodFunc)(node_class* pNode, void* pUserData);
typedef void* (*cNdIt_JudgeFunc)(node_class* pNode, void* pUserData);
BOOL cNdIt_Method(node_class* pNode, cNdIt_MethodFunc pMethod, void* pUserData);
void* cNdIt_Judge(node_class* pNode, cNdIt_JudgeFunc pJudge, void* pUserData);
