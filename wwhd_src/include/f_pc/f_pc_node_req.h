/* Node creation requests. */
#pragma once
#include "f_pc/f_pc_node.h"
#include "f_pc/f_pc_create_req.h"
#include "SSystem/SComponent/c_phase.h"


struct node_create_request_method_class {
    gfn mpExecuteFunc;
    gfn mpCancelFunc;
    gfn mpUnkFunc;
    gfn mpPostMethodFunc;
};
struct unk_process_node_class {
    gptr<process_node_class> mpNodeProc;
    be<fpc_ProcID> mProcId;
};
struct node_create_request {
    /* 0x00 */ create_tag_class mCreateTag;
    /* 0x14 */ process_method_tag_class mProcMthCls;
    /* 0x30 */ request_of_phase_process_class mReqPhsProc;
    /* 0x38 */ gptr<cPhs__Handler> mpPhsHandler;
    /* 0x3C */ gptr<node_create_request_method_class> mpNodeCrReqMthCls;
    /* 0x40 */ be<s32> mParameter;
    /* 0x44 */ be<u32> mRequestId;
    /* 0x48 */ unk_process_node_class mNodeProc;
    /* 0x50 */ gptr<layer_class> mpLayerClass;
    /* 0x54 */ be<fpc_ProcID> mCreatingID;
    /* 0x58 */ be<s16> mProcName;
    u8 padding[2];
    /* 0x5C */ gptr<void> mpUserData;
    /* 0x60 */ be<s16> unk_0x60;
    u8 padding2[2];
};
WWHD_SIZE(node_create_request_method_class, 0x10);
WWHD_SIZE(unk_process_node_class, 8);
WWHD_OFFSET(node_create_request, mProcMthCls, 0x14);
WWHD_OFFSET(node_create_request, mReqPhsProc, 0x30);
WWHD_OFFSET(node_create_request, mpPhsHandler, 0x38);
WWHD_OFFSET(node_create_request, mNodeProc, 0x48);
WWHD_OFFSET(node_create_request, mCreatingID, 0x54);
WWHD_OFFSET(node_create_request, mpUserData, 0x5C);
WWHD_SIZE(node_create_request, 0x64);
struct request_node_class {
    node_class base;
    gptr<node_create_request> mNodeCrReq;
};
WWHD_SIZE(request_node_class, 0x10);
enum NodeRequestType : u32 {
    NodeRequest_Create = 0,
    NodeRequest_Delete = 1,
    NodeRequest_Change = 2,
    NodeRequest_Blocked = 4,
};
struct NodeRequestPhaseTable { cPhs__Handler methods[6]; };
struct NodeRequestExecuteMap { be<u8> result[3]; };
struct NodeRequestTemplate { be<u32> words[25]; };
BOOL fpcM_IsCreating(u32);
BOOL fpcEx_IsExist(u32);
BOOL fpcDt_IsComplete();
u32 fpcSCtRq_Request(layer_class*, s16, u32, node_create_request*, void*);
void* nodeRequestMemalign(s32, u32);
void nodeRequestFree(void*);
void sBs_ClearArea(void*, u32);
layer_class* fpcLy_CurrentLayer();
void nodeRequestAssert(const char*, u32, const char*);

s32 cTg_SingleCut(create_tag_class*);
s32 cTg_Addition(node_list_class*, create_tag_class*);
void cTg_Create(create_tag_class*, void*);
