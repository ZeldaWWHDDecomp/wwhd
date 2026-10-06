#include "wwhd.h"
#include "gabi.h"
namespace cp { template<class T>T ld(u32 a,u32 o=0){return gabi::load<T>(a+o);} template<class T>void st(u32 a,u32 o,T v){gabi::store<T>(a+o,v);} void* p(u32 a){return gabi::at<void>(a);} }
using namespace cp;
void* cp_stick_ctor(void* self){
 WWHD_FUNC(0x024F6DEC,void*,self);
 u32 a=gabi::ea(self);if(!a)a=gabi::call<u32>(0x0273AD10,0x14);
 if(a){f32 upper=ld<f32>(0x10044734),lower=ld<f32>(0x10044730);st<u32>(a,0,0x10044708);st<s32>(a,0xC,6);st<u16>(a,0x10,0);st<f32>(a,8,upper);st<f32>(a,4,lower);}return p(a);
}
VERIFY(0x024F6DEC,cp_stick_ctor);
s32 cp_stick_shift(void* self,u32 flags){
 WWHD_FUNC(0x024F6E54,s32,self,flags);
 return 0;
}
VERIFY(0x024F6E54,cp_stick_shift);
BOOL cp_change(void* self,s32 index){
 WWHD_FUNC(0x024F727C,BOOL,self,index);
 u32 a=gabi::ea(self);st<s32>(a,8,index);s32 count=ld<s32>(0x10044878);u32 style=0x1004487C;
 if(index<count){style+=u32(index)*0x84;st<u32>(a,4,style);return 1;}st<u32>(a,4,style);return 0;
}
VERIFY(0x024F727C,cp_change);
void* cp_param_ctor(void* self,s32 index){
 WWHD_FUNC(0x024F72BC,void*,self,index);
 u32 a=gabi::ea(self);if(!a)a=gabi::call<u32>(0x0273AD10,0xC);
 if(a){st<u32>(a,0,0x10044840);gabi::call<void>(0x024F727C,p(a),index);}
 return p(a);
}
VERIFY(0x024F72BC,cp_param_ctor);
void cp_param_dtor(void* self,s32 flag){
 WWHD_FUNC(0x024F7318,void,self,flag);
 if(self&&(u32(flag)&1))gabi::call<void>(0x0273AF40,self);
}
VERIFY(0x024F7318,cp_param_dtor);
s32 cp_search(void* self,u32 name){
 WWHD_FUNC(0x024F732C,s32,self,name);
 s32 count=ld<s32>(0x10044878);if(count<=0)return -1;
 for(s32 i=0;i<count;i++){if(ld<u32>(0x1004487C+u32(i)*0x84)==name)return i;}return -1;
}
VERIFY(0x024F732C,cp_search);
BOOL cp_default_radius(void* self,void* value){
 WWHD_FUNC(0x024F7400,BOOL,self,value);
 u32 style=ld<u32>(gabi::ea(self),4),a=gabi::ea(value);f32 lo=ld<f32>(style,0x3C),hi=ld<f32>(style,0x40);
 if(!(lo<hi)){f32 copy=lo;lo=hi;hi=copy;}f32 val=ld<f32>(a);if(val>hi){st<f32>(a,0,hi);return 0;}if(val<lo){st<f32>(a,0,lo);return 0;}return 1;
}
VERIFY(0x024F7400,cp_default_radius);
f32 cp_lockon_fovy(void* self,f32 t){
 WWHD_FUNC(0x024F75B4,f32,self,t);
 u32 a=ld<u32>(gabi::ea(self),4);f32 low=ld<f32>(a,0x78),high=ld<f32>(a,0x7C);return gabi::fmadds(gabi::fsubs_ppc(high,low),t,low);
}
VERIFY(0x024F75B4,cp_lockon_fovy);
f32 cp_lockon_center(void* self,f32 t){
 WWHD_FUNC(0x024F75CC,f32,self,t);
 u32 a=ld<u32>(gabi::ea(self),4);f32 low=ld<f32>(a,0x28),high=ld<f32>(a,0x2C);return gabi::fmadds(gabi::fsubs_ppc(high,low),t,low);
}
VERIFY(0x024F75CC,cp_lockon_center);
void cp_setup_dtor(void* self,s32 flag){
 WWHD_FUNC(0x024F7858,void,self,flag);
 if(self){st<u32>(gabi::ea(self),0x104,0x10044720);if(u32(flag)&1)gabi::call<void>(0x0273AF40,self);}
}
VERIFY(0x024F7858,cp_setup_dtor);
void* cp_bg_ctor(void* self){
 WWHD_FUNC(0x024F6E5C,void*,self);
 u32 a=gabi::ea(self);if(!a)a=gabi::call<u32>(0x0273AD10,0x64);
 if(a){
 f32 v1=ld<f32>(0x10044750);
 f32 v2=ld<f32>(0x10044748);
 f32 v3=ld<f32>(0x1004475C);
 st<f32>(a,0x14,v2);
 f32 v4=ld<f32>(0x10044758);
 st<f32>(a,0x1c,v1);
 f32 v5=ld<f32>(0x10044768);
 st<f32>(a,0x34,v3);
 f32 v6=ld<f32>(0x10044730);
 st<f32>(a,0x24,v4);
 f32 v7=ld<f32>(0x1004473C);
 st<f32>(a,0x28,v3);
 f32 v8=ld<f32>(0x10044764);
 st<f32>(a,0x30,v6);
 f32 v9=ld<f32>(0x10044738);
 st<f32>(a,0x38,v8);
 f32 v10=ld<f32>(0x10044760);
 st<f32>(a,4,v9);
 f32 v11=ld<f32>(0x10044740);
 st<f32>(a,0x2c,v10);
 f32 v12=ld<f32>(0x10044744);
 st<f32>(a,0xc,v11);
 st<f32>(a,0x10,v12);
 f32 v13=ld<f32>(0x1004474C);
 st<f32>(a,8,v7);
 f32 v14=ld<f32>(0x10044754);
 st<f32>(a,0x18,v13);
 f32 v15=ld<f32>(0x1004476C);
 st<f32>(a,0x20,v14);
 f32 v16=ld<f32>(0x10044770);
 f32 v17=ld<f32>(0x10044774);
 st<f32>(a,0x50,v15);
 st<f32>(a,0x48,v17);
 f32 v18=ld<f32>(0x10044778);
 st<f32>(a,0x44,v16);
 f32 v19=ld<f32>(0x1004477C);
 st<f32>(a,0x40,v15);
 st<f32>(a,0x60,v19);
 f32 v20=ld<f32>(0x10044780);
 st<f32>(a,0x54,v5);
 st<f32>(a,0x5c,v20);
 st<u32>(a,0,0x10044720);
 st<f32>(a,0x4c,v18);
 st<f32>(a,0x58,v5);
 st<f32>(a,0x3c,v5);
 }return p(a);
}
VERIFY(0x024F6E5C,cp_bg_ctor);
f32 cp_custom_ratio(f32 x,f32 y){
 WWHD_FUNC(0x024F7194,f32,x,y);
 f32 cutoff=ld<f32>(0x100447C8);if(x>cutoff){f32 neg=ld<f32>(0x100447CC),pos=ld<f32>(0x1004473C);return x>=0?pos:neg;}
 return gabi::call<f32>(0x024F6F9C,gabi::fmuls_ppc(x,ld<f32>(0x100447D0)),y);
}
VERIFY(0x024F7194,cp_custom_ratio);
f32 cp_ratio(void* self,f32 t,f32 upper,f32 lower,f32 base){
 WWHD_FUNC(0x024F71CC,f32,self,t,upper,lower,base);
 f32 zero=ld<f32>(0x10044790);if(t==zero)return base;
 f32 quarter=ld<f32>(0x10044768);f32 span=t<zero?gabi::fsubs_ppc(base,lower):gabi::fsubs_ppc(upper,base);
 f32 curve=gabi::call<f32>(0x024F7194,t,quarter);return gabi::fmadds(span,curve,base);
}
VERIFY(0x024F71CC,cp_ratio);
f32 cp_radius_ratio(void* self,f32 radius){
 WWHD_FUNC(0x024F7370,f32,self,radius);
 f32 eps=ld<f32>(0x100447D4);u32 a=ld<u32>(gabi::ea(self),4);f32 center=ld<f32>(a,0x30);
 if(radius<center){f32 low=ld<f32>(a,0x34),dist=gabi::fsubs_ppc(center,radius),span=gabi::fsubs_ppc(center,low);if(!(dist<span))return ld<f32>(0x100447CC);if(span>eps)return -(dist/span);}
 else if(radius>center){f32 high=ld<f32>(a,0x38),dist=gabi::fsubs_ppc(radius,center),span=gabi::fsubs_ppc(high,center);if(!(dist<span))return ld<f32>(0x1004473C);if(span>eps)return dist/span;}
 return ld<f32>(0x10044790);
}
VERIFY(0x024F7370,cp_radius_ratio);
f32 cp_center_height(void* self,f32 t){
 WWHD_FUNC(0x024F7464,f32,self,t);
 u32 a=ld<u32>(gabi::ea(self),4);f32 upper=ld<f32>(a,0x20),base=ld<f32>(a,0x1C),lower=ld<f32>(a,0x24);
 return gabi::call<f32>(0x024F71CC,self,t,upper,lower,base);
}
VERIFY(0x024F7464,cp_center_height);
f32 cp_fovy(void* self,f32 t){
 WWHD_FUNC(0x024F7478,f32,self,t);
 u32 a=ld<u32>(gabi::ea(self),4);f32 upper=ld<f32>(a,0x70),base=ld<f32>(a,0x6C),lower=ld<f32>(a,0x74);
 return gabi::call<f32>(0x024F71CC,self,t,upper,lower,base);
}
VERIFY(0x024F7478,cp_fovy);
BOOL cp_latitude_range(void* self,void* angle){
 WWHD_FUNC(0x024F7878,BOOL,self,angle);
 u32 a=gabi::ea(self),v=gabi::ea(angle);f32 high=ld<f32>(a,0x64),factor=ld<f32>(0x100447D8);s16 max=s16(gabi::ftoi(gabi::fmuls_ppc(high,factor)));f32 low=ld<f32>(a,0x60);s16 min=s16(gabi::ftoi(gabi::fmuls_ppc(low,factor)));s16 val=ld<s16>(v);
 if(val>max){st<s16>(v,0,max);return 0;}if(val<min){st<s16>(v,0,min);return 0;}return 1;
}
VERIFY(0x024F7878,cp_latitude_range);
f32 cp_fan_bank(void* self){
 WWHD_FUNC(0x024F78F0,f32,self);
 u32 a=gabi::ea(self);f32 width=ld<f32>(a,0x8C),random=gabi::call<f32>(0x02019918,width),wind=gabi::call<f32>(0x02578348),base=ld<f32>(a,0x88);
 return gabi::fmadds(random,wind,base);
}
VERIFY(0x024F78F0,cp_fan_bank);
void* cp_setup_ctor(void* self){
 WWHD_FUNC(0x024F75E4,void*,self);
 u32 a=gabi::ea(self);if(!a)a=gabi::call<u32>(0x0273AD10,0x168);
 if(a){st<u32>(a,0,0x10044858);if(!(a+0xD4))gabi::call<void>(0x0273AD10,0xC);gabi::call<void>(0x024F6DEC,p(a+0xF0));gabi::call<void>(0x024F6E5C,p(a+0x104));
 f32 q1=ld<f32>(0x100447E4);
 f32 q2=ld<f32>(0x100447E0);
 st<f32>(a,0x60,q1);
 f32 q3=ld<f32>(0x1004473C);
 f32 q4=ld<f32>(0x100447E8);
 st<f32>(a,4,q3);
 st<f32>(a,0x2c,q4);
 f32 q5=ld<f32>(0x100447F0);
 f32 q6=ld<f32>(0x100447F8);
 f32 q7=ld<f32>(0x100447FC);
 st<u32>(a,0x20,0xffffffff);
 f32 q8=ld<f32>(0x10044800);
 f32 q9=ld<f32>(0x10044764);
 f32 q10=ld<f32>(0x1004477C);
 st<f32>(a,0x28,q2);
 f32 q11=ld<f32>(0x100447DC);
 st<f32>(a,0x4c,q10);
 st<f32>(a,0x40,q3);
 f32 q12=ld<f32>(0x10044740);
 f32 q13=ld<f32>(0x1004476C);
 st<f32>(a,8,q11);
 f32 q14=ld<f32>(0x10044730);
 st<f32>(a,0x34,q9);
 st<f32>(a,0x50,q3);
 f32 q15=ld<f32>(0x100447F4);
 f32 q16=ld<f32>(0x10044804);
 st<f32>(a,0x3c,q15);
 st<f32>(a,0xa0,q16);
 st<f32>(a,0x58,q8);
 f32 q17=ld<f32>(0x10044780);
 st<f32>(a,0x48,q7);
 st<f32>(a,0x38,q12);
 st<f32>(a,0x54,q5);
 st<f32>(a,0x64,q13);
 f32 q18=ld<f32>(0x100447EC);
 f32 q19=ld<f32>(0x10044750);
 st<f32>(a,0x30,q18);
 f32 q20=ld<f32>(0x1004480C);
 f32 q21=ld<f32>(0x10044810);
 st<u32>(a,0x14,0xffffffff);
 st<f32>(a,0xb4,q21);
 st<f32>(a,0x70,q19);
 st<f32>(a,0x68,q17);
 st<f32>(a,0x24,q14);
 st<f32>(a,0x6c,q17);
 st<f32>(a,0x44,q16);
 st<u32>(a,0x10,0x1);
 f32 q22=ld<f32>(0x10044790);
 f32 q23=ld<f32>(0x10044808);
 st<f32>(a,0xc4,q22);
 f32 q24=ld<f32>(0x10044758);
 f32 q25=ld<f32>(0x10044814);
 st<f32>(a,0xa4,q24);
 st<f32>(a,0xc0,q25);
 st<f32>(a,0xa8,q14);
 f32 q26=ld<f32>(0x10044818);
 st<f32>(a,0x5c,q6);
 st<f32>(a,0xd8,q22);
 f32 q27=ld<f32>(0x10044760);
 st<f32>(a,0x74,q26);
 f32 q28=ld<f32>(0x1004475C);
 st<f32>(a,0x90,q27);
 f32 q29=ld<f32>(0x10044824);
 f32 q30=ld<f32>(0x10044820);
 st<f32>(a,0xd4,q29);
 st<u32>(a,0xd0,0xffffffff);
 st<u16>(a,0xc,0x1);
 st<f32>(a,0xdc,q22);
 st<f32>(a,0x8c,q30);
 st<f32>(a,0xac,q20);
 f32 q31=ld<f32>(0x1004481C);
 st<f32>(a,0xc8,q16);
 st<f32>(a,0x78,q31);
 st<u32>(a,0x9c,0x96);
 st<f32>(a,0x88,q28);
 st<f32>(a,0x94,q23);
 st<u32>(a,0xb8,0x3c);
 st<u32>(a,0xb0,0x14);
 st<f32>(a,0xe4,q23);
 f32 q32=ld<f32>(0x10044828);
 f32 q33=ld<f32>(0x10044754);
 f32 q34=ld<f32>(0x10044744);
 st<f32>(a,0xec,q32);
 st<f32>(a,0x84,q34);
 st<f32>(a,0xe0,q5);
 st<f32>(a,0xe8,q13);
 st<f32>(a,0xbc,q6);
 st<f32>(a,0xcc,q5);
 st<f32>(a,0x80,q33);
 st<u32>(a,0x7c,0x78);
 }return p(a);
}
VERIFY(0x024F75E4,cp_setup_ctor);
f32 cp_rational(f32 input,f32 weight){
 WWHD_FUNC(0x024F6F9C,f32,input,weight);
 f64 x=input,w=weight,one=ld<f64>(0x10044788),sign=one;
 f32 zero=ld<f32>(0x10044790);if(input<zero){x=-x;sign=ld<f64>(0x10044798);}
 f64 twice=x+x,twicew=w+w,b=gabi::fmsub(twice,w,twice)-twicew,neg=-b,a=neg-one;
 f64 four=ld<f64>(0x100447A0),disc=gabi::fmsub(b,b,(four*a)*x),sq=ld<f64>(0x100447A8);
 if(disc>sq)sq=gabi::call<f32>(0x028F4384,f32(disc));
 f64 denom=a+a,hi=ld<f64>(0x100447B0),numerator=neg-sq;
 if(!(denom>hi)){f64 low=ld<f64>(0x100447B8);if(!(denom<low))return zero;}
 f64 t=numerator/denom,remaining=one-t,twicerem=remaining+remaining,cross=(twicerem*t)*w,tsq=t*t,rsq=gabi::fmadd(remaining,remaining,cross),sum=rsq+tsq,eps=ld<f64>(0x100447C0);
 if(sum>eps)return f32((tsq/sum)*sign);return zero;
}
VERIFY(0x024F6F9C,cp_rational);
struct CpAngles {s16 base,multiplied,difference,maximum;};
s32 cp_lockon_longitude(void* self,f32 t){
 WWHD_FUNC(0x024F748C,s32,self,t);
 u32 a=gabi::ea(self);gabi::Local<CpAngles> local;u32 style=ld<u32>(a,4);f32 low=ld<f32>(style,0x64);gabi::call<void>(0x020066C0,p(local.a),low);
 style=ld<u32>(a,4);f32 high=ld<f32>(style,0x68);gabi::call<void>(0x020066C0,p(local.a+6),high);gabi::call<void>(0x020068B0,p(local.a+6),p(local.a+4),p(local.a));gabi::call<void>(0x0200693C,p(local.a+4),p(local.a+2),t);gabi::call<void>(0x020068CC,p(local.a),p(local.a+2));return ld<s16>(local.a);
}
VERIFY(0x024F748C,cp_lockon_longitude);
s32 cp_lockon_latitude(void* self,f32 t){
 WWHD_FUNC(0x024F7520,s32,self,t);
 u32 a=gabi::ea(self);gabi::Local<CpAngles> local;u32 style=ld<u32>(a,4);f32 low=ld<f32>(style,0x50);gabi::call<void>(0x020066C0,p(local.a),low);
 style=ld<u32>(a,4);f32 high=ld<f32>(style,0x54);gabi::call<void>(0x020066C0,p(local.a+6),high);gabi::call<void>(0x020068B0,p(local.a+6),p(local.a+4),p(local.a));gabi::call<void>(0x0200693C,p(local.a+4),p(local.a+2),t);gabi::call<void>(0x020068CC,p(local.a),p(local.a+2));return ld<s16>(local.a);
}
VERIFY(0x024F7520,cp_lockon_latitude);
f32 cp_zoom(f32 degrees,f32 scale){
 WWHD_FUNC(0x024F7948,f32,degrees,scale);
 gabi::Local<f32> degree;gabi::call<void>(0x02006A3C,degree.get(),degrees);f32 cosine=gabi::call<f32>(0x02006AE8,degree.get());f32 product=gabi::fmuls_ppc(scale,cosine),sine=gabi::call<f32>(0x02006AC4,degree.get()),angle=gabi::call<f32>(0x0201971C,sine,product);f32 half=ld<f32>(0x10044830),pi=ld<f32>(0x1046EE60);return gabi::fmuls_ppc(angle,half/pi);
}
VERIFY(0x024F7948,cp_zoom);
struct CpMatrix {f32 values[12];};struct CpVector {f32 x,y,z;};
void* cp_rotate_x(void* out,void* vector,void* angle){
 WWHD_FUNC(0x024F79BC,void*,out,vector,angle);
 gabi::Local<CpMatrix> matrix;gabi::Local<CpVector> result;s32 value=ld<s16>(gabi::ea(angle));gabi::call<void>(0x025F18EC,matrix.get(),value);gabi::call<void>(0x028E8F64,matrix.get(),vector,result.get());u32 a=gabi::ea(out);if(!a)a=gabi::call<u32>(0x0273AD10,0xC);if(a){st<u32>(a,0,ld<u32>(result.a));st<u32>(a,4,ld<u32>(result.a,4));st<u32>(a,8,ld<u32>(result.a,8));}return p(a);
}
VERIFY(0x024F79BC,cp_rotate_x);
void* cp_rotate_y(void* out,void* vector,void* angle){
 WWHD_FUNC(0x024F7A40,void*,out,vector,angle);
 gabi::Local<CpMatrix> matrix;gabi::Local<CpVector> result;s32 value=ld<s16>(gabi::ea(angle));gabi::call<void>(0x025F1884,matrix.get(),value);gabi::call<void>(0x028E8F64,matrix.get(),vector,result.get());u32 a=gabi::ea(out);if(!a)a=gabi::call<u32>(0x0273AD10,0xC);if(a){st<u32>(a,0,ld<u32>(result.a));st<u32>(a,4,ld<u32>(result.a,4));st<u32>(a,8,ld<u32>(result.a,8));}return p(a);
}
VERIFY(0x024F7A40,cp_rotate_y);
f32 cp_horizontal_distance(void* a,void* b){
 WWHD_FUNC(0x024F7AC4,f32,a,b);
 u32 x=gabi::ea(a),y=gabi::ea(b);f32 bz=ld<f32>(y,8),az=ld<f32>(x,8),z=gabi::fsubs_ppc(az,bz),bx=ld<f32>(y),ax=ld<f32>(x),dx=gabi::fsubs_ppc(ax,bx);f64 zz=f64(z)*z,sum=gabi::fmadd(dx,dx,zz);return gabi::call<f32>(0x028F4384,f32(sum));
}
VERIFY(0x024F7AC4,cp_horizontal_distance);
void* cp_project(void* out,void* angle,void* a,void* b){
 WWHD_FUNC(0x024F7AEC,void*,out,angle,a,b);
 gabi::Local<CpVector> line,rot,back;gabi::Local<s16> ang;gabi::call<void>(0x0201ADE0,b,line.get(),a);gabi::call<void>(0x02006880,angle,ang.get());gabi::call<void>(0x024F7A40,rot.get(),line.get(),ang.get());f32 zero=ld<f32>(0x10044790);st<f32>(rot.a,0,zero);gabi::call<void>(0x02006644,ang.get(),angle);u32 ap=gabi::cpu->r[3];gabi::call<void>(0x024F7A40,back.get(),rot.get(),p(ap));u32 yy=ld<u32>(back.a,4),zz=ld<u32>(back.a,8);st<u32>(line.a,4,yy);st<u32>(line.a,8,zz);st<u32>(line.a,0,ld<u32>(back.a));return gabi::call<void*>(0x0201AD78,a,out,line.get());
}
VERIFY(0x024F7AEC,cp_project);
void cp_startup(){
 WWHD_FUNC(0x024F7BA4,void);
 st<u32>(0x1046EE68,8,0);st<u32>(0x1046EE68,0,0);st<u32>(0x1046EE68,12,0);st<u32>(0x1046EE68,4,0);gabi::call<void>(0x028F026C,p(0x101D540C));f32 lo=ld<f32>(0x10044838),hi=ld<f32>(0x1004483C);st<f32>(0x1046EE5C,0,lo);st<f32>(0x1046EE60,0,hi);gabi::call<void>(0x028ED6F8,p(0x1046EE64));gabi::call<void>(0x028F026C,p(0x101D5418));gabi::call<void>(0x028EAB2C,p(0x1046EE65));gabi::call<void>(0x028F026C,p(0x101D5424));
}
VERIFY(0x024F7BA4,cp_startup);
