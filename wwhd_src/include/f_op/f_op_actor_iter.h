/* Actor iteration filters, WWHD layout. GameCube layouts and names are unchanged.
 * The verifier retains raw guest callback/data registers at the public boundary.
 */
#pragma once
#include "bindings.h"

struct node_list_class {
    /* 0x00 */ be<u32> mpHead;
    /* 0x04 */ be<u32> mpTail;
    /* 0x08 */ be<s32> mSize;
};
WWHD_SIZE(node_list_class, 0xC);
struct method_filter {
    /* 0x00 */ be<u32> mpMethodFunc;
    /* 0x04 */ be<u32> mpUserData;
};
struct judge_filter {
    /* 0x00 */ be<u32> mpJudgeFunc;
    /* 0x04 */ be<u32> mpUserData;
};
WWHD_OFFSET(method_filter, mpUserData, 4);
WWHD_OFFSET(judge_filter, mpUserData, 4);
WWHD_SIZE(method_filter, 8);
WWHD_SIZE(judge_filter, 8);

// HD: guest entry points used as callbacks by the list iterator.
enum : u32 {
    cTgIt_MethodCall = 0x0201A9F4,
    cTgIt_JudgeFilter = 0x0201AA08,
};
s32 cLsIt_Method(node_list_class* list, u32 callback, method_filter* data);
u32 cLsIt_Judge(node_list_class* list, u32 callback, judge_filter* data);
