// HD pipeline resource-prefix accessors; these views assert only their consumed prefixes.
#include "JSystem/J3DGraph/PipelineResourceViews.h"
#include "bindings.h"
using namespace j3d::pipeline;
namespace j3d::pipeline {
/* 027F68F4 */
RelativeResourceHeader_l* PipelineResourceView::resource() const { return mResource; }
/* 027F68FC */
void* PipelineResourceView::relativeData() const {
    RelativeResourceHeader_l* header=mResource;
    u32 offset=header->mRelativeData;
    if(offset==0) return nullptr;
    return gabi::at<void>(gabi::ea(header)+offsetof(RelativeResourceHeader_l,mRelativeData)+offset);
}
/* 027F7870 */
void PipelineWordReference::setWord(u32 word) { mWord=word; }
/* 027F7878 */
u32 PipelineWordReference::word() const { return mWord; }
}
/* WWHD glue
*/
