#include "gabi.h"
using namespace gabi;
namespace f_op_scene_pause {
s32 enable(void *scene) {
 WWHD_FUNC(0x025DCA64,s32,scene);
 if(!scene)return 0;
 call<void>(0x025DFB20,scene,u32(1));
 call<void>(0x025DFB20,scene,u32(2));
 return 1;
}
VERIFY(0x025DCA64,enable);
s32 disable(void *scene) {
 WWHD_FUNC(0x025DCAC4,s32,scene);
 if(!scene)return 0;
 u32 layer=load<u32>(ea(scene)+0x2C);
 u32 parent=load<u32>(layer+0x18);
 if(!parent){
  call<void>(0x025DFB24,scene,u32(1));
  call<void>(0x025DFB24,scene,u32(2));
 }else if(call<s32>(0x025DE564,load<u32>(parent+4))==1){
  if(call<s32>(0x025DFB1C,at<void>(parent),u32(1))==0)call<void>(0x025DFB24,scene,u32(1));
  if(call<s32>(0x025DFB1C,at<void>(parent),u32(2))==0)call<void>(0x025DFB24,scene,u32(2));
 }
 return 1;
}
VERIFY(0x025DCAC4,disable);
void static_init() {
  WWHD_FUNC(0x025DCB94,void);
  store<u32>(0x1048A60C,0);store<u32>(0x1048A604,0);store<u32>(0x1048A610,0);store<u32>(0x1048A608,0);
  call<void>(0x028F026C,at<void>(0x101F37BC));
  f32 first=load<f32>(0x10057D90),second=load<f32>(0x10057D94);
  store<f32>(0x1048A5F8,first);store<f32>(0x1048A5FC,second);
  call<void>(0x028ED6F8,at<void>(0x1048A600));call<void>(0x028F026C,at<void>(0x101F37C8));
  call<void>(0x028EAB2C,at<void>(0x1048A601));call<void>(0x028F026C,at<void>(0x101F37D4));
}
VERIFY(0x025DCB94,static_init);
}
