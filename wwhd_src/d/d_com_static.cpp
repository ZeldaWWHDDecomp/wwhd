#include "bindings.h"

static void steam_init() { WWHD_FUNC(0x025260D8,void,(u32)0); gabi::store<u8>(0x10475650,0); }
VERIFY(0x025260D8,steam_init);
static void salvage_room(u32 room) { WWHD_FUNC(0x025260E8,void,room);u32 p=gabi::load<u32>(0x10475634);if(p)gabi::call(0x025B4AD8,p,room); }
VERIFY(0x025260E8,salvage_room);
static u32 dai_max() { WWHD_FUNC(0x02526100,u32,(u32)0);return gabi::load<u8>(0x10475655); }
VERIFY(0x02526100,dai_max);
static u32 dai_items() { WWHD_FUNC(0x0252610C,u32,(u32)0);return gabi::load<u8>(0x10475656); }
VERIFY(0x0252610C,dai_items);
static void arrow_set(u32 v) { WWHD_FUNC(0x02526118,void,v);gabi::store<u8>(0x101D5F43,v); }
VERIFY(0x02526118,arrow_set);
static void ib_init() { WWHD_FUNC(0x02526124,void,(u32)0);for(u32 i=0;i<5;i++)gabi::store<u32>(0x1047563C+4*i,~0u); }
VERIFY(0x02526124,ib_init);
static void ib_reg(u32 a) { WWHD_FUNC(0x02526144,void,a);for(u32 i=0;i<5;i++){u32 p=0x1047563C+4*i;if(gabi::load<u32>(p)==~0u){gabi::store<u32>(p,a?gabi::load<u32>(a+4):~0u);return;}} }
VERIFY(0x02526144,ib_reg);
static void ib_remove(u32 a) { WWHD_FUNC(0x02526180,void,a);for(u32 i=0;i<5;i++){u32 p=0x1047563C+4*i;u32 id=a?gabi::load<u32>(a+4):~0u;if(gabi::load<u32>(p)==id){gabi::store<u32>(p,~0u);return;}} }
VERIFY(0x02526180,ib_remove);
static void ib_old() { WWHD_FUNC(0x025261C0,void,(u32)0);u32 id=~0u;for(u32 i=0;i<5;i++){u32 v=gabi::load<u32>(0x1047563C+4*i);if(v==~0u)return;if(v<id)id=v;}gabi::Local<gabi::be<u32>> search;*search=id;u32 a=0;if(id!=~0u)a=gabi::call<u32>(0x025D5218,0x025E1234,search.get());if(a){gabi::call(0x0217BE98,a);gabi::call(0x02526180,a);} }
VERIFY(0x025261C0,ib_old);
static s32 roten_count() { WWHD_FUNC(0x0252624C,s32,(u32)0);s32 n=0;for(u32 i=0;i<12;i++){u32 g=gabi::load<u32>(0x101F84DC);if(gabi::call<s32>(0x025B7840,g+0xB0,i)||i==0)++n;}return n>9?4:n>6?3:n>3?2:1; }
VERIFY(0x0252624C,roten_count);
static void title_on() { WWHD_FUNC(0x02526570,void,(u32)0);gabi::store<u8>(0x101D5F49,1); }
VERIFY(0x02526570,title_on);
static s32 light_lod(s32 a) { WWHD_FUNC(0x02526890,s32,a);u32 frame=gabi::load<u32>(0x101FF558);bool ok=frame!=gabi::load<u32>(0x101D5F34);if(ok)gabi::store<u16>(0x101D5F3C,a);gabi::store<u32>(0x101D5F30,frame);return ok; }
VERIFY(0x02526890,light_lod);
static s32 light_renew() { WWHD_FUNC(0x025268C8,s32,(u32)0);gabi::store<u8>(0x101D5F4A,1);if(gabi::call<s32>(0x02556D14)==1){u32 g=gabi::load<u32>(0x101F84DC);if(gabi::call<s32>(0x025B8B94,g+0x644,0x1C02)==1){gabi::call(0x02526890,(s32)(s16)(gabi::load<s16>(0x101D5F3C)+0x80));return 1;}}return 0; }
VERIFY(0x025268C8,light_renew);
static s32 light_angle() { WWHD_FUNC(0x02526944,s32,(u32)0);return (s16)(gabi::load<s16>(0x101D5F3C)+0xF6C2); }
VERIFY(0x02526944,light_angle);
static s32 light_frrs(s32 a) { WWHD_FUNC(0x0252695C,s32,a);u32 frame=gabi::load<u32>(0x101FF558);bool ok=frame!=gabi::load<u32>(0x101D5F30)&&frame-1==gabi::load<u32>(0x101D5F34);if(ok)gabi::store<u16>(0x101D5F3C,a);gabi::store<u32>(0x101D5F34,frame);return ok; }
VERIFY(0x0252695C,light_frrs);
static void smoke_dtor(u32 p,u32 flags) { WWHD_FUNC(0x02526C80,void,p,flags);if(p&&(flags&1))gabi::call(0x0273AF40,p,flags); }
VERIFY(0x02526C80,smoke_dtor);
static s32 md_timer() { WWHD_FUNC(0x02526C94,s32,(u32)0);return 450; }
VERIFY(0x02526C94,md_timer);
static u32 arrow_get() { WWHD_FUNC(0x02526C9C,u32,(u32)0);return gabi::load<u8>(0x101D5F43); }
VERIFY(0x02526C9C,arrow_get);
static s32 light_diff() { WWHD_FUNC(0x02526CA8,s32,(u32)0);return gabi::load<s16>(0x101D5F3C); }
VERIFY(0x02526CA8,light_diff);
static f32 ship_offset(u32 angle,u32 speed,f64 scale) {
 WWHD_FUNC(0x025269A8,f32,angle,speed,scale);
 s32 a=gabi::load<s16>(angle);gabi::call(0x0200F428,speed,(u32)(a+0x4000)<0x8001?0x180:0x280,0x10,0x300);
 u32 sum=(u16)(gabi::load<s16>(angle)+gabi::load<s16>(speed));gabi::store<u16>(angle,sum);
 return gabi::fmuls_ppc((f32)scale,gabi::load<f32>(0x104A44F8+(sum>>3)*8));
}
VERIFY(0x025269A8,ship_offset);
static void dig_main(u32 p) {
 WWHD_FUNC(0x02526330,void,p);
 u32 g=gabi::load<u32>(0x101F84DC);
 if(gabi::call<s32>(0x025BA0C0,g+0x20,gabi::load<u32>(p+0x3C0),(s32)gabi::load<s8>(p+0x2FE)))return;
 if(!gabi::load<u8>(p+0x3B5)||gabi::load<u8>(p+0x3B4))return;
 gabi::Local<gabi::be<f32>[3]> scale;for(u32 i=0;i<3;i++)gabi::store<f32>(scale.a+4*i,gabi::load<f32>(0x101FFBA8+4*i));
 gabi::Local<gabi::be<s16>[3]> angle;for(u32 i=0;i<3;i++)gabi::store<s16>(angle.a+2*i,gabi::load<s16>(0x101FFB14+2*i));
 if(!gabi::call<s32>(0x025D4604,gabi::load<u32>(p+0x3C4)))return;
 u32 pig=gabi::load<u32>(p+0x3C4);if(!pig||gabi::load<s16>(pig+8)!=220)return;
 bool done=false;
 if(gabi::load<u8>(p+0x3BC)!=255){
  gabi::store<s16>(angle.a+2,(s16)gabi::ftoi(gabi::call<f64>(0x020198D8,gabi::load<f32>(0x1004BCC4))));
  f32 r=gabi::load<f32>(0x1004BCC8);f64 horizontal=gabi::call<f64>(0x020198D8,r);f64 vertical=gabi::call<f64>(0x02019918,r);
  u32 item=gabi::call<u32>(0x025D8AB0,gabi::load<u32>(p+0x3C4)+0x314,gabi::load<u8>(p+0x3BC),(s32)gabi::load<s8>(p+0x326),angle.get(),scale.get(),gabi::load<u32>(p+0x3B8),0,horizontal,(f32)(vertical+gabi::load<f32>(0x1004BCCC)),gabi::load<f32>(0x1004BCD0));
  if(item)gabi::store<u32>(item+0x2E0,gabi::load<u32>(item+0x2E0)|0x4000);
  u32 sw=gabi::load<u32>(p+0x3C0);if(sw!=255){g=gabi::load<u32>(0x101F84DC);gabi::call(0x025B9E38,g+0x20,sw,(s32)gabi::load<s8>(p+0x2FE));}else gabi::call(0x025D57E0,p);
  done=true;
 }
 u32 enemy=gabi::load<u8>(p+0x3BD);if(enemy!=255){gabi::store<u16>(gabi::load<u32>(p+0x3C4)+0x2FC,enemy);g=gabi::load<u32>(0x101F84DC);gabi::call(0x025B9E38,g+0x20,gabi::load<u32>(p+0x3C0),(s32)gabi::load<s8>(p+0x2FE));gabi::call(0x025D57E0,p);done=true;}
 if(done)gabi::store<u8>(p+0x3B4,1);gabi::store<u8>(p+0x3B5,0);
}
VERIFY(0x02526330,dig_main);
static void kb_dig(u32 p,u32 pig) {WWHD_FUNC(0x02526560,void,p,pig);gabi::store<u32>(p+0x3C4,pig);gabi::store<u8>(p+0x3B5,1);gabi::call(0x02526330,p);}
VERIFY(0x02526560,kb_dig);
static void matrix_words(u32 src,u32 dst) {f32 v[12];for(u32 i=0;i<12;i++)v[i]=gabi::load<f32>(src+4*i);for(u32 i=0;i<12;i++)gabi::store<f32>(dst+4*i,v[i]);}
static s32 item_joint(u32 node,s32 timing) {
 WWHD_FUNC(0x02526580,s32,node,timing);if(timing)return 1;
 u32 joint=gabi::call<u32>(0x027F7878,node);u32 model=gabi::load<u32>(0x104B462C);u32 actor=gabi::load<u32>(model+0xB8);u32 n=gabi::load<u16>(joint+4);
 if(!actor||!gabi::call<s32>(0x025D4604,actor)||gabi::load<s16>(actor+8)!=462)return 1;
 const u32 stack=0x1048D0CC;u32 data=gabi::load<u32>(model+0x2C);gabi::store<u16>(data+4,gabi::load<u16>(data+4)|0x10);
 gabi::call(0x028E90D4,gabi::load<u32>(data+0x10)+n*48,stack);
 u32 item=gabi::load<u8>(actor+0x734);
 if(item==0x92)gabi::call(0x025F1BF4,stack,(s32)gabi::load<s16>(actor+0x7D0));
 if(item==0x95){if(n==0)gabi::call(0x028E90D4,stack,actor+0x74C);else if(n==1)gabi::call(0x028E90D4,stack,actor+0x77C);}
 data=gabi::load<u32>(model+0x2C);gabi::store<u16>(data+4,gabi::load<u16>(data+4)|0x10);matrix_words(stack,gabi::load<u32>(data+0x10)+n*48);gabi::call(0x028E90D4,stack,0x104B4868);return 1;
}
VERIFY(0x02526580,item_joint);
static void static_init() {
 WWHD_FUNC(0x02526A4C,void,(u32)0);
 for(u32 i=0;i<4;i++)gabi::store<u32>(0x10475430+4*i,0);
 gabi::call(0x028F026C,0x101D5EE8);
 gabi::store<f32>(0x10475424,gabi::load<f32>(0x1004BCDC));gabi::store<f32>(0x10475428,gabi::load<f32>(0x1004BCE0));
 gabi::call(0x028ED6F8,0x1047542C);gabi::call(0x028F026C,0x101D5EF4);gabi::call(0x028EAB2C,0x1047542D);gabi::call(0x028F026C,0x101D5F00);
 gabi::call(0x025A5C04,0x10475440,0,1,1,1);gabi::call(0x028F026C,0x101D5F0C);
 const u32 src=0x1016E414,dst=0x10475460;
 // The HD initializer materializes the TEV template in this exact load/store sequence.
 f32 f14=gabi::load<f32>(src+0x14);s16 h20=gabi::load<s16>(src+0x20),h1e=gabi::load<s16>(src+0x1E);gabi::store<u16>(dst+0x20,h20);
 f32 f08=gabi::load<f32>(src+8);u8 b1b=gabi::load<u8>(src+0x1B);gabi::store<u16>(dst+0x1E,h1e);
 f32 f10=gabi::load<f32>(src+0x10);gabi::store<f32>(dst+0x14,f14);gabi::store<f32>(dst+0x10,f10);gabi::store<f32>(dst+8,f08);
 f32 f00=gabi::load<f32>(src),f0c=gabi::load<f32>(src+0xC);gabi::store<f32>(dst,f00);gabi::store<f32>(dst+0xC,f0c);
 f32 f24=gabi::load<f32>(src+0x24),f2c=gabi::load<f32>(src+0x2C);u8 b18=gabi::load<u8>(src+0x18),b19=gabi::load<u8>(src+0x19);gabi::store<u8>(dst+0x18,b18);
 s16 h1c=gabi::load<s16>(src+0x1C);f32 f40=gabi::load<f32>(src+0x40);gabi::store<u8>(dst+0x19,b19);s16 h22=gabi::load<s16>(src+0x22);u8 b1a=gabi::load<u8>(src+0x1A);gabi::store<u16>(dst+0x22,h22);gabi::store<f32>(dst+0x24,f24);
 f32 f28=gabi::load<f32>(src+0x28),f30=gabi::load<f32>(src+0x30);gabi::store<f32>(dst+0x2C,f2c);gabi::store<f32>(dst+0x30,f30);gabi::store<f32>(dst+0x28,f28);gabi::store<u16>(dst+0x1C,h1c);gabi::store<u8>(dst+0x1B,b1b);gabi::store<u8>(dst+0xD9,b19);gabi::store<u8>(dst+0xD8,b18);gabi::store<u8>(dst+0xDA,b1a);
 f32 f34=gabi::load<f32>(src+0x34),f04=gabi::load<f32>(src+4);gabi::store<f32>(dst+0x34,f34);gabi::store<f32>(dst+4,f04);f32 f38=gabi::load<f32>(src+0x38);
 gabi::store<u8>(dst+0x1A,b1a);gabi::store<u16>(dst+0xDC,h1c);gabi::store<u16>(dst+0xDE,h1e);gabi::store<u16>(dst+0xE0,h20);gabi::store<u16>(dst+0xE2,h22);
 gabi::store<f32>(dst+0x148,f04);gabi::store<f32>(dst+0x144,f00);gabi::store<f32>(dst+0x14C,f08);gabi::store<f32>(dst+0x150,f0c);gabi::store<f32>(dst+0x154,f10);gabi::store<f32>(dst+0x158,f14);
 f32 f3c=gabi::load<f32>(src+0x3C);gabi::store<f32>(dst+0x38,f38);gabi::store<f32>(dst+0x3C,f3c);gabi::store<u8>(dst+0xDB,b1b);gabi::store<u8>(dst+0x15C,b18);gabi::store<u8>(dst+0x15D,b19);gabi::store<u8>(dst+0x15E,b1a);
 gabi::store<f32>(dst+0x100,f40);gabi::store<f32>(dst+0xD4,f14);gabi::store<f32>(dst+0xFC,f3c);gabi::store<f32>(dst+0xD0,f10);gabi::store<f32>(dst+0xF8,f38);gabi::store<f32>(dst+0xCC,f0c);gabi::store<f32>(dst+0xF4,f34);gabi::store<f32>(dst+0xC8,f08);gabi::store<f32>(dst+0xF0,f30);gabi::store<f32>(dst+0xC4,f04);gabi::store<f32>(dst+0xEC,f2c);gabi::store<f32>(dst+0xC0,f00);gabi::store<f32>(dst+0xE4,f24);gabi::store<f32>(dst+0xE8,f28);
 gabi::store<u8>(dst+0x15F,b1b);gabi::store<u16>(dst+0x160,h1c);gabi::store<u16>(dst+0x162,h1e);gabi::store<u16>(dst+0x164,h20);gabi::store<u16>(dst+0x166,h22);gabi::store<f32>(dst+0x168,f24);gabi::store<f32>(dst+0x16C,f28);gabi::store<f32>(dst+0x170,f2c);gabi::store<f32>(dst+0x174,f30);gabi::store<f32>(dst+0x178,f34);gabi::store<f32>(dst+0x17C,f38);gabi::store<f32>(dst+0x180,f3c);gabi::store<f32>(dst+0x184,f40);gabi::store<f32>(dst+0x40,f40);
}
VERIFY(0x02526A4C,static_init);
