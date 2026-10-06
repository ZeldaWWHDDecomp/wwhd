#include "gabi.h"
using namespace gabi;
namespace ext_complete_tail {
void nestedLineDelete(void* self,u32 flags){
 WWHD_FUNC(0x025EE34C,void,self,flags);u32 s=ea(self);if(!s)return;u32 base=load<u32>(s+8);
 if(base){
  for(s32 i=0;i<load<s32>(s+4);i++){
   u32 group=base+(u32)i*20;
   if(group){store<u32>(group,0);
    for(u32 slot: {8u,16u}){
     u32 array=load<u32>(group+slot);
     if(array){
      for(s32 j=0;j<load<s32>(group+slot-4);j++){
       u32 entry=array+(u32)j*0xF4;
       call_ptr(load<u32>(load<u32>(entry+0xF0)+0xC),at<void>(entry),2);
       array=load<u32>(group+slot);
      }
      u32 heap=call<u32>(0x02755FEC,at<void>(load<u32>(0x101F8B4C)),at<void>(array));
      call_ptr(load<u32>(load<u32>(heap+0xC)+0x3C),at<void>(heap),at<void>(load<u32>(group+slot)));
      store<u32>(group+slot-4,0);store<u32>(group+slot,0);
     }
    }
    base=load<u32>(s+8);
   }
  }
  u32 heap=call<u32>(0x02755FEC,at<void>(load<u32>(0x101F8B4C)),at<void>(base));
  call_ptr(load<u32>(load<u32>(heap+0xC)+0x3C),at<void>(heap),at<void>(load<u32>(s+8)));
  store<u32>(s+4,0);store<u32>(s+8,0);
 }
 if(flags&1)call(0x0273AF40,self);
}
VERIFY(0x025EE34C,nestedLineDelete);
void cupForward(void* self,void* arguments){WWHD_FUNC(0x025EE22C,void,self,arguments);call_ptr(load<u32>(load<u32>(ea(self)+0xC)+0x24),self,arguments);}
VERIFY(0x025EE22C,cupForward);
void cupForwardOther(void* self,void* arguments){WWHD_FUNC(0x025EE290,void,self,arguments);call_ptr(load<u32>(load<u32>(ea(self)+0xC)+0x24),self,arguments);}
VERIFY(0x025EE290,cupForwardOther);
void invisibleNoop(){WWHD_FUNC(0x025EE2F4,void);}
VERIFY(0x025EE2F4,invisibleNoop);
s32 material0(){WWHD_FUNC(0x025EE584,s32);return 0;}
VERIFY(0x025EE584,material0);
s32 material1(){WWHD_FUNC(0x025EE58C,s32);return 1;}
VERIFY(0x025EE58C,material1);
void sortNoop(){WWHD_FUNC(0x025EE594,void);}
VERIFY(0x025EE594,sortNoop);
void lineBaseDelete(void* self,u32 flags){WWHD_FUNC(0x025F03AC,void,self,flags);if(ea(self)&&(flags&1))call(0x0273AF40,self);}
VERIFY(0x025F03AC,lineBaseDelete);
void lineNoop(){WWHD_FUNC(0x025F03C0,void);}
VERIFY(0x025F03C0,lineNoop);

/* ---- m_Do_ext tail ---- */

/* 025EE4E8: array-element constructor (12 bytes) used by the line-material init functions
 * (025E9B80, 025EBA58 via the array constructor 028EFFD0): zero word at +0, then the inline
 * constructor of the 8-byte member at +4 (GHS null check of the member address, allocation when
 * it is NULL) zeroes it. */
void* lineElementCtor(void* self){
 WWHD_FUNC(0x025EE4E8,void*,self);
 u32 o=ea(self);
 if(!o){o=call<u32>(0x0273AD10,0xC);if(!o)return nullptr;}
 u32 m=o+4;
 store<u32>(o,0);
 if(!m){m=call<u32>(0x0273AD10,8);if(!m)return at<void>(o);}
 store<u32>(m+4,0);store<u32>(m,0);
 return at<void>(o);
}
VERIFY(0x025EE4E8,lineElementCtor);

/* 025EE558: array-element constructor of an empty 16-byte class (line-material init, 025E9B80):
 * only allocates when this == NULL */
void* lineEmptyElementCtor(void* self){
 WWHD_FUNC(0x025EE558,void*,self);
 u32 o=ea(self);
 if(!o)o=call<u32>(0x0273AD10,0x10);
 return at<void>(o);
}
VERIFY(0x025EE558,lineEmptyElementCtor);

/* 025EE5EC: this TU's sead::SafeString assureTerminationImpl_ (vtable 10058704; also called
 * directly by mDoExt_McaMorf::setAnm / mDoExt_McaMorf2::setAnm): empty */
void safeStringTerminate(void* self){WWHD_FUNC(0x025EE5EC,void,self);}
VERIFY(0x025EE5EC,safeStringTerminate);

/* 025EE5F0: rotation matrix (3x4, r4) -> quaternion {x,y,z,w} (r3), this TU's out-of-line copy of
 * the sead quaternion-from-matrix helper (called by the old-frame and morf2 calc functions).
 * Picks the largest of w^2 = (trace+1)/4 and x^2, y^2, z^2 (= w^2 - (other two diagonals)/2),
 * takes its square root with the frsqrte + one Newton step idiom, divides 1/4 by it and fills
 * the other three components from matrix sums/differences. */
static inline f32 quat_sqrt(f32 v,f32 zero,f32 half,f32 three){
 /* the estimate stays a double; fmuls rounds its second operand to 25 bits (Espresso) */
 f32 r=zero;
 if(v>zero){f64 e=frsqrte(v);f32 sq=(f32)(e*round25(e)),eh=(f32)(e*round25(half));sq=fnmsubs(sq,v,three);r=fmuls_ppc(sq,eh);}
 return fmuls_ppc(r,v);
}
void mtxToQuat(void* outp,void* mtxp){
 WWHD_FUNC(0x025EE5F0,void,outp,mtxp);
 u32 q=ea(outp),m=ea(mtxp);
 f32 m00=load<f32>(m),m11=load<f32>(m+0x14),m22=load<f32>(m+0x28);
 f32 s01=fadds_ppc(m00,m11);
 f32 tr=fadds_ppc(s01,m22);
 f32 one=load<f32>(0x100586D4);
 f32 s20=fadds_ppc(m22,m00);
 tr=fadds_ppc(tr,one);
 f32 quarter=load<f32>(0x100586D0);
 f32 s12=fadds_ppc(m11,m22);
 f32 w2=fmuls_ppc(quarter,tr);
 f32 half=load<f32>(0x100586D8);
 f32 x2=fnmsubs(half,s12,w2),y2=fnmsubs(half,s20,w2),z2=fnmsubs(half,s01,w2);
 int c;
 if(w2>x2){ if(w2>y2){ c=(w2>z2)?0:3; } else c=(y2>z2)?2:3; }
 else if(x2>y2){ c=(x2>z2)?1:3; }
 else c=(y2>z2)?2:3;
 f32 zero=load<f32>(0x100586DC),three=load<f32>(0x100586E0);
 if(c==0){
  f32 r=quat_sqrt(w2,zero,half,three);f32 k=quarter/r;
  store<f32>(q+0xC,r);
  store<f32>(q,fmuls_ppc(fsubs_ppc(load<f32>(m+0x24),load<f32>(m+0x18)),k));
  store<f32>(q+4,fmuls_ppc(fsubs_ppc(load<f32>(m+8),load<f32>(m+0x20)),k));
  store<f32>(q+8,fmuls_ppc(fsubs_ppc(load<f32>(m+0x10),load<f32>(m+4)),k));
 }else if(c==1){
  f32 r=quat_sqrt(x2,zero,half,three);f32 k=quarter/r;
  store<f32>(q,r);
  store<f32>(q+0xC,fmuls_ppc(fsubs_ppc(load<f32>(m+0x24),load<f32>(m+0x18)),k));
  store<f32>(q+4,fmuls_ppc(fadds_ppc(load<f32>(m+4),load<f32>(m+0x10)),k));
  store<f32>(q+8,fmuls_ppc(fadds_ppc(load<f32>(m+8),load<f32>(m+0x20)),k));
 }else if(c==2){
  f32 r=quat_sqrt(y2,zero,half,three);f32 k=quarter/r;
  store<f32>(q+4,r);
  store<f32>(q+0xC,fmuls_ppc(fsubs_ppc(load<f32>(m+8),load<f32>(m+0x20)),k));
  store<f32>(q+8,fmuls_ppc(fadds_ppc(load<f32>(m+0x18),load<f32>(m+0x24)),k));
  store<f32>(q,fmuls_ppc(fadds_ppc(load<f32>(m+0x10),load<f32>(m+4)),k));
 }else{
  f32 r=quat_sqrt(z2,zero,half,three);f32 k=quarter/r;
  store<f32>(q+8,r);
  store<f32>(q+0xC,fmuls_ppc(fsubs_ppc(load<f32>(m+0x10),load<f32>(m+4)),k));
  store<f32>(q,fmuls_ppc(fadds_ppc(load<f32>(m+0x20),load<f32>(m+8)),k));
  store<f32>(q+4,fmuls_ppc(fadds_ppc(load<f32>(m+0x24),load<f32>(m+0x18)),k));
 }
}
VERIFY(0x025EE5F0,mtxToQuat);

/* ---- orphan deleting destructors ----
 * GHS deleting destructors in this TU's vtables (destructor slot +0xC): run the base destructor with
 * flags 0, then free the object when bit 0 of the flags is set; nothing when this is NULL.
 * The matcher's name J3DFrameCtrl::~J3DFrameCtrl is wrong for all five. */
static inline void deletingDtor(void* self,u32 flags,u32 base){
 if(!ea(self))return;
 call(base,self,0);
 if(flags&1)call(0x0273AF40,self);
}
/* 025EE1D8: mDoExt_McaMorf's calculation-interface vtable 10058CC0 (set by the constructor 025E4F64);
 * the base destructor 027F3628 is the empty J3DMtxCalc-family destructor */
void morfCalcDelete(void* self,u32 flags){WWHD_FUNC(0x025EE1D8,void,self,flags);deletingDtor(self,flags,0x027F3628);}
VERIFY(0x025EE1D8,morfCalcDelete);
/* 025EE23C: mDoExt_offCupOnAupPacket (vtable 10058D10, draw 025E8728); base J3DPacket destructor 027F13DC */
void offCupOnAupDelete(void* self,u32 flags){WWHD_FUNC(0x025EE23C,void,self,flags);deletingDtor(self,flags,0x027F13DC);}
VERIFY(0x025EE23C,offCupOnAupDelete);
/* 025EE2A0: mDoExt_onCupOffAupPacket (vtable 10058D40, draw 025E8774) */
void onCupOffAupDelete(void* self,u32 flags){WWHD_FUNC(0x025EE2A0,void,self,flags);deletingDtor(self,flags,0x027F13DC);}
VERIFY(0x025EE2A0,onCupOffAupDelete);
/* 025EE2F8: mDoExt_invJntPacket (vtable 10058D70, draw 025E87C0) */
void invJntDelete(void* self,u32 flags){WWHD_FUNC(0x025EE2F8,void,self,flags);deletingDtor(self,flags,0x027F13DC);}
VERIFY(0x025EE2F8,invJntDelete);
/* 025EE598: mDoExt_3DlineMatSortPacket (vtable 10058E20, set by the constructor 025EDC2C; draw 025EDC88) */
void lineMatSortDelete(void* self,u32 flags){WWHD_FUNC(0x025EE598,void,self,flags);deletingDtor(self,flags,0x027F13DC);}
VERIFY(0x025EE598,lineMatSortDelete);
}
