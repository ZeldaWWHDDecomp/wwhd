#include "wwhd.h"
#include "gabi.h"
using gabi::load;
using gabi::store;
static u32 addr(void* p) { return gabi::ea(p); }
void* cDT_NamePTbl_ctor(void* object) {
 WWHD_FUNC(0x0200E7BC, void*, object);
 if (!object) object=gabi::call<void*>(0x0273AD10,12);
 if (object) { u32 a=addr(object);store<u32>(a+4,0);store<u32>(a+8,0x10001E60);store<u32>(a,0); }
 return object;
}
VERIFY(0x0200E7BC,cDT_NamePTbl_ctor);
void cDT_NamePTbl_Set(void* object,u32 count,u32 names) {
 WWHD_FUNC(0x0200E808,void,object,count,names);
 store<u32>(addr(object),count);store<u32>(addr(object)+4,names);
}
VERIFY(0x0200E808,cDT_NamePTbl_Set);
s32 cDT_NamePTbl_GetIndex(void* object,u32 name,u32 index) {
 WWHD_FUNC(0x0200E814,s32,object,name,index);
 u32 count=load<u32>(addr(object));
 if(index<count) {
  u32 names=load<u32>(addr(object)+4);
  while(index<count) {
   u32 other=load<u32>(names+index*4u),i=0;
   u8 left,right;
   do { left=load<u8>(name+i);right=load<u8>(other+i);++i; } while(left==right && left!=0);
   if(left==right) return s32(index);
   ++index;
  }
 }
 return -1;
}
VERIFY(0x0200E814,cDT_NamePTbl_GetIndex);
void* cDT_Format_ctor(void* object) {
 WWHD_FUNC(0x0200E870,void*,object);
 if(!object) object=gabi::call<void*>(0x0273AD10,12);
 if(object) {cDT_NamePTbl_ctor(object);store<u32>(addr(object)+8,0x10001E70);}
 return object;
}
VERIFY(0x0200E870,cDT_Format_ctor);
void* cDT_Name_ctor(void* object) {
 WWHD_FUNC(0x0200E8C4,void*,object);
 if(!object) object=gabi::call<void*>(0x0273AD10,12);
 if(object) {cDT_NamePTbl_ctor(object);store<u32>(addr(object)+8,0x10001E80);}
 return object;
}
VERIFY(0x0200E8C4,cDT_Name_ctor);
void* cDT_DataSrc_ctor(void* object) {
 WWHD_FUNC(0x0200E918,void*,object);
 if(!object) object=gabi::call<void*>(0x0273AD10,16);
 if(object) {u32 a=addr(object);store<u32>(a+8,0);store<u32>(a+12,0x10001E90);store<u32>(a,0);store<u32>(a+4,0);}
 return object;
}
VERIFY(0x0200E918,cDT_DataSrc_ctor);
void cDT_DataSrc_Set(void* object,u32 rows,u32 columns,u32 bytes) {
 WWHD_FUNC(0x0200E968,void,object,rows,columns,bytes);
 u32 a=addr(object);store<u32>(a+4,columns);store<u32>(a+8,bytes);store<u32>(a,rows);
}
VERIFY(0x0200E968,cDT_DataSrc_Set);
u8 cDT_DataSrc_GetInf(void* object,s32 row,s32 column) {
 WWHD_FUNC(0x0200E978,u8,object,row,column);
 u32 a=addr(object);
 if(row<0) return 255;
 u32 rows=load<u32>(a);
 if(u32(row)>=rows || column<0 || u32(column)>=load<u32>(a+4)) return 255;
 return load<u8>(load<u32>(a+8)+u32(row)+u32(column)*rows);
}
VERIFY(0x0200E978,cDT_DataSrc_GetInf);
void* cDT_ctor(void* object) {
 WWHD_FUNC(0x0200E9BC,void*,object);
 if(!object) object=gabi::call<void*>(0x0273AD10,40);
 if(object) {u32 a=addr(object);cDT_Format_ctor(object);cDT_Name_ctor(gabi::at<void>(a+12));cDT_DataSrc_ctor(gabi::at<void>(a+24));}
 return object;
}
VERIFY(0x0200E9BC,cDT_ctor);
void cDT_delete(void* object,s32 flags) {
 WWHD_FUNC(0x0200EA14,void,object,flags);
 if(object && (u32(flags)&1)) gabi::call<void>(0x0273AF40,object);
}
VERIFY(0x0200EA14,cDT_delete);
void cDT_Set(void* object,u32 formatCount,u32 formats,u32 nameCount,u32 names,u32 bytes) {
 WWHD_FUNC(0x0200EA28,void,object,formatCount,formats,nameCount,names,bytes);
 u32 a=addr(object);
 cDT_NamePTbl_Set(object,formatCount,formats);
 cDT_NamePTbl_Set(gabi::at<void>(a+12),nameCount,names);
 cDT_DataSrc_Set(gabi::at<void>(a+24),formatCount,nameCount,bytes);
}
VERIFY(0x0200EA28,cDT_Set);
u8 cDT_GetInf(void* object,s32 row,s32 column) {
 WWHD_FUNC(0x0200EA74,u8,object,row,column);
 return cDT_DataSrc_GetInf(gabi::at<void>(addr(object)+24),row,column);
}
VERIFY(0x0200EA74,cDT_GetInf);
void c_data_tbl_static_init() {
 WWHD_FUNC(0x0200EA7C,void);
 store<u32>(0x101FF578,0);store<u32>(0x101FF570,0);store<u32>(0x101FF57C,0);store<u32>(0x101FF574,0);
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C6FC));
 f32 negativePi=load<f32>(0x10001E54),positivePi=load<f32>(0x10001E58);
 store<f32>(0x101FF564,negativePi);store<f32>(0x101FF568,positivePi);
 gabi::call<void>(0x028ED6F8,gabi::at<void>(0x101FF56C));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C708));
 gabi::call<void>(0x028EAB2C,gabi::at<void>(0x101FF56D));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C714));
}
VERIFY(0x0200EA7C,c_data_tbl_static_init);

/* ---- hosted here: the static initializer(s) of three separate header-static-only TUs linked between c_data_tbl and c_lib (names unknown).
 * Each only initializes the shared header statics (zeroed 16-byte object, -pi/pi pair, two
 * registered global objects); no code of their own. ---- */
void hd_static_init_0200EB10() {
 WWHD_FUNC(0x0200EB10,void);
 gabi::store<u32>(0x101FF594,0);gabi::store<u32>(0x101FF58C,0);gabi::store<u32>(0x101FF598,0);gabi::store<u32>(0x101FF590,0);
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C720));
 f32 negativePi=gabi::load<f32>(0x10001EA0),positivePi=gabi::load<f32>(0x10001EA4);
 gabi::store<f32>(0x101FF580,negativePi);gabi::store<f32>(0x101FF584,positivePi);
 gabi::call<void>(0x028ED6F8,gabi::at<void>(0x101FF588));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C72C));
 gabi::call<void>(0x028EAB2C,gabi::at<void>(0x101FF589));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C738));
}
VERIFY(0x0200EB10,hd_static_init_0200EB10);
void hd_static_init_0200EBA4() {
 WWHD_FUNC(0x0200EBA4,void);
 gabi::store<u32>(0x101FF5B0,0);gabi::store<u32>(0x101FF5A8,0);gabi::store<u32>(0x101FF5B4,0);gabi::store<u32>(0x101FF5AC,0);
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C744));
 f32 negativePi=gabi::load<f32>(0x10001EA8),positivePi=gabi::load<f32>(0x10001EAC);
 gabi::store<f32>(0x101FF59C,negativePi);gabi::store<f32>(0x101FF5A0,positivePi);
 gabi::call<void>(0x028ED6F8,gabi::at<void>(0x101FF5A4));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C750));
 gabi::call<void>(0x028EAB2C,gabi::at<void>(0x101FF5A5));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C75C));
}
VERIFY(0x0200EBA4,hd_static_init_0200EBA4);
void hd_static_init_0200EC38() {
 WWHD_FUNC(0x0200EC38,void);
 gabi::store<u32>(0x101FF5CC,0);gabi::store<u32>(0x101FF5C4,0);gabi::store<u32>(0x101FF5D0,0);gabi::store<u32>(0x101FF5C8,0);
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C768));
 f32 negativePi=gabi::load<f32>(0x10001EB0),positivePi=gabi::load<f32>(0x10001EB4);
 gabi::store<f32>(0x101FF5B8,negativePi);gabi::store<f32>(0x101FF5BC,positivePi);
 gabi::call<void>(0x028ED6F8,gabi::at<void>(0x101FF5C0));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C774));
 gabi::call<void>(0x028EAB2C,gabi::at<void>(0x101FF5C1));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C780));
}
VERIFY(0x0200EC38,hd_static_init_0200EC38);
