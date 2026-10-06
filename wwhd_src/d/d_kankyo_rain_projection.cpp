// HD weather projection parameter helper.
#include "gabi.h"
using namespace gabi;
struct ProjectionParameters { u32 words[10]; };
void rainProjection(u32 output){
 WWHD_FUNC(0x02563DD8,void,output);
 Local<ProjectionParameters> parameters;
 for(u32 i=0;i<5;i++){
  store<u32>(parameters.a+i*8,load<u32>(0x1004F4F4+i*8));
  store<u32>(parameters.a+i*8+4,load<u32>(0x1004F4F8+i*8));
 }
 f32 upper=load<f32>(0x104B45C0+0x134);
 f32 lower=load<f32>(0x104B45C0+0x130);
 store<f32>(parameters.a+20,upper);
 store<f32>(parameters.a+16,lower);
 for(u32 i=0;i<5;i++){
  store<u32>(output+i*8,load<u32>(parameters.a+i*8));
  store<u32>(output+i*8+4,load<u32>(parameters.a+i*8+4));
 }
}
VERIFY(0x02563DD8,rainProjection);
