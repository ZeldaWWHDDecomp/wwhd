// Eleven explicit original switch arms select the contiguous sixteen-byte color records.
#include "JSystem/J3DGraph/PipelineColorTable.h"
using namespace j3d::pipeline;
namespace j3d::pipeline {
/* 027F9F0C */
sead::ColorChannels* PipelineColorTable_l::color(PipelineColorTable_l* table,u32 index) {
    if(index>=11) return nullptr;
    return &table->mColors[index];
}
}
/* WWHD glue
*/
