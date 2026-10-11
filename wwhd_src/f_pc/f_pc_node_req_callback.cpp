/* Supplemental verification view of the same HD Handler, not an additional TU entry.
 * Callback effects may change the embedded next link. GameCube-style source verified
 * by tools/verify, f_pc_node_req_callback. */
#include "f_pc/f_pc_node_req_callback.h"

static node_request_queue l_fpcNdRq_Queue = {0, 0, 0};

/* 025E062C */
static s32 fpcNdRq_Handler() {
    request_node_class* currentNode = l_fpcNdRq_Queue.mpHead;
    while (currentNode != NULL) {
        node_create_request* req = currentNode->mNodeCrReq;
        u32 result = gabi::call_ptr<u32>(req->mpNodeCrReqMthCls->mpExecuteFunc.get(), req);
        // HD: reload next after the callback, which can unlink this request.
        currentNode = currentNode->mpNext;
        switch (result) {
        case cPhs_STOP_e:
        case cPhs_ERROR_e:
            if (fpcNdRq_Cancel(req) == FALSE) {
                return 0;
            }
            break;
        case cPhs_COMPLEATE_e:
            if (fpcNdRq_Delete(req) == FALSE) {
                return 0;
            }
            break;
        }
    }
    return 1;
}

/* WWHD glue
data l_fpcNdRq_Queue 101F3DA4
ext 025E05C4 s32 fpcNdRq_Cancel(node_create_request* req)
ext 025E0538 s32 fpcNdRq_Delete(node_create_request* req)
*/
