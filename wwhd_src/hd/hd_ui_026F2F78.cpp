/* HD UI part item scheduler and animation completion path, binary-derived.
 * Original behaviour only. */
#include "gabi.h"
using namespace gabi;
namespace hd_ui_026F2F78 {
static s32 divided(s32 a,s32 b) {
 if(!b || (a==s32(0x80000000u) && b==-1)) return a<0?-1:0;
 return a/b;
}
static s32 sub(s32 a,s32 b) { return s32(u32(a)-u32(b)); }
static void item(u32 self,s32 index) { call<void>(0x026F47D0,self,u32(index)); }
u32 updateItems(u32 self,f32 before,f32 now) {
 WWHD_FUNC(0x026F48A4,u32,self,before,now);
 s32 count=load<s32>(self+0xD00);
 if(count==0) return 1;
 f32 one=load<f32>(0x10109A98),zero=load<f32>(0x10109AA4),progress=zero;
 u32 config=load<u32>(self+0xCF4);
 if(count==1) {
  u32 animator=0;
  if(load<u32>(config+0x60)) animator=load<u32>(load<u32>(config+0x68));
  if(load<f32>(animator+0x24)==zero)
   call_ptr<void>(load<u32>(load<u32>(animator+0x14)+0xAC),animator,one);
  return 1;
 }
 s32 first=0,last=0;
 if(load<s32>(config+0x4C)<=0) {
  progress=one;first=0;last=sub(count,1);
 } else {
  u32 mode=load<u32>(config+0x58);
  if(mode==0 || mode==4) {
   s32 duration=load<s32>(config+0x4C);
   s32 end=sub(count,1);
   if(mode==4) end=s32(u32(end)+load<u32>(config+0x50));
   // GHS first divides time by duration, then by 1/(count-1 or extended end).
   f32 divisor=one/f32(end);
   progress=before/f32(duration);
   first=ftoi(progress/divisor);
   last=ftoi((now/f32(duration))/divisor);
   if(mode==4) {
    s32 edge=sub(count,1);
    if(first>=end) {
     first=edge;
     if(last>=end) last=edge;
     else if(last>=edge) last=sub(edge,1);
    } else {
     if(first>=edge) first=sub(edge,1);
     if(last>=end) last=edge;
     else if(last>=edge) last=sub(edge,1);
    }
   }
  } else if(mode==1 || mode==2 || mode==3) {
   s32 delay=load<s32>(self+0xD04);
   if(delay!=0) {
    store<u32>(self+0xD04,u32(delay)-1);
    first=0;last=0;
   } else {
    s32 remaining=load<s32>(self+0xD08);
    last=sub(count,remaining);first=last;
    store<u32>(self+0xD08,u32(remaining)-1);
    s32 next;
    if(mode==1) {
     u32 r=u32(remaining)-1;
     next=divided(s32(r*r),load<s32>(self+0xD0C));
    } else if(mode==2) {
     // Count is reloaded here before the countdown store in the original.
     u32 r=load<u32>(self+0xD00)-(u32(remaining)-1);
     next=divided(s32(r*r),load<s32>(self+0xD0C));
    } else {
     config=load<u32>(self+0xCF4);
     s32 maximum=load<s32>(config+0x4C);
     next=s32(u32(ftoi(call<f32>(0x020198D8,f32(maximum))))+1);
     if(next>maximum) next=maximum;
    }
    store<u32>(self+0xD04,u32(next));
    s32 remainingNow=load<s32>(self+0xD08);
    config=load<u32>(self+0xCF4);
    if(remainingNow<=0) progress=one;
   }
  }
 }
 u32 order=load<u32>(config+0x54);
 if(order==0) {
  for(s32 i=first;i<=last;++i) {
   if(i>=load<s32>(self+0xD00)) break;
   item(self,i);
  }
 } else if(order==1) {
  for(s32 i=first;i<=last;++i) {
   s32 n=load<s32>(self+0xD00);if(i>=n) break;
   item(self,sub(sub(n,i),1));
  }
 } else if(order==2) {
  for(s32 i=first;i<=last;++i) {
   s32 n=load<s32>(self+0xD00);
   if(n%2==1) {
    s32 half=i/2,center=sub(n,1)/2;
    item(self,sub(center,half));item(self,s32(u32(center)+u32(half)));
   } else if(i!=0) {
    s32 half=sub(i,1)/2,center=n/2;
    item(self,sub(sub(center,half),1));item(self,s32(u32(center)+u32(half)));
   }
  }
 } else if(order==3) {
  for(s32 i=first;i<=last;++i) {
   s32 half=i/2;item(self,half);
   s32 n=load<s32>(self+0xD00);item(self,sub(sub(n,half),1));
  }
 } else if(order==4) {
  for(s32 i=first;i<=last;++i) {
   if(i>=load<s32>(self+0xD00)) break;
   s32 index=load<s32>(load<u32>(self+0xCF8)+u32(i)*4);
   if(index>=0) item(self,index);
  }
 }
 return !(progress<one);
}
VERIFY(0x026F48A4,updateItems);
void update(u32 self) {
 WWHD_FUNC(0x026F4FD8,void,self);
 if(!load<u32>(self+0xCF4)) return;
 if(load<u8>(self+0xD10)) {
  f32 previous=load<f32>(self+0xCFC);
  f32 current=fadds_ppc(previous,load<f32>(0x10109A98));
  store<f32>(self+0xCFC,current);
  if(call<u32>(0x026F48A4,self,previous,current)) store<u8>(self+0xD10,0);
 } else {
  u32 animator=load<u32>(self+0xD14);
  if(!animator) return;
  if(load<f32>(animator+0x24)!=load<f32>(0x10109AA4) && !(load<u32>(animator+0x2C)&2)) return;
  u32 config=load<u32>(self+0xCF4);
  store<u32>(self+0xD14,0);
  if(load<u8>(config+0x5D)) call<void>(0x026F4EA8,self,config);
 }
}
VERIFY(0x026F4FD8,update);
}
