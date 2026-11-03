// Text-table identities are the own constructor's installed 1016E5A4/5BC/5D4/67C data.
#include "JSystem/J3DGraph/PipelineFormattedText.h"
void releasePipelineStorage(void*);
using namespace j3d::pipeline;
namespace j3d::pipeline {
/* 027F74C0 */
void PipelineValueNameText_l::destroy(PipelineValueNameText_l* object,u32 flags) {
    gabi::AutoFrame frame{0x027F74C0u};
    if(object!=nullptr && (flags&1u)!=0) releasePipelineStorage(object);
}
/* 027F781C */
void PipelineBaseTextBuffer_l::destroy(PipelineBaseTextBuffer_l* object,u32 flags) {
    gabi::AutoFrame frame{0x027F781Cu};
    if(object!=nullptr && (flags&1u)!=0) releasePipelineStorage(object);
}
/* 027F7830 */
void PipelineValueNameText_l::assureTermination(PipelineValueNameText_l*) {
    // This virtual slot performs no reads, stores or calls.
}
/* 027F7834 */
void PipelineBaseText_l::destroy(PipelineBaseText_l* object,u32 flags) {
    gabi::AutoFrame frame{0x027F7834u};
    if(object!=nullptr && (flags&1u)!=0) releasePipelineStorage(object);
}
/* 027F785C */
void PipelineFormattedText_l::destroy(PipelineFormattedText_l* object,u32 flags) {
    gabi::AutoFrame frame{0x027F785Cu};
    if(object!=nullptr && (flags&1u)!=0) releasePipelineStorage(object);
}
/* 027F7848 */
void PipelineSecondaryTextBufferPrefix_l::destroy(PipelineSecondaryTextBufferPrefix_l* object,u32 flags) {
    gabi::AutoFrame frame{0x027F7848u};
    if(object!=nullptr && (flags&1u)!=0) releasePipelineStorage(object);
}
/* 027F9098 */
void PipelineSurfaceNameText_l::destroy(PipelineSurfaceNameText_l* object,u32 flags) {
    gabi::AutoFrame frame{0x027F9098u};
    if(object!=nullptr && (flags&1u)!=0) releasePipelineStorage(object);
}
/* 027F90AC */
void PipelineSurfaceNameText_l::assureTermination(PipelineSurfaceNameText_l*) {}
/* 027F9F08 */
void PipelineTaskNameText_l::assureTermination(PipelineTaskNameText_l*) {}
}

/* WWHD glue
*/
