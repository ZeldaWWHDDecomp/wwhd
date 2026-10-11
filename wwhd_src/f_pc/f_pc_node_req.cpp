/* Process-node requests. */
#include "f_pc/f_pc_node_req.h"

static node_list_class l_fpcNdRq_Queue;
static u32 request_id;
static NodeRequestTemplate clear;
static NodeRequestPhaseTable createMethods;
static NodeRequestPhaseTable deleteMethods;
static NodeRequestPhaseTable changeMethods;
static NodeRequestExecuteMap executeResults;

/* 025E0330 */
cPhs_State fpcNdRq_phase_IsCreated(node_create_request* i_NdCtReq) {
    if (fpcM_IsCreating(i_NdCtReq->mCreatingID) == TRUE) {
        return cPhs_INIT_e;
    }
    if (fpcEx_IsExist(i_NdCtReq->mCreatingID) == TRUE) return cPhs_NEXT_e;
    return cPhs_STOP_e;
}

/* 025E039C */
cPhs_State fpcNdRq_phase_Create(node_create_request* i_NdCtReq) {
    node_create_request_method_class* methods = i_NdCtReq->mpNodeCrReqMthCls;
    u32 id = fpcSCtRq_Request(i_NdCtReq->mpLayerClass, i_NdCtReq->mProcName,
                             methods->mpPostMethodFunc.get(), i_NdCtReq, i_NdCtReq->mpUserData);
    i_NdCtReq->mCreatingID = id;
    if (id == fpcM_ERROR_PROCESS_ID_e) return cPhs_STOP_e;
    return cPhs_NEXT_e;
}

/* 025E03F8 */
cPhs_State fpcNdRq_phase_IsDeleted(node_create_request* i_NdCtReq) {
    return fpcDt_IsComplete() == FALSE ? cPhs_INIT_e : cPhs_NEXT_e;
}

/* 025E0424 */
cPhs_State fpcNdRq_phase_Delete(node_create_request* i_NdCtReq) {
    process_node_class* node = i_NdCtReq->mNodeProc.mpNodeProc;
    if (node != NULL) {
        if (fpcDt_Delete(&node->base) == FALSE) {
            return cPhs_INIT_e;
        }
        i_NdCtReq->mNodeProc.mpNodeProc = NULL;
    }
    return cPhs_NEXT_e;
}

/* 025E0488 */
cPhs_State fpcNdRq_DoPhase(node_create_request* i_NdCtReq) {
    cPhs_State result;
    // HD: iterate consecutive next phases rather than recursively calling DoPhase.
    do {
        result = cPhs_Handler(&i_NdCtReq->mReqPhsProc, i_NdCtReq->mpPhsHandler, i_NdCtReq);
    } while (result == cPhs_NEXT_e);
    return result;
}

/* 025E04D0 */
u32 fpcNdRq_Execute(node_create_request* i_NdCtReq) {
    u32 result = fpcNdRq_DoPhase(i_NdCtReq);
    if (result <= cPhs_LOADING_e) {
        return cPhs_INIT_e;
    }
    if (result >= cPhs_STOP_e && result <= cPhs_ERROR_e) {
        // HD: the compiler's three-byte phase-result table is retained.
        return executeResults.result[result - cPhs_STOP_e];
    }
    return result;
}

/* 025E0538 */
BOOL fpcNdRq_Delete(node_create_request* i_NdCtReq) {
    // HD: RequestQTo is inlined, preserving its three external operations.
    fpcLy_CreatedMesg(i_NdCtReq->mpLayerClass);
    fpcLy_CancelQTo(&i_NdCtReq->mProcMthCls);
    cTg_SingleCut(&i_NdCtReq->mCreateTag);
    node_create_request_method_class* methods = i_NdCtReq->mpNodeCrReqMthCls;
    if (methods != NULL) {
        u32 method = methods->mpUnkFunc.get();
        if (method != 0 && fpcMtd_Method(method, i_NdCtReq) == FALSE) {
            return FALSE;
        }
    }
    nodeRequestFree(i_NdCtReq);
    return TRUE;
}

/* 025E05C4 */
BOOL fpcNdRq_Cancel(node_create_request* i_NdCtReq) {
    node_create_request_method_class* methods = i_NdCtReq->mpNodeCrReqMthCls;
    if (methods != NULL && fpcMtd_Method(methods->mpCancelFunc.get(), i_NdCtReq) == FALSE) {
        return FALSE;
    }
    return fpcNdRq_Delete(i_NdCtReq);
}

/* 025E062C */
s32 fpcNdRq_Handler() {
    node_class* currentNode = l_fpcNdRq_Queue.mpHead;
    while (currentNode != NULL) {
        node_create_request* req = reinterpret_cast<request_node_class*>(currentNode)->mNodeCrReq;
        node_create_request_method_class* methods = req->mpNodeCrReqMthCls;
        u32 result = gabi::call_ptr<u32>(methods->mpExecuteFunc.get(), req);
        // HD: fetch the next node after execute, before cancel/delete callbacks.
        currentNode = currentNode->mpNextNode;
        switch (result) {
        case cPhs_STOP_e:
        case cPhs_ERROR_e:
            if (fpcNdRq_Cancel(req) == FALSE) return FALSE;
            break;
        case cPhs_COMPLEATE_e:
            if (fpcNdRq_Delete(req) == FALSE) return FALSE;
            break;
        }
    }
    return TRUE;
}

/* 025E06EC */
s32 fpcNdRq_IsPossibleTarget(process_node_class* i_procNode) {
    u32 bsPcId = i_procNode->base.mBsPcId;
    node_class* currentNode = l_fpcNdRq_Queue.mpHead;
    while (currentNode != NULL) {
        node_create_request* currentNdCr = reinterpret_cast<request_node_class*>(currentNode)->mNodeCrReq;
        s32 type = currentNdCr->mParameter;
        if ((type == NodeRequest_Change || type == NodeRequest_Blocked || type == NodeRequest_Delete)
            && currentNdCr->mNodeProc.mProcId == bsPcId) {
            return FALSE;
        }
        currentNode = currentNode->mpNextNode;
    }
    return TRUE;
}

/* 025E0750 */
s32 fpcNdRq_IsIng(process_node_class* i_procNode) {
    u32 bsPcId = i_procNode->base.mBsPcId;
    node_class* currentNode = l_fpcNdRq_Queue.mpHead;
    while (currentNode != NULL) {
        node_create_request* currentNodeReq = reinterpret_cast<request_node_class*>(currentNode)->mNodeCrReq;
        if (currentNodeReq->mCreatingID == bsPcId) return TRUE;
        currentNode = currentNode->mpNextNode;
    }
    return FALSE;
}

/* 025E0798 */
node_create_request* fpcNdRq_Create(u32 i_requestSize) {
    node_create_request* req = static_cast<node_create_request*>(nodeRequestMemalign(-4, i_requestSize));
    if (req != NULL) {
        sBs_ClearArea(req, i_requestSize);
        // HD: the template is copied as 25 consecutive guest words, including padding.
        be<u32>* words = reinterpret_cast<be<u32>*>(req);
        for (u32 i = 0; i < 25; ++i) words[i] = clear.words[i];
        cTg_Create(&req->mCreateTag, req);
        fpcMtdTg_Init(&req->mProcMthCls, gabi::guest_fn((const void*)fpcNdRq_Cancel), req);
        u32 id = request_id;
        request_id = id + 1;
        req->mRequestId = id;
    }
    return req;
}

/* 025E0840 */
node_create_request* fpcNdRq_Request(u32 i_requestSize, u32 i_reqType,
    process_node_class* i_procNode, s16 i_procName, void* i_data,
    node_create_request_method_class* i_nodeCtRqMtd) {
    node_create_request* req = NULL;
    // HD: the three request constructors are inlined, with their original store order.
    if (i_reqType == NodeRequest_Create) {
        layer_class* layer = fpcLy_CurrentLayer();
        if (layer->mLayerID != 0 && fpcNdRq_IsPossibleTarget(layer->mpPcNode) == FALSE) return NULL;
        req = fpcNdRq_Create(i_requestSize);
        if (req != NULL) {
            req->mpPhsHandler = createMethods.methods;
            if (layer->mLayerID != 0) {
                req->mNodeProc.mpNodeProc = layer->mpPcNode;
                req->mNodeProc.mProcId = layer->mpPcNode->base.mBsPcId;
            }
            req->mProcName = i_procName;
            req->mpLayerClass = layer;
            req->mpUserData = i_data;
        }
    } else if (i_reqType == NodeRequest_Delete || i_reqType == NodeRequest_Change) {
        if (fpcNdRq_IsPossibleTarget(i_procNode) != TRUE) return NULL;
        if (fpcNdRq_IsIng(i_procNode) != FALSE) return NULL;
        req = fpcNdRq_Create(i_requestSize);
        if (req != NULL) {
            req->mNodeProc.mpNodeProc = i_procNode;
            req->mpPhsHandler = i_reqType == NodeRequest_Delete ? deleteMethods.methods : changeMethods.methods;
            req->mNodeProc.mProcId = i_procNode->base.mBsPcId;
            layer_class* layer = i_procNode->base.mLyTg.mpLayer;
            if (i_reqType == NodeRequest_Change) req->mProcName = i_procName;
            req->mpLayerClass = layer;
            if (i_reqType == NodeRequest_Change) req->mpUserData = i_data;
        }
    } else {
        // HD: invalid types assert, unlike the unfinished GameCube switch case.
        nodeRequestAssert("f_pc_node_req.cpp", 825, "invalid node request");
        return NULL;
    }
    if (req != NULL) {
        req->mParameter = i_reqType;
        req->mpNodeCrReqMthCls = i_nodeCtRqMtd;
        cTg_Addition(&l_fpcNdRq_Queue, &req->mCreateTag);
        fpcLy_ToCancelQ(req->mpLayerClass, &req->mProcMthCls);
        fpcLy_CreatingMesg(req->mpLayerClass);
    }
    return req;
}

/* 025E0A20 */
s32 fpcNdRq_ReChangeNode(u32 i_requestID, s16 i_procName, void* i_data) {
    node_class* currentNode = l_fpcNdRq_Queue.mpHead;
    while (currentNode != NULL) {
        node_create_request* found = reinterpret_cast<request_node_class*>(currentNode)->mNodeCrReq;
        if (found->mParameter == NodeRequest_Change && found->mRequestId == i_requestID) {
            if (found->mCreatingID != u32(-2)) return FALSE;
            found->mProcName = i_procName;
            found->mpUserData = i_data;
            return TRUE;
        }
        currentNode = currentNode->mpNextNode;
    }
    return FALSE;
}

/* 025E0A84 */
static void __sinit_f_pc_node_req_cpp() {
    // HD: initialize header statics emitted into this translation unit.
    sinit_header_statics(0x1048AAEC, 0x101F3E24);
}

/* 025E0B18 */
cPhs_State fpcNdRq_phase_IsDeleteTiming(node_create_request* i_NdCtReq) {
    return cPhs_NEXT_e;
}

/* WWHD glue
data l_fpcNdRq_Queue 101F3DA4
data request_id 101F3DBC
data clear 101F3DC0
data createMethods 101F3DB0
data deleteMethods 101F3D94
data changeMethods 101F3D7C
data executeResults 10058404
str 1005840C "f_pc_node_req.cpp" renamed
str 10058407 "invalid node request" renamed
ext 025DD868 BOOL fpcM_IsCreating(u32)
ext 025DE564 BOOL fpcEx_IsExist(u32)
ext 025E14A8 u32 fpcSCtRq_Request(layer_class*, s16, u32, node_create_request*, void*)
ext 025DDFA4 BOOL fpcDt_IsComplete()
ext 025DE1B8 BOOL fpcDt_Delete(base_process_class*)
ext 0201A0AC cPhs_State cPhs_Handler(request_of_phase_process_class*, cPhs__Handler*, void*)
ext 025DEA90 void fpcLy_CreatedMesg(layer_class*)
ext 025DE9E0 void fpcLy_CancelQTo(process_method_tag_class*)
ext 0201A868 s32 cTg_SingleCut(create_tag_class*)
ext 025DFCAC BOOL fpcMtd_Method(u32, void*)
ext 0201945C void nodeRequestFree(void*)
ext 02019430 void* nodeRequestMemalign(s32, u32)
ext 0201A918 void cTg_Create(create_tag_class*, void*)
ext 025DFE2C void fpcMtdTg_Init(process_method_tag_class*, u32, void*)
ext 0273AA24 void nodeRequestAssert(const char*, u32, const char*)
ext 0201A8B4 s32 cTg_Addition(node_list_class*, create_tag_class*)
ext 025DE9E4 void fpcLy_ToCancelQ(layer_class*, process_method_tag_class*)
ext 025DEA80 void fpcLy_CreatingMesg(layer_class*)
*/
