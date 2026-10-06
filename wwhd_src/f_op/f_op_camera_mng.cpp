#include "wwhd.h"
#include "gabi.h"
u32 fopCamM_GetParam(void* camera) {
 WWHD_FUNC(0x025DA64C,u32,camera);
 return gabi::load<u32>(gabi::ea(camera)+0xB0);
}
VERIFY(0x025DA64C,fopCamM_GetParam);
u32 fopCamM_Create(u32 index,u32 parameter,u32 userData) {
 WWHD_FUNC(0x025DA654,u32,index,parameter,userData);
 u32 layer=gabi::call<u32>(0x025DED64,index);
 u32 result=gabi::call<u32>(0x025E14A8,layer,parameter,0,0,userData);
 gabi::store<u32>(0x10487554+index*4,result);
 return result;
}
VERIFY(0x025DA654,fopCamM_Create);
void f_op_camera_mng_static_init() {
 WWHD_FUNC(0x025DA6BC,void);
 gabi::store<u32>(0x1048756C,0);gabi::store<u32>(0x10487564,0);gabi::store<u32>(0x10487570,0);gabi::store<u32>(0x10487568,0);
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101F336C));
 f32 lo=gabi::load<f32>(0x10057964),hi=gabi::load<f32>(0x10057968);
 gabi::store<f32>(0x10487548,lo);gabi::store<f32>(0x1048754C,hi);
 gabi::call<void>(0x028ED6F8,gabi::at<void>(0x10487550));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101F3378));
 gabi::call<void>(0x028EAB2C,gabi::at<void>(0x10487551));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101F3384));
}
VERIFY(0x025DA6BC,f_op_camera_mng_static_init);

/* 025DA750 fopCamM_Management (empty, as on GameCube; called by fapGm_After): empty function */
static void fopCamM_Management() {
    WWHD_FUNC(0x025DA750, void);
}
VERIFY(0x025DA750, fopCamM_Management);

/* 025DA754 fopCamM_Init (empty, as on GameCube; called by fapGm_Create): empty function */
static void fopCamM_Init() {
    WWHD_FUNC(0x025DA754, void);
}
VERIFY(0x025DA754, fopCamM_Init);
