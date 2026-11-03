// Conditional batching after the original primary/alternate chain traversal.
#include "JSystem/J3DGraph/J3DBatchAdapter.h"
using namespace j3d;
BatchNode_l* primaryBatchNext(BatchNode_l*);
BatchNode_l* alternateBatchNext(BatchNode_l*);
BatchResource_l* lookupBatchResource(void*,u32);
BatchResource_l* selectBatchResource(void*,u32);
void copyBatchResource(BatchResource_l*,const BatchResource_l*);
void applyBatch(BatchObject_l*,u32,u16,u32);
// borrowed: this unit loads the manager pointer but does not install it.
static BatchManagerRef_l sBatchResourceManager;
/* inlined into BatchAdapter::combine */
static bool sameBatchResource(const BatchResource_l* current,const BatchResource_l* incoming) {
    for(u32 i=0;i<6;++i)
        if(u32(current->mIdentity[i])!=u32(incoming->mIdentity[i])) return false;
    if(u32(current->mAlignment)!=u32(incoming->mAlignment)) return false;
    if(u32(current->mLayout)!=u32(incoming->mLayout)) return false;
    return u32(current->mUsage)==u32(incoming->mUsage);
}
/* 02837D40 */
void BatchAdapter::combine(void*,BatchGroup_l* group) {
    gabi::AutoFrame frame{gabi::current_function()};
    BatchChains_l* chains=group->mChains;
    u32 count=chains->mCount;
    if(count<2) return;
    BatchSelector_l* selector=group->mSelector;
    u32 alternate=gabi::call_ptr<u32>(selector->mMethods->mAlternate,selector);
    chains=group->mChains;
    BatchNode_l* (*next)(BatchNode_l*)=alternate==0?primaryBatchNext:alternateBatchNext;
    BatchNode_l* node=alternate==0?chains->mPrimary:chains->mAlternate;
    BatchObject_l* first=node->mObject;
    u32 active=0;
    node=next(node);
    if(node==NULL) return;
    do {
        BatchObject_l* object=node->mObject;
        if(object->mDisabled==0) ++active;
        node=next(node);
    } while(node!=NULL);
    if(active<2 || first->mIneligible!=0) return;
    u32 countDoubled=active<<1;
    if(countDoubled>750) return;
    if(group->mPolicy->mEnabled!=0) {
        BatchResource_l* incoming=lookupBatchResource(sBatchResourceManager.mManager,6);
        if(incoming!=NULL) {
            BatchResource_l* current=selectBatchResource(first->mResource,0);
            if(!sameBatchResource(current,incoming)) {
                copyBatchResource(current,incoming);
                applyBatch(first,1,u16(countDoubled),2);
                return;
            }
            u32 location=incoming->mLocation;
            u32 auxiliary=incoming->mAuxiliary;
            current->mLocation=location;
            current->mRegisterAuxiliary=auxiliary;
            current->mRegisterSlot=location;
            current->mAuxiliary=auxiliary;
        }
    }
    applyBatch(first,1,u16(countDoubled),2);
}
/* WWHD glue
 ext 02830804 j3d::BatchNode_l* primaryBatchNext(j3d::BatchNode_l*)
 ext 0283080C j3d::BatchNode_l* alternateBatchNext(j3d::BatchNode_l*)
 ext 027F81A4 j3d::BatchResource_l* lookupBatchResource(void*,u32)
 ext 027FE640 j3d::BatchResource_l* selectBatchResource(void*,u32)
 ext 027BDEB4 void copyBatchResource(j3d::BatchResource_l*,const j3d::BatchResource_l*)
 ext 02825284 void applyBatch(j3d::BatchObject_l*,u32,u16,u32)
 data sBatchResourceManager 101F9968
*/
