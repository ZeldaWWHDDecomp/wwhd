#include "wwhd.h"
#include "gabi.h"
u32 Kyeff2_Draw(u32 self) {
 WWHD_FUNC(0x02586BC0,u32,self);
 gabi::call<void>(0x0257D174);
 return 1;
}
VERIFY(0x02586BC0,Kyeff2_Draw);
u32 Kyeff2_Move(u32 self) {
 WWHD_FUNC(0x02586BE4,u32,self);
 gabi::call<void>(0x0257C5B0);
 return 1;
}
VERIFY(0x02586BE4,Kyeff2_Move);
u32 Kyeff2_Execute(u32 self) {
 WWHD_FUNC(0x02586C08,u32,self);
 return Kyeff2_Move(self);
}
VERIFY(0x02586C08,Kyeff2_Execute);
u32 Kyeff2_IsDelete(u32 self) {
 WWHD_FUNC(0x02586C0C,u32,self);
 return 1;
}
VERIFY(0x02586C0C,Kyeff2_IsDelete);
u32 Kyeff2_Delete(u32 self) {
 WWHD_FUNC(0x02586C14,u32,self);
 if(self)gabi::call<void>(0x025DD630,self,0);
 gabi::call<void>(0x025782D0);
 return 1;
}
VERIFY(0x02586C14,Kyeff2_Delete);
u32 Kyeff2_Create(u32 self) {
 WWHD_FUNC(0x02586C48,u32,self);
 if(self) {
  gabi::call<void>(0x025DD5F0,self);
  gabi::store<u32>(self+0xB4,0x100505B4);
 }
 gabi::call<void>(0x02577E00);
 return 4;
}
VERIFY(0x02586C48,Kyeff2_Create);
void Kyeff2_Init() {
 WWHD_FUNC(0x02586C90,void);
 gabi::store<u32>(0x10477608,0);
 gabi::store<u32>(0x10477600,0);
 gabi::store<u32>(0x1047760C,0);
 gabi::store<u32>(0x10477604,0);
 gabi::call<void>(0x028F026C,0x101E9B44);
 gabi::store<f32>(0x104775F4,-3.1415927410125732f);
 gabi::store<f32>(0x104775F8,3.1415927410125732f);
 gabi::call<void>(0x028ED6F8,0x104775FC);
 gabi::call<void>(0x028F026C,0x101E9B50);
 gabi::call<void>(0x028EAB2C,0x104775FD);
 gabi::call<void>(0x028F026C,0x101E9B5C);
}
VERIFY(0x02586C90,Kyeff2_Init);
