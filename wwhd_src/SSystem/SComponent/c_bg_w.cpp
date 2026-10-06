#include "gabi.h"
#include <cmath>
using namespace gabi;
struct BgVec { be<f32> x,y,z; };
struct BgMtx { be<f32> a[12]; };
static u32 word(u32 p,u32 off=0){return load<u32>(p+off);}
static u16 half(u32 p,u32 off=0){return load<u16>(p+off);}
static u8 byte(u32 p,u32 off=0){return load<u8>(p+off);}
static void put(u32 p,u32 off,u32 v){store<u32>(p+off,v);}
static void puth(u32 p,u32 off,u32 v){store<u16>(p+off,(u16)v);}
static void putb(u32 p,u32 off,u32 v){store<u8>(p+off,(u8)v);}
static f32 fl(u32 p,u32 off=0){return load<f32>(p+off);}
static void pf(u32 p,u32 off,f32 v){store<f32>(p+off,v);}
void ASSERT_SOLDHEAP_hd(){WWHD_FUNC(0x02009380,void);u32 heap=call<u32>(0x027EC230);u32 type=call<u32>(0x027EC23C,at<void>(heap));if(type!=0x534C4944)call<void>(0x025F2710,at<void>(0x10001000));}
VERIFY(0x02009380,ASSERT_SOLDHEAP_hd);
void* cBgW_Rwg_ctor_hd(void*p){WWHD_FUNC(0x020093C4,void*,p);u32 a=ea(p);if(!a)a=call<u32>(0x0273AD10,8);if(a){puth(a,0,0);put(a,4,0x100011EC);}return at<void>(a);}
VERIFY(0x020093C4,cBgW_Rwg_ctor_hd);
void* cBgW_Node_ctor_hd(void*p){WWHD_FUNC(0x0200940C,void*,p);u32 a=ea(p);if(!a)a=call<u32>(0x0273AD10,0x20);if(a){put(a,0x1C,0x1000120C);put(a,0x18,0x100011CC);}return at<void>(a);}
VERIFY(0x0200940C,cBgW_Node_ctor_hd);
void* cBgW_Base_ctor_hd(void*p){WWHD_FUNC(0x02009458,void*,p);u32 a=ea(p);if(!a)a=call<u32>(0x0273AD10,8);if(a){put(a,0,0x100);put(a,4,0x1000121C);}return at<void>(a);}
VERIFY(0x02009458,cBgW_Base_ctor_hd);
void* cBgW_ctor_hd(void*p){
 WWHD_FUNC(0x020094A0,void*,p);u32 a=ea(p);if(!a)a=call<u32>(0x0273AD10,0xA8);if(!a)return nullptr;
 call<void>(0x02009458,at<void>(a));put(a,8,0);put(a,4,0x10001308);call<void>(0x028F521C,at<void>(a+0xC),0x30);call<void>(0x028F521C,at<void>(a+0x3C),0x30);
 put(a,0x9C,0);put(a,0x98,0);put(a,0xA4,0);f32 zero=fl(0x1000122C);put(a,0x70,0);pf(a,0x7C,zero);pf(a,0x80,zero);put(a,8,0);put(a,0x8C,0);putb(a,0x75,0);put(a,0x88,0);put(a,0x94,0);put(a,0x78,0);put(a,0x90,0);putb(a,0x74,0);putb(a,0x6D,1);put(a,0xA0,0);putb(a,0x6C,0x20);pf(a,0x84,zero);
 call<void>(0x028E9098,at<void>(a+0xC));call<void>(0x028E9098,at<void>(a+0x3C));put(a,0x78,0);putb(a,0x75,2);put(a,0xA4,0xFFFF);return at<void>(a);
}
VERIFY(0x020094A0,cBgW_ctor_hd);
void cBgW_FreeArea_hd(u32 a){WWHD_FUNC(0x0200959C,void,a);put(a,0x90,0);put(a,0x8C,0);put(a,0x9C,0);put(a,0x98,0);put(a,0xA0,0);put(a,0x88,0);}
VERIFY(0x0200959C,cBgW_FreeArea_hd);
void cBgW_GlobalVtx_hd(u32 a){
 WWHD_FUNC(0x020095BC,void,a);u32 matrix=word(a,8);if(!matrix)return;bool full=byte(a,0x6D)!=0;u32 bg=word(a,0x94);s32 count=(s32)word(bg);
 if(!full){for(u32 i=0;(s32)i<count;i++){u32 dst=word(a,0x90)+i*12;call<void>(0x028E8D88,at<void>(dst),at<void>(a+0x7C),at<void>(dst));bg=word(a,0x94);count=(s32)word(bg);}}
 else{for(u32 i=0;(s32)i<count;i++){u32 src=word(bg,4),dst=word(a,0x90);call<void>(0x028E8F64,at<void>(matrix),at<void>(src+i*12),at<void>(dst+i*12));bg=word(a,0x94);count=(s32)word(bg);if((s32)(i+1)<count)matrix=word(a,8);}}
}
VERIFY(0x020095BC,cBgW_GlobalVtx_hd);
void cBgW_CopyOldMtx_hd(u32 a){WWHD_FUNC(0x020096B0,void,a);if(word(a,8)){call<void>(0x028E90D4,at<void>(a+0x3C),at<void>(a+0xC));call<void>(0x028E90D4,at<void>(word(a,8)),at<void>(a+0x3C));}}
VERIFY(0x020096B0,cBgW_CopyOldMtx_hd);
void cBgW_MakeBlckTransMinMax_hd(u32 a,u32 lo,u32 hi){WWHD_FUNC(0x020096FC,void,a,lo,hi);call<void>(0x028E8D88,at<void>(lo),at<void>(a+0x7C),at<void>(lo));call<void>(0x028E8D88,at<void>(hi),at<void>(a+0x7C),at<void>(hi));}
VERIFY(0x020096FC,cBgW_MakeBlckTransMinMax_hd);
void cBgW_MakeBlckMinMax_hd(u32 a,s32 i,u32 lo,u32 hi){
 WWHD_FUNC(0x02009750,void,a,i,lo,hi);u32 v=word(a,0x90)+(u32)i*12;
 for(u32 o=0;o<12;o+=4){f32 point=fl(v,o),low=fl(lo,o);if(low>point){pf(lo,o,point);point=fl(v,o);}f32 high=fl(hi,o);if(high<point)pf(hi,o,point);}
}
VERIFY(0x02009750,cBgW_MakeBlckMinMax_hd);
void cBgW_MakeBlckBnd_hd(u32 a,u32 block,u32 lo,u32 hi){
 WWHD_FUNC(0x020097D8,void,a,block,lo,hi);u32 bg=word(a,0x94),num=word(bg,0x10),tab=word(bg,0x14)+block*2;u32 start=half(tab);u8 full=byte(a,0x6D);s32 last=block==num-1?(s32)(word(bg,8)-1):(s32)(half(tab,2)-1);
 if(!full){call<void>(0x020096FC,at<void>(a),at<void>(lo),at<void>(hi));return;}
 f32 low=fl(0x10001230),high=fl(0x10001234);pf(lo,0,low);pf(lo,8,low);pf(lo,4,low);pf(hi,8,high);pf(hi,0,high);pf(hi,4,high);
 for(u32 i=start;(s32)i<=last;i++){u32 off=i*10;for(u32 j=0;j<6;j+=2){u32 data=word(a,0x94),tris=word(data,0xC);u32 vertex=half(tris+off,j);call<void>(0x02009750,at<void>(a),vertex,at<void>(lo),at<void>(hi));}}
 f32 margin=fl(0x10001238),lx=fl(lo),ly=fl(lo,4);lx=fsubs_ppc(lx,margin);f32 lz=fl(lo,8);ly=fsubs_ppc(ly,margin);lz=fsubs_ppc(lz,margin);pf(lo,0,lx);pf(lo,4,ly);pf(lo,8,lz);f32 hx=fl(hi),hy=fl(hi,4);hx=fadds_ppc(hx,margin);f32 hz=fl(hi,8);hy=fadds_ppc(hy,margin);hz=fadds_ppc(hz,margin);pf(hi,0,hx);pf(hi,4,hy);pf(hi,8,hz);
}
VERIFY(0x020097D8,cBgW_MakeBlckBnd_hd);
void cBgW_MakeNodeTreeRp_hd(u32 a,u32 i){
 WWHD_FUNC(0x02009930,void,a,i);u32 bg=word(a,0x94),node=word(bg,0x1C)+i*20;
 if(half(node)&1){u32 block=half(node,4);if(block!=0xFFFF){u32 box=word(a,0xA0)+i*28;call<void>(0x020097D8,at<void>(a),block,at<void>(box),at<void>(box+12));}return;}
 call<void>(0x02017DAC,at<void>(word(a,0xA0)+i*28));for(u32 j=0;j<8;j++){u32 child=half(node,4+j*2);if(child==0xFFFF)continue;call<void>(0x02009930,at<void>(a),child);u32 boxes=word(a,0xA0);call<void>(0x02017E74,at<void>(boxes+i*28),at<void>(boxes+child*28));boxes=word(a,0xA0);call<void>(0x02017E74,at<void>(boxes+i*28),at<void>(boxes+child*28+12));}
}
VERIFY(0x02009930,cBgW_MakeNodeTreeRp_hd);
void cBgW_MakeNodeTreeGrpRp_hd(u32 a,u32 i){
 WWHD_FUNC(0x02009A10,void,a,i);u32 off=i*52,boxoff=i*32;u32 group=word(word(a,0x94),0x24)+off;u32 node=half(group,0x2E);
 if(node!=0xFFFF){call<void>(0x02009930,at<void>(a),node);group=word(word(a,0x94),0x24)+off;node=half(group,0x2E);u32 boxes=word(a,0x9C),nodes=word(a,0xA0);call<void>(0x02017DF4,at<void>(boxes+boxoff),at<void>(nodes+node*28));group=word(word(a,0x94),0x24)+off;node=half(group,0x2E);nodes=word(a,0xA0);boxes=word(a,0x9C);call<void>(0x02017E34,at<void>(boxes+boxoff),at<void>(nodes+node*28+12));group=word(word(a,0x94),0x24)+off;}
 for(u32 child=half(group,0x28);child!=0xFFFF;){call<void>(0x02009A10,at<void>(a),child);u32 boxes=word(a,0x9C),childoff=child*32;call<void>(0x02017DF4,at<void>(boxes+boxoff),at<void>(boxes+childoff));boxes=word(a,0x9C);call<void>(0x02017E34,at<void>(boxes+boxoff),at<void>(boxes+childoff+12));group=word(word(a,0x94),0x24)+child*52;child=half(group,0x26);}
}
VERIFY(0x02009A10,cBgW_MakeNodeTreeGrpRp_hd);
void cBgW_MakeNodeTree_hd(u32 a){
 WWHD_FUNC(0x02009B38,void,a);bool has=word(a,0x90)!=0;u32 bg=word(a,0x94);s32 n=(s32)word(bg,0x20);
 if(has){for(u32 i=0;(s32)i<n;i++){call<void>(0x02017DAC,at<void>(word(a,0x9C)+i*32));bg=word(a,0x94);n=(s32)word(bg,0x20);}}
 if(n<=0)return;u32 groups=word(bg,0x24);for(u32 i=0;(s32)i<n;i++){if(half(groups+i*52,0x24)==0xFFFF){put(a,0xA4,i);if(has)call<void>(0x02009A10,at<void>(a),i);return;}}
}
VERIFY(0x02009B38,cBgW_MakeNodeTree_hd);
void cBgW_Move_hd(u32 a){
 WWHD_FUNC(0x02009C84,void,a);u8 flags=byte(a,0x6C);if((flags&0x80)||!(flags&1))return;
 if(!(flags&2)){
 bool full=byte(a,0x74)==255;u32 matrix=0;
 if(!full){matrix=word(a,8);for(u32 off=0;off<48;off+=4){if(off%16==12)continue;if(!(fl(a,0x3C+off)==fl(matrix,off))){full=true;break;}}}
 if(full)putb(a,0x6D,1);
 else{f32 oldx=fl(a,0x48),newx=fl(matrix,12);bool same=oldx==newx;if(same){same=fl(a,0x58)==fl(matrix,28);if(same)same=fl(a,0x68)==fl(matrix,44);}
 if(same){call<void>(0x028E90D4,at<void>(matrix),at<void>(a+12));if(!(word(a,0x78)&8))return;}
 else{f32 dx=fsubs_ppc(newx,oldx);matrix=word(a,8);f32 oy=fl(a,0x58);pf(a,0x7C,dx);f32 ny=fl(matrix,28),dy=fsubs_ppc(ny,oy),oz=fl(a,0x68);pf(a,0x80,dy);f32 nz=fl(matrix,44),dz=fsubs_ppc(nz,oz);putb(a,0x6D,0);pf(a,0x84,dz);}}
 u8 count=byte(a,0x74);putb(a,0x74,count==255?0:count+1);call<void>(0x020095BC,at<void>(a));}
 call<void>(0x020096B0,at<void>(a));u32 target=word(word(a,4),0x1C);call_ptr<void>(target,at<void>(a));target=word(word(a,4),0x24);call_ptr<void>(target,at<void>(a));call<void>(0x02009B38,at<void>(a));
}
VERIFY(0x02009C84,cBgW_Move_hd);
u8 cBgW_ChkMemoryError_hd(u32 a){WWHD_FUNC(0x02009E58,u8,a);return !word(a,0x88)||!word(a,0x8C)||!word(a,0x98)||!word(a,0xA0)||!word(a,0x9C);}
VERIFY(0x02009E58,cBgW_ChkMemoryError_hd);
u8 cBgW_SetVtx_hd(u32 a){
 WWHD_FUNC(0x02009EA4,u8,a);call<void>(0x02009380);u8 flags=byte(a,0x6C);if(flags&0x10){put(a,0x90,0);return 0;}u32 bg=word(a,0x94);if(!(flags&1)){put(a,0x90,word(bg,4));return 0;}u32 dst=call<u32>(0x0273ADAC,word(bg)*12);put(a,0x90,dst);if(!dst)return 1;
 if(byte(a,0x6C)&0x40){s32 count=(s32)word(word(a,0x94));f32 zero=fl(0x1000122C);for(u32 i=0;(s32)i<count;i++){pf(dst,i*12,zero);dst=word(a,0x90);pf(dst,i*12+4,zero);dst=word(a,0x90);pf(dst,i*12+8,zero);count=(s32)word(word(a,0x94));if((s32)(i+1)<count)dst=word(a,0x90);}}
 call<void>(0x020095BC,at<void>(a));return 0;
}
VERIFY(0x02009EA4,cBgW_SetVtx_hd);
u8 cBgW_SetTri_hd(u32 a){WWHD_FUNC(0x02009FAC,u8,a);call<void>(0x02009380);u32 bg=word(a,0x94),dst=call<u32>(0x028EFFD0,0,word(bg,8),0x18,at<void>(0x0200B56C));put(a,0x88,dst);if(!dst)return 1;u32 target=word(word(a,4),0x1C);call_ptr<void>(target,at<void>(a));return 0;}
VERIFY(0x02009FAC,cBgW_SetTri_hd);
u8 cBgW_Set_hd(u32 a,u32 bg,u32 flags,u32 matrix){
 WWHD_FUNC(0x0200A030,u8,a,bg,flags,matrix);call<void>(0x02009380);f32 range=fl(0x1000123C);putb(a,0x6C,0x20);for(u32 off:{0x90u,0x88u,0x8Cu,0x98u,0xA0u,0x9Cu})put(a,off,0);f32 random=call<f32>(0x020198D8,range);putb(a,0x74,(u32)ftoi(random));if(!bg)return 1;
 putb(a,0x6C,flags);if(flags&0x20){put(a,8,0);call<void>(0x028E9098,at<void>(a+12));call<void>(0x028E9098,at<void>(a+0x3C));}
 else{put(a,8,matrix);call<void>(0x028E90D4,at<void>(matrix),at<void>(a+12));call<void>(0x028E90D4,at<void>(word(a,8)),at<void>(a+0x3C));}
 put(a,0x94,bg);bool error=call<s32>(0x02009EA4,at<void>(a))!=0;
 if(!error)error=call<s32>(0x02009FAC,at<void>(a))!=0;
 if(!error){u32 n=word(word(a,0x94),8),dst=call<u32>(0x028EFFD0,0,n,8,at<void>(0x020093C4));put(a,0x8C,dst);error=!dst;}
 if(!error){u32 n=word(word(a,0x94),0x10),dst=call<u32>(0x0273ADAC,n*6);put(a,0x98,dst);error=!dst;}
 if(!error){u32 n=word(word(a,0x94),0x18),dst=call<u32>(0x028EFFD0,0,n,0x1C,at<void>(0x0200B5C0));put(a,0xA0,dst);error=!dst;}
 if(!error){u32 n=word(word(a,0x94),0x20),dst=call<u32>(0x028EFFD0,0,n,0x20,at<void>(0x0200940C));put(a,0x9C,dst);error=!dst;}
 if(error){call<void>(0x0200959C,at<void>(a));return 1;}u32 target=word(word(a,4),0x24);call_ptr<void>(target,at<void>(a));putb(a,0x6D,1);call<void>(0x02009B38,at<void>(a));return 0;
}
VERIFY(0x0200A030,cBgW_Set_hd);
static void copy_vec(u32 d,u32 s){for(u32 o=0;o<12;o+=4)put(d,o,word(s,o));}
void cBgW_GetTriPnt_hd(u32 a,u32 i,u32 p0,u32 p1,u32 p2){
 WWHD_FUNC(0x0200A220,void,a,i,p0,p1,p2);u32 bg=word(a,0x94);if(!bg){call<void>(0x0273AA24,at<void>(0x10001258),0xCE0,at<void>(0x10001248));bg=word(a,0x94);}u32 tri=word(bg,12)+i*10;u32 i0=half(tri),vertices=word(a,0x90);copy_vec(p0,vertices+i0*12);u32 i1=half(tri,2);vertices=word(a,0x90);copy_vec(p1,vertices+i1*12);u32 i2=half(tri,4);vertices=word(a,0x90);copy_vec(p2,vertices+i2*12);
}
VERIFY(0x0200A220,cBgW_GetTriPnt_hd);
u32 cBgW_GetRoomId_hd(u32 a,s32 i){WWHD_FUNC(0x0200A300,u32,a,i);u32 bg=0;if(i<0){call<void>(0x0273AA24,at<void>(0x10001294),0xCA4,at<void>(0x10001264));bg=word(a,0x94);}else{bg=word(a,0x94);if(i>=(s32)word(bg,0x20)){call<void>(0x0273AA24,at<void>(0x10001294),0xCA4,at<void>(0x10001264));bg=word(a,0x94);}}u32 groups=word(bg,0x24),parent=half(groups+(u32)i*52,0x24);if(parent==0xFFFF)return 0xFFFF;parent=half(groups+parent*52,0x24);if(parent==0xFFFF)return 0xFFFF;return half(groups+parent*52,0x2A);}
VERIFY(0x0200A300,cBgW_GetRoomId_hd);
void cBgW_GetTrans_hd(u32 a,u32 d){WWHD_FUNC(0x0200A3BC,void,a,d);u32 matrix=word(a,8);f32 old=fl(a,0x18),now=fl(matrix,12);pf(d,0,fsubs_ppc(now,old));matrix=word(a,8);now=fl(matrix,28);old=fl(a,0x28);pf(d,4,fsubs_ppc(now,old));matrix=word(a,8);now=fl(matrix,44);old=fl(a,0x38);pf(d,8,fsubs_ppc(now,old));}
VERIFY(0x0200A3BC,cBgW_GetTrans_hd);
void cBgW_GetTopUnder_hd(u32 a,u32 top,u32 under){WWHD_FUNC(0x0200A3FC,void,a,top,under);u32 idx=word(a,0xA4),boxes=word(a,0x9C);pf(under,0,fl(boxes+idx*32,4));idx=word(a,0xA4);boxes=word(a,0x9C);pf(top,0,fl(boxes+idx*32,0x10));}
VERIFY(0x0200A3FC,cBgW_GetTopUnder_hd);
void cBgW_CalcPlane_hd(u32 a){
 WWHD_FUNC(0x0200A430,void,a);u32 vertices=word(a,0x90),bg=word(a,0x94),tris=word(bg,12);if(!vertices)return;bool full=byte(a,0x6D)!=0;s32 count=(s32)word(bg,8);
 if(!full){for(u32 i=0;(s32)i<count;i++){u32 plane=word(a,0x88)+i*24;f32 dot=call<f32>(0x028E8F44,at<void>(plane),at<void>(a+0x7C));pf(plane,12,fsubs_ppc(fl(plane,12),dot));count=(s32)word(word(a,0x94),8);}}
 else{for(u32 i=0;(s32)i<count;i++){u32 tri=tris+i*10;u32 i0=half(tri),planes=word(a,0x88),i1=half(tri,2),i2=half(tri,4),plane=planes+i*24;call<void>(0x02010D18,at<void>(vertices+i0*12),at<void>(vertices+i1*12),at<void>(vertices+i2*12),at<void>(plane),at<void>(plane+12));count=(s32)word(word(a,0x94),8);if((s32)(i+1)<count)vertices=word(a,0x90);}}
}
VERIFY(0x0200A430,cBgW_CalcPlane_hd);
void cBgW_BlckConnect_hd(u32 a,u32 head,u32 prev,u32 i){WWHD_FUNC(0x0200A564,void,a,head,prev,i);if(half(head)==0xFFFF)puth(head,0,i);u32 p=word(prev);if(p!=0xFFFF)puth(word(a,0x8C)+p*8,0,i);put(prev,0,i);puth(word(a,0x8C)+i*8,0,0xFFFF);}
VERIFY(0x0200A564,cBgW_BlckConnect_hd);
void cBgW_ClassifyPlane_hd(u32 a){
 WWHD_FUNC(0x0200A5A8,void,a);if(!word(a,0x90))return;u32 bg=word(a,0x94);s32 count=(s32)word(bg,0x10);if(count<=0)return;f32 roof=fl(0x100012A4),ground=fl(0x100012A0),epsilon=fl(0x100030B8);
 for(u32 block=0;(s32)block<count;block++){
 u32 starttable=word(bg,0x14)+block*2,start=half(starttable);s32 last=block==(u32)count-1?(s32)(word(bg,8)-1):(s32)(half(starttable,2)-1);
 puth(word(a,0x98)+block*6,0,0xFFFF);puth(word(a,0x98)+block*6,2,0xFFFF);puth(word(a,0x98)+block*6,4,0xFFFF);
 Local<be<s32>> prevRoof,prevWall,prevGround;*prevGround=0xFFFF;*prevRoof=0xFFFF;*prevWall=0xFFFF;
 for(u32 i=start;(s32)i<=last;i++){u32 plane=word(a,0x88)+i*24;f32 nx=fl(plane),ny=fl(plane,4);bool tiny=std::fabs(nx)<epsilon;if(tiny){tiny=std::fabs(fl(plane,4))<epsilon;if(tiny)tiny=std::fabs(fl(plane,8))<epsilon;}if(tiny)continue;u32 ignore=word(a,0x78),kind,previous;
 if(!(ny<ground)){if(ignore&1)continue;kind=4;previous=prevGround.a;}
 else if(ny<roof){if(ignore&4)continue;kind=0;previous=prevRoof.a;}
 else{if(ignore&2)continue;kind=2;previous=prevWall.a;}
 u32 head=word(a,0x98)+block*6+kind;call<void>(0x0200A564,at<void>(a),at<void>(head),at<void>(previous),i);
 }
 bg=word(a,0x94);count=(s32)word(bg,0x10);
 }
}
VERIFY(0x0200A5A8,cBgW_ClassifyPlane_hd);
struct BgTri { u8 bytes[0x38]; };
u8 cBgW_RwgLineCheck_hd(u32 a,s32 index,u32 chk){
 WWHD_FUNC(0x0200A78C,u8,a,index,chk);Local<BgTri> triangle;Local<BgVec> point;call<void>(0x02019040,triangle.get());u32 found=0;
 do{u32 tri=word(word(a,0x94),12)+(u32)index*10;u32 i1=half(tri,2),i0=half(tri),i2=half(tri,4),planes=word(a,0x88),vertices=word(a,0x90);
 call<void>(0x020192AC,triangle.get(),at<void>(vertices+i0*12),at<void>(vertices+i1*12),at<void>(vertices+i2*12),at<void>(planes+(u32)index*24));
 u32 front=byte(chk,0x53),back=byte(chk,0x54);s32 hit=call<s32>(0x02012AE8,at<void>(chk+0x24),triangle.get(),point.get(),front,back);
 if(hit){u32 target=word(word(a,4),0x2C),pass=word(chk);s32 through=call_ptr<s32>(target,at<void>(a),index,at<void>(pass));if(!through){call<void>(0x02018870,at<void>(chk+0x24),point.get());if(index<0)call<void>(0x0273AA24,at<void>(0x100012B8),0x7B,at<void>(0x100012A8));puth(chk,0x14,(u32)index);found=1;}}
 index=half(word(a,0x8C)+(u32)index*8);
 }while(index!=0xFFFF);return (u8)found;
}
VERIFY(0x0200A78C,cBgW_RwgLineCheck_hd);
u8 cBgW_LineCheckRp_hd(u32 a,u32 chk,u32 i){
 WWHD_FUNC(0x0200A8BC,u8,a,chk,i);u32 box=word(a,0xA0)+i*28;if(!call<s32>(0x02010FFC,at<void>(box),at<void>(box+12),at<void>(chk+0x24),at<void>(chk+0x30)))return 0;u32 node=word(word(a,0x94),0x1C)+i*20;u32 found=0;
 if(half(node)&1){for(u32 kind=0;kind<3;kind++){if(!byte(chk,0x50+kind))continue;u32 block=half(node,4),head=word(a,0x98)+block*6+(kind==0?2:kind==1?4:0),index=half(head);if(index!=0xFFFF && call<s32>(0x0200A78C,at<void>(a),index,at<void>(chk)))found=1;}}
 else{for(u32 child=0;child<8;child++){u32 index=half(node,4+child*2);if(index!=0xFFFF && call<s32>(0x0200A8BC,at<void>(a),at<void>(chk),index))found=1;}}return (u8)found;
}
VERIFY(0x0200A8BC,cBgW_LineCheckRp_hd);
u8 cBgW_LineCheckGrpRp_hd(u32 a,u32 chk,u32 i,u32 depth){
 WWHD_FUNC(0x0200AB4C,u8,a,chk,i,depth);u32 target=word(word(a,4),0x3C),pass=word(chk,4);if(call_ptr<s32>(target,at<void>(a),i,at<void>(pass),depth))return 0;u32 box=word(a,0x9C)+i*32;if(!call<s32>(0x02010FFC,at<void>(box),at<void>(box+12),at<void>(chk+0x24),at<void>(chk+0x30)))return 0;
 u32 group=word(word(a,0x94),0x24)+i*52,node=half(group,0x2E),found=0;if(node!=0xFFFF){if(call<s32>(0x0200A8BC,at<void>(a),at<void>(chk),node))found=1;group=word(word(a,0x94),0x24)+i*52;}
 depth++;for(u32 child=half(group,0x28);child!=0xFFFF;){if(call<s32>(0x0200AB4C,at<void>(a),at<void>(chk),child,depth))found=1;group=word(word(a,0x94),0x24)+child*52;child=half(group,0x26);}return (u8)found;
}
VERIFY(0x0200AB4C,cBgW_LineCheckGrpRp_hd);
u8 cBgW_RwgGroundCheckCommon_hd(u32 a,s32 i,u32 c,f32 h){WWHD_FUNC(0x0200AC70,u8,a,i,c,h);if(!(h<fl(c,0x28))||!(h>fl(c,0x34)))return 0;u32 t=word(word(a,0x94),12)+(u32)i*10,v=word(a,0x90);if(!call<s32>(0x02012504,at<void>(v+half(t)*12),at<void>(v+half(t,2)*12),at<void>(v+half(t,4)*12),at<void>(c+0x24)))return 0;if(call_ptr<s32>(word(word(a,4),0x2C),at<void>(a),i,at<void>(word(c))))return 0;pf(c,0x34,h);if(i<0)call<void>(0x0273AA24,at<void>(0x100012DC),0x7B,at<void>(0x100012CC));puth(c,0x14,i);return 1;}
VERIFY(0x0200AC70,cBgW_RwgGroundCheckCommon_hd);
static u8 ground_list(u32 a,u32 i,u32 c,bool wall){u32 found=0;f32 threshold=fl(0x100012F0);do{u32 p=word(a,0x88)+i*24,r=word(a,0x8C)+i*8;f32 ny=fl(p,4);if(!wall||!(ny<threshold)){f32 term=-fmuls_ppc(fl(p),fl(c,0x24));f32 sum=(f32)(-((f64)fl(p,8)*(f64)fl(c,0x2C)-(f64)term));f32 h=fsubs_ppc(sum,fl(p,12))/ny;if(call<s32>(0x0200AC70,at<void>(a),i,at<void>(c),h))found=1;}i=half(r);}while(i!=0xFFFF);return found;}
u8 cBgW_RwgGroundCheckGnd_hd(u32 a,u32 i,u32 c){WWHD_FUNC(0x0200ADA4,u8,a,i,c);return ground_list(a,i,c,false);}
VERIFY(0x0200ADA4,cBgW_RwgGroundCheckGnd_hd);
u8 cBgW_RwgGroundCheckWall_hd(u32 a,u32 i,u32 c){WWHD_FUNC(0x0200AE54,u8,a,i,c);return ground_list(a,i,c,true);}
VERIFY(0x0200AE54,cBgW_RwgGroundCheckWall_hd);
static bool ground_box(u32 box,u32 c){if(!call<s32>(0x02017D3C,at<void>(box),at<void>(c+0x24)))return false;if(!call<s32>(0x02017D84,at<void>(box),fl(c,0x28)))return false;return !call<s32>(0x02017D98,at<void>(box),fl(c,0x34));}
u8 cBgW_GroundCrossRp_hd(u32 a,u32 c,u32 i){WWHD_FUNC(0x0200AF30,u8,a,c,i);u32 n=word(word(a,0x94),0x1C)+i*20,found=0;if(half(n)&1){if(word(c,0x3C)){u32 h=half(word(a,0x98)+half(n,4)*6,4);if(h!=0xFFFF&&call<s32>(0x0200ADA4,at<void>(a),h,at<void>(c)))found=1;}if(word(c,0x38)){u32 h=half(word(a,0x98)+half(n,4)*6,2);if(h!=0xFFFF&&call<s32>(0x0200AE54,at<void>(a),h,at<void>(c)))found=1;}}else{for(u32 j=0;j<8;j++){static const u32 offsets[8]={8,10,16,18,4,6,12,14};u32 off=offsets[j],child=half(n,off);if(child==0xFFFF)continue;u32 b=word(a,0xA0)+child*28;if(ground_box(b,c)&&call<s32>(0x0200AF30,at<void>(a),at<void>(c),half(n,off)))found=1;}}return found;}
VERIFY(0x0200AF30,cBgW_GroundCrossRp_hd);
u8 cBgW_GroundCrossGrpRp_hd(u32 a,u32 c,u32 i,u32 depth){WWHD_FUNC(0x0200B380,u8,a,c,i,depth);if(call_ptr<s32>(word(word(a,4),0x3C),at<void>(a),i,at<void>(word(c,4)),depth))return 0;if(!ground_box(word(a,0x9C)+i*32,c))return 0;u32 g=word(word(a,0x94),0x24)+i*52,n=half(g,0x2E),found=0;if(n!=0xFFFF){if(call<s32>(0x0200AF30,at<void>(a),at<void>(c),n))found=1;g=word(word(a,0x94),0x24)+i*52;}depth++;for(u32 child=half(g,0x28);child!=0xFFFF;){if(call<s32>(0x0200B380,at<void>(a),at<void>(c),child,depth))found=1;child=half(word(word(a,0x94),0x24)+child*52,0x26);}return found;}
VERIFY(0x0200B380,cBgW_GroundCrossGrpRp_hd);
u8 cBgW_ChkPolyThrough_hd(){WWHD_FUNC(0x0200B4C8,u8);return 0;}
VERIFY(0x0200B4C8,cBgW_ChkPolyThrough_hd);
u8 cBgW_ChkGrpThrough_hd(){WWHD_FUNC(0x0200B4D0,u8);return 0;}
VERIFY(0x0200B4D0,cBgW_ChkGrpThrough_hd);
void cBgW_StaticInit_hd(){WWHD_FUNC(0x0200B4D8,void);for(u32 o:{8u,0u,12u,4u})put(0x101FF4D8,o,0);call<void>(0x028F026C,at<void>(0x1018C66C));pf(0x101FF4CC,0,fl(0x10001300));pf(0x101FF4D0,0,fl(0x10001304));call<void>(0x028ED6F8,at<void>(0x101FF4D4));call<void>(0x028F026C,at<void>(0x1018C678));call<void>(0x028EAB2C,at<void>(0x101FF4D5));call<void>(0x028F026C,at<void>(0x1018C684));}
VERIFY(0x0200B4D8,cBgW_StaticInit_hd);
