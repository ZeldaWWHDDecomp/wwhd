/* Local WWHD first-creation process request reconstruction. */
#include "wwhd.h"
s32 fst_phase(void* request) {
 WWHD_FUNC(0x025DE820,s32,request);
 u32 addr=gabi::ea(request),callback=gabi::load<u32>(addr+0x48);
 if(callback) {
  void* data=gabi::at<void>(gabi::load<u32>(addr+0x4C));
  void* process=gabi::at<void>(gabi::load<u32>(addr+0x40));
  if(gabi::call_ptr<s32>(callback,process,data)==0) return 3;
 }
 return 4;
}
VERIFY(0x025DE820,fst_phase);
s32 fst_cancel(void* request) {
 WWHD_FUNC(0x025DE87C,s32,request);
 return 1;
}
VERIFY(0x025DE87C,fst_cancel);
void* fst_request(void* layer,s32 profile,void* callback,void* data,void* append) {
 WWHD_FUNC(0x025DE884,void*,layer,profile,callback,data,append);
 void* req=gabi::call<void*>(0x025DDAD4,layer,80,gabi::at<void>(0x101F3AB8));
 if(!req) return nullptr;
 gabi::call(0x025DEAB4,layer);
 u32 id=gabi::call<u32>(0x025DD290);
 void* process=gabi::call<void*>(0x025DD414,profile,id,append);
 if(process) {
  u32 addr=gabi::ea(req),proc=gabi::ea(process);
  gabi::store<u8>(addr+0x38,1);
  gabi::store<u32>(proc+0x14,addr);
  gabi::store<u32>(addr+0x40,proc);
  u32 processID=gabi::load<u32>(proc+4);
  gabi::store<u32>(addr+0x3C,processID);
  if(gabi::call<s32>(0x025DD538,process)==2) {
   gabi::store<u32>(addr+0x48,gabi::ea(callback));
   gabi::store<u32>(addr+0x4C,gabi::ea(data));
   return process;
  }
 }
 gabi::call(0x025DD934,req);
 return nullptr;
}
VERIFY(0x025DE884,fst_request);
void fst_initializer() {
 WWHD_FUNC(0x025DE94C,void);
 for(u32 off:{8u,0u,12u,4u}) gabi::store<u32>(0x1048A7E0+off,0);
 gabi::call(0x028F026C,gabi::at<void>(0x101F3AC4));
 f32 a=gabi::load<f32>(0x100582E8),b=gabi::load<f32>(0x100582EC);
 gabi::store<f32>(0x1048A7D4,a); gabi::store<f32>(0x1048A7D8,b);
 gabi::call(0x028ED6F8,gabi::at<void>(0x1048A7DC));
 gabi::call(0x028F026C,gabi::at<void>(0x101F3AD0));
 gabi::call(0x028EAB2C,gabi::at<void>(0x1048A7DD));
 gabi::call(0x028F026C,gabi::at<void>(0x101F3ADC));
}
VERIFY(0x025DE94C,fst_initializer);
