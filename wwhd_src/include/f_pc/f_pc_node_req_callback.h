/* Supplemental node-request callback view, GameCube field names.
 * Accessed request/tag layout is unchanged in HD. */
#pragma once
#include "bindings.h"
#include "gc_api/SSystem/c_phase.h"
struct node_create_request;
struct request_node_class {
    /* 0x00 */ gptr<request_node_class> mpPrev;
    /* 0x04 */ gptr<void> mpList;
    /* 0x08 */ gptr<request_node_class> mpNext;
    /* 0x0C */ gptr<node_create_request> mNodeCrReq;
};
struct node_create_request_method_class {
    gfn mpExecuteFunc;
    gfn mpCancelFunc;
    gfn mpUnkFunc;
    gfn mpPostMethodFunc;
};
struct node_create_request {
    /* 0x00 */ u8 mCreateTag[0x14];
    /* 0x14 */ u8 mProcMthCls[0x1C];
    /* 0x30 */ u8 mReqPhsProc[8];
    /* 0x38 */ gptr<void> mpPhsHandler;
    /* 0x3C */ gptr<node_create_request_method_class> mpNodeCrReqMthCls;
};
struct node_request_queue {
    gptr<request_node_class> mpHead;
    gptr<request_node_class> mpTail;
    be<s32> mSize;
};
WWHD_SIZE(request_node_class, 0x10);
WWHD_OFFSET(request_node_class, mpNext, 8);
WWHD_OFFSET(request_node_class, mNodeCrReq, 0xC);
WWHD_SIZE(node_create_request_method_class, 0x10);
WWHD_SIZE(node_create_request, 0x40);
WWHD_OFFSET(node_create_request, mpNodeCrReqMthCls, 0x3C);
WWHD_SIZE(node_request_queue, 0xC);
s32 fpcNdRq_Cancel(node_create_request* req);
s32 fpcNdRq_Delete(node_create_request* req);
