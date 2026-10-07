#include "gabi.h"
using namespace gabi;
namespace hd_screen_btn_map_icon {
// HD-only map coordinate helpers. Addresses and offsets come from the binary.
f32 chasePoint(u32 point,u32 target,f32 ratio,f32 maximum,f32 minimum) {
 WWHD_FUNC(0x0263862C,f32,point,target,ratio,maximum,minimum);
 f32 x=load<f32>(point),y=load<f32>(point+4),tx=load<f32>(target),ty=load<f32>(target+4);
 if(x!=tx || y!=ty) {
  f32 dy=y-ty,dx=x-tx;
  f32 length=call<f32>(0x028F4384,fmadds(dx,dx,dy*dy));
  bool snap=length<minimum;
  if(!snap) {
   f32 step=length*ratio; dx=dx*ratio; dy=dy*ratio;
   snap=step==0.0f;
   if(!snap) {
    if(step>maximum || step<minimum) {
     f32 bound=step>maximum?maximum:minimum;
     f32 scale=bound/step; dy=dy*scale; dx=dx*scale;
    }
    x=load<f32>(point)-dx; y=load<f32>(point+4)-dy;
    store<f32>(point,x);store<f32>(point+4,y);
    tx=load<f32>(target);ty=load<f32>(target+4);
   }
  }
  if(snap) {x=load<f32>(target);store<f32>(point,x);y=load<f32>(target+4);store<f32>(point+4,y);ty=load<f32>(target+4);tx=load<f32>(target);}
 }
 f32 dy=y-ty,dx=x-tx;
 return call<f32>(0x028F4384,fmadds(dx,dx,dy*dy));
}
VERIFY(0x0263862C,chasePoint);
u32 matchesCell(u32 self,u32 offset) {
 u32 owner=load<u32>(self+0x164);if(!owner)return 0;
 u32 info=load<u32>(owner+0x118);if(!info)return 0;
 u32 map=load<u32>(load<u32>(0x101F8344)+0x218);
 return load<s8>(map+offset)==load<s8>(info+4) && load<s8>(map+offset+1)==load<s8>(info+5);
}
u32 matchesCell1(u32 self){WWHD_FUNC(0x0263896C,u32,self);return matchesCell(self,0x3C);}
VERIFY(0x0263896C,matchesCell1);
u32 matchesCell2(u32 self){WWHD_FUNC(0x02639184,u32,self);return matchesCell(self,0x4C);}
VERIFY(0x02639184,matchesCell2);
void mapPoint(u32 out,u32 cell,u32 pos) {
 u32 map=load<u32>(load<u32>(0x101F8344)+0x218);
 f32 cx=(f32)load<s8>(map+cell),cy=(f32)load<s8>(map+cell+1);
 f32 x=load<f32>(map+pos),y=load<f32>(map+pos+4);
 store<f32>(out,fnmsubs(cx,100000.0f,x));store<f32>(out+4,fnmsubs(cy,100000.0f,y));
}
void mapPoint1(u32 self,u32 out){WWHD_FUNC(0x026389EC,void,self,out);mapPoint(out,0x3C,0x30);}
VERIFY(0x026389EC,mapPoint1);
void mapPoint2(u32 self,u32 out){WWHD_FUNC(0x02639204,void,self,out);mapPoint(out,0x4C,0x40);}
VERIFY(0x02639204,mapPoint2);
u32 insideBounds(u32 self,u32 point) {
 WWHD_FUNC(0x02638A8C,u32,self,point);
 f32 x=load<f32>(0x104901E8),y=load<f32>(0x104901EC),nx=-x,ny=-y;
 f32 left,right,bottom,top;
 if(nx>=x){left=x;right=nx;}else{left=nx;right=x;}
 if(ny>=y){bottom=y;top=ny;}else{bottom=ny;top=y;}
 f32 sy=1.0f,sx=1.0f;u32 pane=load<u32>(self+0x94);
 if(pane){sy=load<f32>(pane+0x38);sx=load<f32>(pane+0x34);}
 sy=1.0f/(sy*5.0f);sx=1.0f/(sx*5.0f);
 f32 cx=(left+right)*0.5f,hx=(right-left)*(sx*0.5f);
 f32 cy=(bottom+top)*0.5f,hy=(top-bottom)*(sy*0.5f);
 f32 dx=-(load<f32>(self+0xE4)-load<f32>(self+0xD4));
 f32 dy=-(load<f32>(self+0xE8)-load<f32>(self+0xD8));
 left=(cx-hx)+dx;right=(cx+hx)+dx;bottom=(cy-hy)+dy;top=(cy+hy)+dy;
 f32 px=load<f32>(point),py=load<f32>(point+4);
 if(left>px || px>right || bottom>py || py>top)return 0;
 return 1;
}
VERIFY(0x02638A8C,insideBounds);
void relativePoint(u32 self,u32 out,u32 helper) {
 if(!out)return;
 call<void>(helper,self,out);
 u32 owner=load<u32>(self+0x164);if(!owner)return;
 u32 info=load<u32>(owner+0x118);if(!info)return;
 store<f32>(out,load<f32>(out)-(f32)load<s16>(info+6));
 store<f32>(out+4,load<f32>(out+4)-(f32)load<s16>(info+8));
}
void relativePoint1(u32 self,u32 out){WWHD_FUNC(0x02638C10,void,self,out);relativePoint(self,out,0x026389EC);}
VERIFY(0x02638C10,relativePoint1);
void relativePoint2(u32 self,u32 out){WWHD_FUNC(0x026392A4,void,self,out);relativePoint(self,out,0x02639204);}
VERIFY(0x026392A4,relativePoint2);
void showPane(u32 pane){store<u8>(pane+0x44,(load<u8>(pane+0x44)&0xFE)+1);}
void hidePane(u32 pane){store<u8>(pane+0x44,load<u8>(pane+0x44)&0xFE);}
void updateIn(u32 self,u32 paneOffset,u32 match,u32 map,u32 relative) {
 NativeFrame<0x20> frame;u32 pane=load<u32>(self+paneOffset);if(!pane)return;
 if(!call<u32>(match,self)){hidePane(pane);return;}
 showPane(pane);
 FrameLocal<u64> mapped(0x10);store<f32>(mapped.a,0.0f);store<f32>(mapped.a+4,0.0f);
 call<void>(map,self,mapped.a);
 if(!call<u32>(0x02638A8C,self,mapped.a)){hidePane(pane);return;}
 showPane(pane);
 FrameLocal<u64> local(8);store<f32>(local.a,0.0f);store<f32>(local.a+4,0.0f);
 call<void>(relative,self,local.a);
 f32 x=load<f32>(local.a)*load<f32>(self+0x150);
 f32 y=-(load<f32>(local.a+4)*load<f32>(self+0x154));
 f32 dy=-(load<f32>(self+0xE8)*load<f32>(self+0x14C));
 f32 dx=load<f32>(self+0xE4)*load<f32>(self+0x148);
 store<f32>(local.a,x);store<f32>(local.a+4,y);
 f32 sx=1.0f,sy=1.0f;u32 parent=load<u32>(self+0x94);
 if(parent){sy=load<f32>(parent+0x38);sx=load<f32>(parent+0x34);}
 x=fmadds(x,sx,dx);y=fmadds(y,sy,dy);
 store<f32>(local.a,x);store<f32>(local.a+4,y);
 store<f32>(pane+0x24,0.0f);store<f32>(pane+0x1C,x);store<f32>(pane+0x20,y);store<u8>(pane+0x44,load<u8>(pane+0x44)|0x10);
}
void updateIn1(u32 self){WWHD_FUNC(0x02638CA8,void,self);updateIn(self,0xB8,0x0263896C,0x026389EC,0x02638C10);}
VERIFY(0x02638CA8,updateIn1);
void updateIn2(u32 self){WWHD_FUNC(0x0263933C,void,self);updateIn(self,0x98,0x02639184,0x02639204,0x026392A4);}
VERIFY(0x0263933C,updateIn2);
void updateOut(u32 self,u32 paneOffset,u32 match,u32 relative) {
 NativeFrame<0x18> frame;u32 pane=load<u32>(self+paneOffset);if(!pane)return;
 if(!call<u32>(match,self)){hidePane(pane);return;}
 showPane(pane);FrameLocal<u64> local(8);store<f32>(local.a,0.0f);store<f32>(local.a+4,0.0f);
 call<void>(relative,self,local.a);
 if(!call<u32>(0x02638DE8,self,local.a)){hidePane(pane);return;}
 showPane(pane);
 f32 x=load<f32>(local.a)*load<f32>(self+0x150),y=-(load<f32>(local.a+4)*load<f32>(self+0x154));
 store<f32>(local.a,x);store<f32>(local.a+4,y);
 f32 sx=1.0f,sy=1.0f;u32 parent=load<u32>(self+0x94);
 if(parent){sy=load<f32>(parent+0x38);sx=load<f32>(parent+0x34);}
 x=x*sx;y=y*sy;store<f32>(local.a,x);store<f32>(local.a+4,y);
 store<f32>(pane+0x24,0.0f);store<f32>(pane+0x1C,x);store<f32>(pane+0x20,y);store<u8>(pane+0x44,load<u8>(pane+0x44)|0x10);
}
void updateOut1(u32 self){WWHD_FUNC(0x02638E98,void,self);updateOut(self,0xB8,0x0263896C,0x02638C10);}
VERIFY(0x02638E98,updateOut1);
void updateOut2(u32 self){WWHD_FUNC(0x0263947C,void,self);updateOut(self,0x98,0x02639184,0x026392A4);}
VERIFY(0x0263947C,updateOut2);
}
