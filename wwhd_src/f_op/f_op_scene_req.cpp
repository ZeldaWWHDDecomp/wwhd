#include "wwhd.h"
#include "gabi.h"
s32 phase_wait_done(){WWHD_FUNC(0x025DCC28,s32);return gabi::call<s32>(0x025DBE64)==1?2:0;}
VERIFY(0x025DCC28,phase_wait_done);
u32 phase_delete(void* p){WWHD_FUNC(0x025DCC64,u32,p);return gabi::call<u32>(0x025E04D0,p);}
VERIFY(0x025DCC64,phase_delete);
s32 phase_wait_request(){WWHD_FUNC(0x025DCC68,s32);return gabi::call<s32>(0x025DBE38)==1?2:0;}
VERIFY(0x025DCC68,phase_wait_request);
s32 phase_wait_clear(){WWHD_FUNC(0x025DCCA4,s32);return gabi::call<s32>(0x025DBE1C)==1?2:0;}
VERIFY(0x025DCCA4,phase_wait_clear);
s32 phase_done(void* p){
 WWHD_FUNC(0x025DCCE0,s32,p);
 if(gabi::load<u32>(gabi::ea(p)+0x40)!=1){u32 id=gabi::load<u32>(gabi::ea(p)+0x54);u32 scene=gabi::call<u32>(0x025DE50C,id);gabi::call<s32>(0x025DCAC4,scene);}
 gabi::store<u32>(0x101F37E0,0);return 2;
}
VERIFY(0x025DCCE0,phase_done);
s32 fopScnRq_Execute(void* p){
 WWHD_FUNC(0x025DCD24,s32,p);
 s32 r;do {r=gabi::call<s32>(0x02019F74,gabi::at<void>(gabi::ea(p)+0x68),p);}while(r==2);return r;
}
VERIFY(0x025DCD24,fopScnRq_Execute);
s32 fopScnRq_PostMethod(void* scene,void* request){
 WWHD_FUNC(0x025DCD68,s32,scene,request);
 gabi::call<s32>(0x025DCA64,scene);
 if(gabi::load<u32>(gabi::ea(request)+0x64)!=0)gabi::call<void>(0x025DBDE4,gabi::load<u32>(gabi::ea(scene)+4));return 1;
}
VERIFY(0x025DCD68,fopScnRq_PostMethod);
s32 fopScnRq_Cancel(void* p){
 WWHD_FUNC(0x025DCDB8,s32,p);
 if(gabi::load<u32>(gabi::ea(p)+0x64)!=0 && gabi::call<s32>(0x025DBF3C,p)==0)return 0;return 1;
}
VERIFY(0x025DCDB8,fopScnRq_Cancel);
u32 fopScnRq_Request(u32 a,u32 b,u32 c,u32 d,u32 e,u32 f,u32 g){
 WWHD_FUNC(0x025DCE04,u32,a,b,c,d,e,f,g);
 u32 request=gabi::call<u32>(0x025E0840,0x74,a,b,c,d,gabi::at<void>(0x101F37E4));
 if(!request)return 0xFFFFFFFF;
 if(e!=0x7FFF){
  u32 overlap=0;
  if(gabi::load<u32>(0x101F37E0)==0)overlap=gabi::call<u32>(0x025DBE80,e,f,g);
  if(!overlap){gabi::call<void>(0x025E0538,request);return 0xFFFFFFFF;}
  gabi::store<u32>(0x101F37E0,1);gabi::store<u32>(request+0x64,overlap);
  gabi::call<void>(0x02019F0C,gabi::at<void>(request+0x68),gabi::at<void>(0x101F3814));
 }else{
  gabi::store<u32>(request+0x64,0);
  gabi::call<void>(0x02019F0C,gabi::at<void>(request+0x68),gabi::at<void>(0x101F37F4));
 }
 return gabi::load<u32>(request+0x44);
}
VERIFY(0x025DCE04,fopScnRq_Request);
u32 fopScnRq_ReRequest(u32 a,u32 b,u32 c){WWHD_FUNC(0x025DCF08,u32,a,b,c);return gabi::call<u32>(0x025E0A20,a,b,c);}
VERIFY(0x025DCF08,fopScnRq_ReRequest);
u32 fopScnRq_Management(){WWHD_FUNC(0x025DCF0C,u32);return gabi::call<u32>(0x025E062C);}
VERIFY(0x025DCF0C,fopScnRq_Management);
void f_op_scene_req_static_init() {
 WWHD_FUNC(0x025DCF10,void);
 gabi::store<u32>(0x1048A628,0);gabi::store<u32>(0x1048A620,0);gabi::store<u32>(0x1048A62C,0);gabi::store<u32>(0x1048A624,0);
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101F3834));
 f32 lo=gabi::load<f32>(0x10057DA4),hi=gabi::load<f32>(0x10057DA8);
 gabi::store<f32>(0x1048A614,lo);gabi::store<f32>(0x1048A618,hi);
 gabi::call<void>(0x028ED6F8,gabi::at<void>(0x1048A61C));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101F3840));
 gabi::call<void>(0x028EAB2C,gabi::at<void>(0x1048A61D));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101F384C));
}
VERIFY(0x025DCF10,f_op_scene_req_static_init);
