#include "wwhd.h"
#include "gabi.h"
u32 fopScnM_SearchByID(u32 id) {
 WWHD_FUNC(0x025DC80C,u32,id);
 gabi::Local<be<u32>> key;*key=id;
 return gabi::call<u32>(0x025DC73C,gabi::at<void>(0x025E1234),key.get());
}
VERIFY(0x025DC80C,fopScnM_SearchByID);
u32 fopScnM_SearchByName(u32 name) {
 WWHD_FUNC(0x025DC83C,u32,name);
 gabi::Local<be<u32>> key;gabi::store<u16>(gabi::ea(key.get()),u16(name));
 return gabi::call<u32>(0x025DC73C,gabi::at<void>(0x025E121C),key.get());
}
VERIFY(0x025DC83C,fopScnM_SearchByName);
s32 fopScnM_ChangeReq(u32 a,u32 b,u32 c,u32 d,u32 e) {
 WWHD_FUNC(0x025DC86C,s32,a,b,c,d,e);
 s32 result=gabi::call<s32>(0x025DCE04,2,a,b,0,c,d,e);
 if(result==-1) return 0;
 gabi::store<s32>(0x101F3794,result);return 1;
}
VERIFY(0x025DC86C,fopScnM_ChangeReq);
s32 fopScnM_DeleteReq(u32 a) {
 WWHD_FUNC(0x025DC8D0,s32,a);
 return gabi::call<s32>(0x025DCE04,1,a,0x7FFF,0,0x7FFF,0,0)!=-1;
}
VERIFY(0x025DC8D0,fopScnM_DeleteReq);
s32 fopScnM_CreateReq(u32 a,u32 b,u32 c,u32 d) {
 WWHD_FUNC(0x025DC91C,s32,a,b,c,d);
 return gabi::call<s32>(0x025DCE04,0,0,a,d,b,c,0)!=-1;
}
VERIFY(0x025DC91C,fopScnM_CreateReq);
u32 fopScnM_ReRequest(u32 a,u32 b) {
 WWHD_FUNC(0x025DC964,u32,a,b);
 u32 request=gabi::load<u32>(0x101F3794);
 if(request==0xFFFFFFFF) return 0;
 return gabi::call<u32>(0x025DCF08,request,a,b);
}
VERIFY(0x025DC964,fopScnM_ReRequest);
void fopScnM_Management() {
 WWHD_FUNC(0x025DC98C,void);
 if(gabi::call<s32>(0x025DCF0C)==0) gabi::call<void>(0x0273AA24,gabi::at<void>(0x10057D68),0x148,gabi::at<void>(0x10057D64));
}
VERIFY(0x025DC98C,fopScnM_Management);
void f_op_scene_mng_static_init() {
 WWHD_FUNC(0x025DC9CC,void);
 gabi::store<u32>(0x1048A5F0,0);gabi::store<u32>(0x1048A5E8,0);gabi::store<u32>(0x1048A5F4,0);gabi::store<u32>(0x1048A5EC,0);
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101F3798));
 f32 lo=gabi::load<f32>(0x10057D84),hi=gabi::load<f32>(0x10057D88);
 gabi::store<f32>(0x1048A5DC,lo);gabi::store<f32>(0x1048A5E0,hi);
 gabi::call<void>(0x028ED6F8,gabi::at<void>(0x1048A5E4));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101F37A4));
 gabi::call<void>(0x028EAB2C,gabi::at<void>(0x1048A5E5));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101F37B0));
}
VERIFY(0x025DC9CC,f_op_scene_mng_static_init);

/* 025DCA60 fopScnM_Init (empty, as on GameCube; called by fapGm_Create): empty function */
static void fopScnM_Init() {
    WWHD_FUNC(0x025DCA60, void);
}
VERIFY(0x025DCA60, fopScnM_Init);
