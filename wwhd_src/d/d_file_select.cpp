#include "gabi.h"
#include <bit>
using namespace gabi;
// Bounded HD save-data selection screen (0x2D8, vtable 101026C0).
// Native layout/state/resource attribution; no claim to preserve all GC dFile_select APIs.
// Pure native lfs/stfs transfers preserve representation; keep float store footprints.
static f32 fsTransfer(u32 p) {return load<f32>(p);}
static f32 fsRawTransfer(u32 p) {return std::bit_cast<f32>(load<u32>(p));}
static u32 fsPane(u32 p,u32 i) {return load<u32>(load<u32>(p+0x4C)+(i<load<u32>(p+0x48)?4*i:0));}
static u32 fsAnim(u32 p) {return load<u32>(load<u32>(p+0x44)+0xD4);}
static void fsVcall(u32 p,u32 off) {call_ptr<void>(load<u32>(load<u32>(p+4)+off),p);}
static void fsDraw(u32 p) {fsVcall(p,0xB4);fsVcall(p,0x8C);}
void fsDtor(u32 p,u32 flags) {WWHD_FUNC(0x026CBEE0,void,p,flags);if(p){store<u32>(p+0x2C,0x101027B8);store<u32>(p+0x30,0x101027C8);call<void>(0x026F88F0,p,0u);if(flags&1)call<void>(0x0273AF40,p);}}
VERIFY(0x026CBEE0,fsDtor);
u32 fsSoundReset() {WWHD_FUNC(0x026CBF4C,u32);return call<u32>(0x02615154,load<u32>(0x101F4FF0),0u);}
VERIFY(0x026CBF4C,fsSoundReset);
void fsOpenInit(u32 p) {WWHD_FUNC(0x026CBF5C,void,p);const u32 screen=load<u32>(p+0x44);store<u32>(p+0x1D8,3);store<u32>(p+0x1D0,6);call<void>(0x02005674,load<u32>(screen+0xD4),0u,0u);fsVcall(p,0x8C);store<u32>(p+0x27C,1);call<void>(0x026CBF4C,p);}
VERIFY(0x026CBF5C,fsOpenInit);
void fsTransitionInit(u32 p,u32 target,u32 start,u32 end,u32 a,u32 b,u32 c,f32 speed) {WWHD_FUNC(0x026CC1CC,void,p,target,start,end,a,b,c,speed);const f32 ey=fsRawTransfer(end+4),sx=fsRawTransfer(start);store<f32>(p+0x10,ey);store<f32>(p,sx);const f32 ex=fsRawTransfer(end),sy=fsRawTransfer(start+4),ez=fsRawTransfer(end+8);store<f32>(p+4,sy);const f32 factor=load<f32>(0x101021AC);store<u16>(p+0x24,c);store<f32>(p+0xC,ex);store<u32>(p+0x2C,0);store<u16>(p+0x20,a);store<f32>(p+0x1C,fmuls_ppc(speed,factor));store<u16>(p+0x22,b);store<f32>(p+0x14,ez);store<f32>(p+0x18,speed);const f32 sz=fsRawTransfer(start+8);store<u32>(p+0x28,target);store<f32>(p+8,sz);}
VERIFY(0x026CC1CC,fsTransitionInit);
void fsGrowMove(u32 p) {WWHD_FUNC(0x026CCE60,void,p);if(call<u32>(0x026CCC94,p))call<void>(0x020063C0,p+0x18,0x1049A5E4u);fsDraw(p);}
VERIFY(0x026CCE60,fsGrowMove);
u32 fsSlotIdle(u32 p,u32 slot) {WWHD_FUNC(0x026CCECC,u32,p,slot);call<void>(0x02726FC4,load<u32>(p+0x2D4));const u32 q=call<u32>(0x027271F0,load<u32>(p+0x2D4),slot);return q&&load<u16>(q+0x24)==0;}
VERIFY(0x026CCECC,fsSlotIdle);
u32 fsSelectDone(u32 p) {WWHD_FUNC(0x026CD628,u32,p);if(load<u8>(p+0x278))return 0;if(!call<u32>(0x026CCECC,p,u32(load<u8>(p+0x283))))return 0;store<u8>(p+0x1CD,1);return 1;}
VERIFY(0x026CD628,fsSelectDone);
void fsWaitDraw(u32 p) {WWHD_FUNC(0x026CD898,void,p);fsDraw(p);}
VERIFY(0x026CD898,fsWaitDraw);
u32 fsSlotFlag(u32 p,u32 slot) {WWHD_FUNC(0x026CDA70,u32,p,slot);call<void>(0x02726FC4,load<u32>(p+0x2D4));const u32 q=call<u32>(0x027271F0,load<u32>(p+0x2D4),slot);return q&&load<u8>(q+0x26)!=0;}
VERIFY(0x026CDA70,fsSlotFlag);
u32 fsButtonOne(u32 p) {WWHD_FUNC(0x026CE368,u32,p);return load<u32>(fsPane(p,5)+0x78)==1;}
VERIFY(0x026CE368,fsButtonOne);
void fsInInit(u32 p) {WWHD_FUNC(0x026CEA60,void,p);call<void>(0x026CE728,p);store<u32>(p+0x27C,1);call<void>(0x026CE728,p);const u32 q=fsPane(p,load<u32>(p+0x1D8));const u32 sound=load<u32>(0x101F4FF0);call<void>(0x02615154,sound,load<u32>(q+0x50));store<u8>(p+0x284,0);}
VERIFY(0x026CEA60,fsInInit);
void fsCloseFlag(u32 p) {WWHD_FUNC(0x026CEF90,void,p);store<u8>(p+0x1CC,1);}
VERIFY(0x026CEF90,fsCloseFlag);
void fsOutInit(u32 p) {WWHD_FUNC(0x026CEF9C,void,p);const f32 rate=load<f32>(0x101021A8);call<void>(0x020053E4,fsAnim(p),6u,0u,rate);call<void>(0x026CBF4C,p);call<void>(0x026CBFCC,p);}
VERIFY(0x026CEF9C,fsOutInit);
void fsOutMove(u32 p) {WWHD_FUNC(0x026CEFF0,void,p);if(call<u32>(0x02005840,fsAnim(p),6u))call<void>(0x020063C0,p+0x18,0x1049A6ECu);fsDraw(p);}
VERIFY(0x026CEFF0,fsOutMove);
void fsDeleteChoiceInit(u32 p) {WWHD_FUNC(0x026CF144,void,p);call<void>(0x026CF068,p);store<u32>(p+0x27C,2);call<void>(0x026CDFE0,p);const u32 count=load<u32>(p+0x48);store<u32>(p+0x270,0);u32 array=load<u32>(p+0x4C);store<u8>(p+0x284,0);if(count>5)array+=20;const u32 q=load<u32>(array);call_ptr<void>(load<u32>(load<u32>(q+4)+0x1F4),q,0u);}
VERIFY(0x026CF144,fsDeleteChoiceInit);
u32 fsCopySound(u32 p) {WWHD_FUNC(0x026CF1B8,u32,p);const u32 q=fsPane(p,2),sound=load<u32>(0x101F4FF0);return call<u32>(0x02615154,sound,load<u32>(q+0x50));}
VERIFY(0x026CF1B8,fsCopySound);
u32 fsCopyAnim(u32 p) {WWHD_FUNC(0x026CF3EC,u32,p);const f32 f=load<f32>(0x101021A8);return call<u32>(0x020053E4,fsAnim(p),2u,1u,f);}
VERIFY(0x026CF3EC,fsCopyAnim);
void fsCopyInit(u32 p) {WWHD_FUNC(0x026CF9B4,void,p);const f32 f=load<f32>(0x101021A8);call<void>(0x020053E4,fsAnim(p),2u,1u,f);call<void>(0x026CF838,p);store<u32>(p+0x27C,5);store<u32>(p+0x270,0);call<void>(0x026CBFCC,p);call<void>(0x026CBF4C,p);store<u8>(p+0x284,0);}
VERIFY(0x026CF9B4,fsCopyInit);
void fsDeleteInit(u32 p) {WWHD_FUNC(0x026D0420,void,p);call<void>(0x02005674,fsAnim(p),5u,3u);call<void>(0x026CBF4C,p);call<void>(0x026CBFCC,p);call<void>(0x026CDFE0,p);call<void>(0x026D0130,p);store<u32>(p+0x27C,6);store<u32>(p+0x270,0);store<u8>(p+0x284,0);}
VERIFY(0x026D0420,fsDeleteInit);
u32 fsDeleteAnim(u32 p) {WWHD_FUNC(0x026D0848,u32,p);const f32 f=load<f32>(0x101021A8);return call<u32>(0x020053E4,fsAnim(p),2u,1u,f);}
VERIFY(0x026D0848,fsDeleteAnim);
u32 fsBackSound(u32 p) {WWHD_FUNC(0x026D0F94,u32,p);const u32 q=fsPane(p,3),sound=load<u32>(0x101F4FF0);return call<u32>(0x02615154,sound,load<u32>(q+0x50));}
VERIFY(0x026D0F94,fsBackSound);
void fsBackInit(u32 p) {WWHD_FUNC(0x026D0FBC,void,p);store<u32>(p+0x27C,10);store<u8>(p+0x284,0);call<void>(0x026CF068,p);call<void>(0x026D0F94,p);store<u32>(p+0x270,0);}
VERIFY(0x026D0FBC,fsBackInit);
void fsEndInit() {WWHD_FUNC(0x026D15A8,void);store<u8>(0x1047B07C,1);}
VERIFY(0x026D15A8,fsEndInit);
void fsEndMove(u32 p) {WWHD_FUNC(0x026D15B8,void,p);const u32 pad=load<u32>(0x101F5088);if((load<u32>(pad+0x18)|load<u32>(pad+0x20))&1){store<u8>(p+0x1CC,1);call<void>(0x025E1988,0x84Au);}fsDraw(p);}
VERIFY(0x026D15B8,fsEndMove);
void fsTransitionClear(u32 p) {WWHD_FUNC(0x026D1AF0,void,p);const f32 z=load<f32>(0x101021CC);store<u16>(p+0x22,255);store<f32>(p+0x14,z);store<u32>(p+0x28,0);store<f32>(p+0xC,z);store<f32>(p+0x10,z);store<f32>(p+4,z);store<u32>(p+0x2C,0);store<u16>(p+0x20,255);store<f32>(p+8,z);store<u16>(p+0x24,255);store<f32>(p+0x18,z);store<f32>(p,z);}
VERIFY(0x026D1AF0,fsTransitionClear);
void fsDelete(u32 p) {WWHD_FUNC(0x026D20D8,void,p);call<void>(0x02615154,load<u32>(0x101F4FF0),0u);fsVcall(p,0xAC);fsVcall(p,0x84);call<void>(0x026F91F8,p);}
VERIFY(0x026D20D8,fsDelete);
u32 fsMove(u32 p) {WWHD_FUNC(0x026D2140,u32,p);return call<u32>(0x02006364,p+0x18);}
VERIFY(0x026D2140,fsMove);
void fsReset(u32 p) {WWHD_FUNC(0x026D23DC,void,p);store<u8>(p+0x1CC,0);store<u8>(p+0x1CD,0);call<void>(0x026CD420,p);call<void>(0x020063C0,p+0x18,0x1049A63Cu);}
VERIFY(0x026D23DC,fsReset);
u32 fsSlotStatus(u32 p) {WWHD_FUNC(0x026D2424,u32,p);const u32 index=load<u32>(p+0x280)+6;return call<u32>(0x02644008,fsPane(p,index));}
VERIFY(0x026D2424,fsSlotStatus);
u32 fsDeleteSound(u32 p) {WWHD_FUNC(0x026D244C,u32,p);const u32 q=fsPane(p,5),sound=load<u32>(0x101F4FF0);return call<u32>(0x02615154,sound,load<u32>(q+0x50));}
VERIFY(0x026D244C,fsDeleteSound);

// Local descriptor, safe-string and action inline companions referenced by the screen tables.
void fsSmallDtor(u32 p,u32 f) {WWHD_FUNC(0x026D3740,void,p,f);if(p&&(f&1))call<void>(0x0273AF40,p);}
VERIFY(0x026D3740,fsSmallDtor);
void fsInline026D3754() {WWHD_FUNC(0x026D3754,void);}
VERIFY(0x026D3754,fsInline026D3754);
void fsInline026D3758() {WWHD_FUNC(0x026D3758,void);}
VERIFY(0x026D3758,fsInline026D3758);
void fsInline026D375C() {WWHD_FUNC(0x026D375C,void);}
VERIFY(0x026D375C,fsInline026D375C);
u32 fsInline026D3760() {WWHD_FUNC(0x026D3760,u32);return 0;}
VERIFY(0x026D3760,fsInline026D3760);
void fsInline026D3768() {WWHD_FUNC(0x026D3768,void);}
VERIFY(0x026D3768,fsInline026D3768);
void fsInline026D376C() {WWHD_FUNC(0x026D376C,void);}
VERIFY(0x026D376C,fsInline026D376C);
u32 fsInline026D3770() {WWHD_FUNC(0x026D3770,u32);return 0;}
VERIFY(0x026D3770,fsInline026D3770);
void fsInline026D3778() {WWHD_FUNC(0x026D3778,void);}
VERIFY(0x026D3778,fsInline026D3778);
void fsInline026D377C() {WWHD_FUNC(0x026D377C,void);}
VERIFY(0x026D377C,fsInline026D377C);
u32 fsInline026D3780() {WWHD_FUNC(0x026D3780,u32);return 0;}
VERIFY(0x026D3780,fsInline026D3780);
void fsInline026D3788() {WWHD_FUNC(0x026D3788,void);}
VERIFY(0x026D3788,fsInline026D3788);
u32 fsInline026D378C() {WWHD_FUNC(0x026D378C,u32);return 0;}
VERIFY(0x026D378C,fsInline026D378C);
void fsInline026D3794() {WWHD_FUNC(0x026D3794,void);}
VERIFY(0x026D3794,fsInline026D3794);
u32 fsInline026D3798() {WWHD_FUNC(0x026D3798,u32);return 0;}
VERIFY(0x026D3798,fsInline026D3798);
void fsInline026D37A0() {WWHD_FUNC(0x026D37A0,void);}
VERIFY(0x026D37A0,fsInline026D37A0);
u32 fsInline026D37A4() {WWHD_FUNC(0x026D37A4,u32);return 0;}
VERIFY(0x026D37A4,fsInline026D37A4);
void fsInline026D37AC() {WWHD_FUNC(0x026D37AC,void);}
VERIFY(0x026D37AC,fsInline026D37AC);
void fsInline026D37B0() {WWHD_FUNC(0x026D37B0,void);}
VERIFY(0x026D37B0,fsInline026D37B0);
u32 fsInline026D37B4() {WWHD_FUNC(0x026D37B4,u32);return 0;}
VERIFY(0x026D37B4,fsInline026D37B4);
void fsInline026D37BC() {WWHD_FUNC(0x026D37BC,void);}
VERIFY(0x026D37BC,fsInline026D37BC);
void fsInline026D37C0() {WWHD_FUNC(0x026D37C0,void);}
VERIFY(0x026D37C0,fsInline026D37C0);
void fsInline026D37C4() {WWHD_FUNC(0x026D37C4,void);}
VERIFY(0x026D37C4,fsInline026D37C4);
void fsInline026D37C8() {WWHD_FUNC(0x026D37C8,void);}
VERIFY(0x026D37C8,fsInline026D37C8);
u32 fsInline026D37CC() {WWHD_FUNC(0x026D37CC,u32);return 0;}
VERIFY(0x026D37CC,fsInline026D37CC);
u32 fsInline026D37D4() {WWHD_FUNC(0x026D37D4,u32);return 0;}
VERIFY(0x026D37D4,fsInline026D37D4);
void fsInline026D37DC() {WWHD_FUNC(0x026D37DC,void);}
VERIFY(0x026D37DC,fsInline026D37DC);
u32 fsInline026D37E0() {WWHD_FUNC(0x026D37E0,u32);return 0;}
VERIFY(0x026D37E0,fsInline026D37E0);
void fsInline026D37E8() {WWHD_FUNC(0x026D37E8,void);}
VERIFY(0x026D37E8,fsInline026D37E8);
u32 fsInline026D37EC() {WWHD_FUNC(0x026D37EC,u32);return 0;}
VERIFY(0x026D37EC,fsInline026D37EC);
void fsInline026D37F4() {WWHD_FUNC(0x026D37F4,void);}
VERIFY(0x026D37F4,fsInline026D37F4);
u32 fsInline026D37F8() {WWHD_FUNC(0x026D37F8,u32);return 0;}
VERIFY(0x026D37F8,fsInline026D37F8);
u32 fsInline026D3800() {WWHD_FUNC(0x026D3800,u32);return 0;}
VERIFY(0x026D3800,fsInline026D3800);
void fsInline026D3808() {WWHD_FUNC(0x026D3808,void);}
VERIFY(0x026D3808,fsInline026D3808);
u32 fsInline026D380C() {WWHD_FUNC(0x026D380C,u32);return 0;}
VERIFY(0x026D380C,fsInline026D380C);
void fsInline026D3814() {WWHD_FUNC(0x026D3814,void);}
VERIFY(0x026D3814,fsInline026D3814);
u32 fsInline026D3818() {WWHD_FUNC(0x026D3818,u32);return 0;}
VERIFY(0x026D3818,fsInline026D3818);
void fsInline026D3820() {WWHD_FUNC(0x026D3820,void);}
VERIFY(0x026D3820,fsInline026D3820);
u32 fsInline026D3824() {WWHD_FUNC(0x026D3824,u32);return 0;}
VERIFY(0x026D3824,fsInline026D3824);
void fsInline026D382C() {WWHD_FUNC(0x026D382C,void);}
VERIFY(0x026D382C,fsInline026D382C);
u32 fsInline026D3830() {WWHD_FUNC(0x026D3830,u32);return 0;}
VERIFY(0x026D3830,fsInline026D3830);
void fsInline026D3838() {WWHD_FUNC(0x026D3838,void);}
VERIFY(0x026D3838,fsInline026D3838);
u32 fsInline026D383C() {WWHD_FUNC(0x026D383C,u32);return 0;}
VERIFY(0x026D383C,fsInline026D383C);
u32 fsInline026D3844(u32 p) {WWHD_FUNC(0x026D3844,u32,p);return load<u32>(p);}
VERIFY(0x026D3844,fsInline026D3844);
void fsInline026D384C() {WWHD_FUNC(0x026D384C,void);}
VERIFY(0x026D384C,fsInline026D384C);
void fsInline026D3850() {WWHD_FUNC(0x026D3850,void);}
VERIFY(0x026D3850,fsInline026D3850);
void fsInline026D3854() {WWHD_FUNC(0x026D3854,void);}
VERIFY(0x026D3854,fsInline026D3854);
static u32 fsMemberTarget(u32 desc,u32 base,u32 off,u32& self,u32& r9,u32& r10) {const s32 slot=load<s16>(desc+off+2);r9=u32(s32(load<s16>(desc+off)));self=base+r9;if(slot<0)return load<u32>(desc+off+4);const s32 vt=load<s16>(desc+off+6);r9=u32(slot)*8;r10=load<u32>(self+u32(vt))+r9;return load<u32>(r10+4);}
u32 fsDispatch026D3858(u32 desc,u32 base,u32 r5,u32 r6,u32 r7,u32 r8,u32 r9,u32 r10) {WWHD_FUNC(0x026D3858,u32,desc,base,r5,r6,r7,r8,r9,r10);u32 self;const u32 target=fsMemberTarget(desc,base,12,self,r9,r10);return tail_ptr<u32>(target,self,base,r5,r6,r7,r8,r9,r10);}
VERIFY(0x026D3858,fsDispatch026D3858);
u32 fsDispatch026D3898(u32 desc,u32 base,u32 r5,u32 r6,u32 r7,u32 r8,u32 r9,u32 r10) {WWHD_FUNC(0x026D3898,u32,desc,base,r5,r6,r7,r8,r9,r10);u32 self;const u32 target=fsMemberTarget(desc,base,20,self,r9,r10);return tail_ptr<u32>(target,self,base,r5,r6,r7,r8,r9,r10);}
VERIFY(0x026D3898,fsDispatch026D3898);
u32 fsDispatch026D38D8(u32 desc,u32 base,u32 r5,u32 r6,u32 r7,u32 r8,u32 r9,u32 r10) {WWHD_FUNC(0x026D38D8,u32,desc,base,r5,r6,r7,r8,r9,r10);u32 self;const u32 target=fsMemberTarget(desc,base,28,self,r9,r10);return tail_ptr<u32>(target,self,base,r5,r6,r7,r8,r9,r10);}
VERIFY(0x026D38D8,fsDispatch026D38D8);
u32 fsDescriptorValue(u32 p,u32 r4,u32 r5,u32 r6,u32 r7,u32 r8,u32 r9,u32 r10) {WWHD_FUNC(0x026D3918,u32,p,r4,r5,r6,r7,r8,r9,r10);const u32 desc=load<u32>(p+0x2C);if(load<s32>(desc)==-1)return load<u32>(p);return call_ptr<u32>(load<u32>(load<u32>(desc+8)+0x14),desc,r4,r5,r6,r7,r8,r9,r10);}
VERIFY(0x026D3918,fsDescriptorValue);

// Native resource-name and state descriptors, in original store/registration order.
void fsStaticInit() {WWHD_FUNC(0x026D2B68,void);
store<u32>(0x1049A428u,0x00000000u);
store<u32>(0x1049A424u,0x00000000u);
store<u32>(0x1049A420u,0x00000000u);
store<u32>(0x1049A41Cu,0x00000000u);
call<void>(0x028F026C,0x101F69C8u);
store<f32>(0x1049A3E0u,load<f32>(0x10102288u));
store<f32>(0x1049A3E4u,load<f32>(0x1010228Cu));
call<void>(0x028ED6F8,0x1049A3E8u);
call<void>(0x028F026C,0x101F69D4u);
call<void>(0x028EAB2C,0x1049A3E9u);
call<void>(0x028F026C,0x101F69E0u);
store<u32>(0x1049A448u,0x1010205Cu);
store<u32>(0x1049A458u,0x1010205Cu);
store<u32>(0x1049A440u,0x1010205Cu);
store<u32>(0x1049A480u,0x1010205Cu);
store<u32>(0x1049A450u,0x1010205Cu);
store<u32>(0x1049A478u,0x1010205Cu);
store<u32>(0x1049A42Cu,0x101024ACu);
store<u32>(0x1049A4B8u,0x1010205Cu);
store<u32>(0x1049A444u,0x101024B8u);
store<u32>(0x1049A438u,0x1010205Cu);
store<u32>(0x1049A460u,0x1010205Cu);
store<u32>(0x1049A45Cu,0x101023C4u);
store<u32>(0x1049A454u,0x101024CCu);
store<u32>(0x1049A43Cu,0x1010233Cu);
store<u32>(0x1049A468u,0x1010205Cu);
store<u32>(0x1049A4B4u,0x101022A0u);
store<u32>(0x1049A470u,0x1010205Cu);
store<u32>(0x1049A464u,0x101024DCu);
store<u32>(0x1049A434u,0x10102330u);
store<u32>(0x1049A4C0u,0x1010205Cu);
store<u32>(0x1049A4BCu,0x101022B0u);
store<u32>(0x1049A430u,0x1010205Cu);
store<u32>(0x1049A4C8u,0x1010205Cu);
store<u32>(0x1049A44Cu,0x10102290u);
store<u32>(0x1049A4C4u,0x101022C0u);
store<u32>(0x1049A46Cu,0x101023D4u);
store<u32>(0x1049A47Cu,0x10102298u);
store<u32>(0x1049A4D0u,0x1010205Cu);
store<u32>(0x1049A4CCu,0x101022D0u);
store<u32>(0x1049A474u,0x101024E8u);
store<u32>(0x1049A4D8u,0x1010205Cu);
store<u32>(0x1049A4D4u,0x101022E0u);
store<u32>(0x1049A4E0u,0x1010205Cu);
store<u32>(0x1049A4DCu,0x101024FCu);
store<u32>(0x1049A508u,0x1010205Cu);
store<u32>(0x1049A504u,0x10102348u);
store<u32>(0x1049A4E8u,0x1010205Cu);
store<u32>(0x1049A510u,0x1010205Cu);
store<u32>(0x1049A4E4u,0x101023E0u);
store<u32>(0x1049A4F0u,0x1010205Cu);
store<u32>(0x1049A4ECu,0x101023F0u);
store<u32>(0x1049A4F8u,0x1010205Cu);
store<u32>(0x1049A4F4u,0x10102400u);
store<u32>(0x1049A500u,0x1010205Cu);
store<u32>(0x1049A50Cu,0x10102510u);
store<u32>(0x1049A518u,0x1010205Cu);
store<u32>(0x1049A514u,0x10102354u);
store<u32>(0x1049A520u,0x1010205Cu);
store<u32>(0x1049A51Cu,0x10102420u);
store<u32>(0x1049A528u,0x1010205Cu);
store<u32>(0x1049A524u,0x10102520u);
store<u32>(0x1049A530u,0x1010205Cu);
store<u32>(0x1049A498u,0x1010205Cu);
store<u32>(0x1049A490u,0x1010205Cu);
store<u32>(0x1049A52Cu,0x10102530u);
store<u32>(0x1049A538u,0x1010205Cu);
store<u32>(0x1049A5B4u,0x1010205Cu);
store<u32>(0x1049A534u,0x10102544u);
store<u32>(0x1049A540u,0x1010205Cu);
store<u32>(0x1049A4A0u,0x1010205Cu);
store<u32>(0x1049A53Cu,0x10102360u);
store<u32>(0x1049A4B0u,0x1010205Cu);
store<u32>(0x1049A494u,0x10102598u);
store<u32>(0x1049A548u,0x1010205Cu);
store<u32>(0x1049A4A8u,0x1010205Cu);
store<u32>(0x1049A544u,0x10102550u);
store<u32>(0x1049A4A4u,0x101022F0u);
store<u32>(0x1049A4FCu,0x10102410u);
store<u32>(0x1049A5B0u,0x10102348u);
const u32 registration=load<u32>(0x101FD8E4u);
store<u32>(0x1049A49Cu,0x10102430u);
store<u32>(0x1049A488u,0x1010205Cu);
store<u32>(0x1049A4ACu,0x101025B4u);
store<u32>(0x1049A484u,0x10102560u);
store<u32>(0x1049A48Cu,0x1010257Cu);
const u32 firstId=registration?load<u32>(0x101FDD50u):0u;if(!registration)store<u32>(0x101FD8E4u,1u);
store<u16>(0x1049A5C4u,0x00000000u);
store<u16>(0x1049A5C6u,0xFFFFFFFFu);
const u32 value0=(firstId+0x00000001u);
store<u16>(0x1049A5CCu,0x00000000u);
store<u16>(0x1049A5CEu,0xFFFFFFFFu);
store<u32>(0x1049A5B8u,value0);
const u32 value1=(value0+0x00000001u);
store<u16>(0x1049A5D4u,0x00000000u);
store<u32>(0x1049A550u,value1);
store<u32>(0x1049A578u,0x00000000u);
store<u16>(0x1049A55Eu,0x00000019u);
store<u16>(0x1049A576u,0x00000000u);
store<u32>(0x1049A5BCu,0x101025CCu);
store<u32>(0x1049A57Cu,0x101FF32Cu);
store<u32>(0x1049A554u,0x10102308u);
store<u16>(0x1049A56Eu,0x0000001Bu);
store<u16>(0x1049A566u,0x0000001Au);
store<u32>(0x1049A5C0u,0x10102118u);
store<u32>(0x1049A5C8u,0x026CBF5Cu);
store<u16>(0x1049A564u,0x00000000u);
const u32 value2=(value1+0x00000001u);
store<u32>(0x1049A5D0u,0x026D3758u);
store<u32>(0x1049A5E4u,value2);
store<u16>(0x1049A5F0u,0x00000000u);
store<u16>(0x1049A5F2u,0xFFFFFFFFu);
store<u16>(0x1049A5F8u,0x00000000u);
store<u32>(0x1049A570u,0x00000004u);
store<u16>(0x1049A5FAu,0xFFFFFFFFu);
store<u32>(0x1049A5E8u,0x101025DCu);
store<u32>(0x1049A568u,0x00000004u);
store<u16>(0x1049A5D6u,0xFFFFFFFFu);
store<u32>(0x1049A558u,0x10102160u);
store<u32>(0x1049A5ECu,0x10102118u);
store<u32>(0x1049A5F4u,0x026CCFD8u);
store<u32>(0x1049A5FCu,0x026CD740u);
store<u32>(0x1049A604u,0x026D376Cu);
store<u32>(0x1049A5D8u,0x026D375Cu);
const u32 value3=(value2+0x00000001u);
store<u16>(0x1049A600u,0x00000000u);
store<u16>(0x1049A5DCu,0x00000000u);
store<u32>(0x1049A5E0u,0x026D3760u);
store<u16>(0x1049A5DEu,0xFFFFFFFFu);
store<u16>(0x1049A56Cu,0x00000000u);
store<u16>(0x1049A574u,0x00000000u);
store<u32>(0x1049A560u,0x00000004u);
store<u32>(0x1049A610u,value3);
store<u32>(0x1049A614u,0x10102318u);
store<u32>(0x1049A618u,0x10102118u);
store<u16>(0x1049A602u,0xFFFFFFFFu);
store<u16>(0x1049A55Cu,0x00000000u);
store<u32>(0x1049A620u,0x026D3778u);
store<u32>(0x1049A628u,0x026CD898u);
store<u16>(0x1049A608u,0x00000000u);
const u32 value4=(value3+0x00000001u);
store<u32>(0x1049A60Cu,0x026D3770u);
store<u32>(0x1049A63Cu,value4);
store<u16>(0x1049A648u,0x00000000u);
store<u16>(0x1049A64Au,0xFFFFFFFFu);
store<u16>(0x1049A650u,0x00000000u);
store<u16>(0x1049A60Au,0xFFFFFFFFu);
store<u32>(0x1049A630u,0x026D377Cu);
store<u16>(0x1049A61Cu,0x00000000u);
store<u16>(0x1049A61Eu,0xFFFFFFFFu);
store<u32>(0x1049A640u,0x101025F8u);
store<u32>(0x1049A638u,0x026D3780u);
store<u32>(0x1049A644u,0x10102118u);
store<u32>(0x1049A64Cu,0x026CDBFCu);
store<u32>(0x1049A654u,0x026CE118u);
store<u32>(0x1049A65Cu,0x026D3788u);
store<u32>(0x1049A664u,0x026D378Cu);
const u32 value5=(value4+0x00000001u);
store<u16>(0x1049A652u,0xFFFFFFFFu);
store<u16>(0x1049A658u,0x00000000u);
store<u32>(0x1049A668u,value5);
store<u16>(0x1049A65Au,0xFFFFFFFFu);
store<u32>(0x1049A66Cu,0x10102610u);
store<u16>(0x1049A660u,0x00000000u);
store<u32>(0x1049A670u,0x10102118u);
store<u32>(0x1049A678u,0x026CE394u);
const u32 value6=(value5+0x00000001u);
store<u16>(0x1049A624u,0x00000000u);
store<u32>(0x1049A680u,0x026CE620u);
store<u32>(0x1049A688u,0x026D3794u);
store<u16>(0x1049A662u,0xFFFFFFFFu);
store<u32>(0x1049A690u,0x026D3798u);
store<u16>(0x1049A626u,0xFFFFFFFFu);
store<u32>(0x1049A694u,value6);
store<u32>(0x1049A698u,0x1010262Cu);
store<u16>(0x1049A62Cu,0x00000000u);
store<u16>(0x1049A62Eu,0xFFFFFFFFu);
store<u32>(0x1049A69Cu,0x10102118u);
store<u32>(0x1049A6A4u,0x026CEA60u);
store<u32>(0x1049A6ACu,0x026CED0Cu);
store<u16>(0x1049A674u,0x00000000u);
store<u16>(0x1049A676u,0xFFFFFFFFu);
store<u16>(0x1049A67Cu,0x00000000u);
store<u16>(0x1049A67Eu,0xFFFFFFFFu);
store<u16>(0x1049A684u,0x00000000u);
store<u16>(0x1049A686u,0xFFFFFFFFu);
store<u16>(0x1049A6A0u,0x00000000u);
store<u16>(0x1049A6A2u,0xFFFFFFFFu);
store<u16>(0x1049A6A8u,0x00000000u);
store<u16>(0x1049A6AAu,0xFFFFFFFFu);
store<u16>(0x1049A6B0u,0x00000000u);
store<u16>(0x1049A6B2u,0xFFFFFFFFu);
store<u16>(0x1049A6B8u,0x00000000u);
store<u16>(0x1049A6CCu,0x00000000u);
store<u16>(0x1049A6CEu,0xFFFFFFFFu);
store<u16>(0x1049A634u,0x00000000u);
store<u16>(0x1049A636u,0xFFFFFFFFu);
store<u16>(0x1049A6D4u,0x00000000u);
store<u16>(0x1049A6BAu,0xFFFFFFFFu);
store<u16>(0x1049A68Cu,0x00000000u);
const u32 value7=(value6+0x00000001u);
store<u32>(0x1049A6B4u,0x026D37A0u);
store<u16>(0x1049A68Eu,0xFFFFFFFFu);
store<u32>(0x1049A6BCu,0x026D37A4u);
store<u32>(0x1049A6C0u,value7);
store<u32>(0x1049A6C4u,0x10102648u);
store<u32>(0x1049A6C8u,0x10102118u);
store<u32>(0x1049A6D0u,0x026D37ACu);
store<u32>(0x1049A6D8u,0x026CEF90u);
store<u16>(0x1049A6D6u,0xFFFFFFFFu);
store<u16>(0x1049A6DCu,0x00000000u);
store<u16>(0x1049A6DEu,0xFFFFFFFFu);
store<u16>(0x1049A6E4u,0x00000000u);
const u32 value8=(value7+0x00000001u);
store<u32>(0x1049A580u,value8);
store<u16>(0x1049A58Cu,0x00000000u);
store<u16>(0x1049A58Eu,0x0000001Cu);
store<u32>(0x1049A584u,0x10102664u);
store<u32>(0x1049A590u,0x00000004u);
store<u32>(0x1049A598u,0x00000004u);
const u32 value9=(value8+0x00000001u);
store<u32>(0x1049A5A0u,0x00000004u);
store<u32>(0x1049A5A8u,0x00000000u);
store<u32>(0x1049A6ECu,value9);
store<u16>(0x1049A6F8u,0x00000000u);
store<u32>(0x1049A6F0u,0x1010236Cu);
const u32 value10=(value9+0x00000001u);
store<u32>(0x1049A6F4u,0x10102118u);
store<u32>(0x1049A588u,0x10102160u);
store<u32>(0x1049A6E0u,0x026D37B0u);
store<u16>(0x1049A594u,0x00000000u);
store<u32>(0x1049A5ACu,0x101FF32Cu);
store<u32>(0x1049A6E8u,0x026D37B4u);
store<u32>(0x1049A6FCu,0x026D37C0u);
store<u16>(0x1049A6FAu,0xFFFFFFFFu);
store<u32>(0x1049A704u,0x026D37C4u);
store<u32>(0x1049A718u,value10);
store<u16>(0x1049A724u,0x00000000u);
store<u16>(0x1049A726u,0xFFFFFFFFu);
store<u16>(0x1049A700u,0x00000000u);
store<u16>(0x1049A702u,0xFFFFFFFFu);
store<u32>(0x1049A71Cu,0x10102380u);
store<u16>(0x1049A596u,0x0000001Du);
store<u16>(0x1049A708u,0x00000000u);
store<u16>(0x1049A70Au,0xFFFFFFFFu);
store<u16>(0x1049A6E6u,0xFFFFFFFFu);
store<u16>(0x1049A59Cu,0x00000000u);
store<u32>(0x1049A70Cu,0x026D37C8u);
store<u32>(0x1049A714u,0x026D37CCu);
store<u16>(0x1049A710u,0x00000000u);
const u32 value11=(value10+0x00000001u);
store<u16>(0x1049A712u,0xFFFFFFFFu);
store<u32>(0x1049A744u,value11);
store<u16>(0x1049A72Cu,0x00000000u);
store<u16>(0x1049A72Eu,0xFFFFFFFFu);
store<u32>(0x1049A748u,0x10102678u);
store<u16>(0x1049A59Eu,0x0000001Eu);
store<u16>(0x1049A5A4u,0x00000000u);
store<u32>(0x1049A720u,0x10102118u);
store<u32>(0x1049A74Cu,0x10102118u);
store<u32>(0x1049A728u,0x026CF144u);
store<u16>(0x1049A5A6u,0x00000000u);
store<u32>(0x1049A730u,0x026CF264u);
store<u32>(0x1049A738u,0x026CF3ECu);
store<u16>(0x1049A734u,0x00000000u);
store<u16>(0x1049A736u,0xFFFFFFFFu);
store<u32>(0x1049A740u,0x026D37D4u);
store<u16>(0x1049A73Cu,0x00000000u);
store<u16>(0x1049A750u,0x00000000u);
store<u16>(0x1049A752u,0xFFFFFFFFu);
store<u16>(0x1049A758u,0x00000000u);
store<u32>(0x1049A754u,0x026CF408u);
const u32 value12=(value11+0x00000001u);
store<u16>(0x1049A75Au,0xFFFFFFFFu);
store<u16>(0x1049A73Eu,0xFFFFFFFFu);
store<u16>(0x1049A760u,0x00000000u);
store<u16>(0x1049A762u,0xFFFFFFFFu);
store<u32>(0x1049A75Cu,0x026CF4E0u);
store<u32>(0x1049A770u,value12);
store<u32>(0x1049A774u,0x1010239Cu);
store<u32>(0x1049A778u,0x10102118u);
store<u32>(0x1049A764u,0x026D37DCu);
store<u32>(0x1049A780u,0x026CF588u);
store<u16>(0x1049A768u,0x00000000u);
const u32 value13=(value12+0x00000001u);
store<u32>(0x1049A788u,0x026CF660u);
store<u32>(0x1049A79Cu,value13);
const u32 value14=(value13+0x00000001u);
store<u16>(0x1049A77Cu,0x00000000u);
store<u16>(0x1049A77Eu,0xFFFFFFFFu);
store<u16>(0x1049A784u,0x00000000u);
store<u16>(0x1049A786u,0xFFFFFFFFu);
store<u16>(0x1049A78Cu,0x00000000u);
store<u16>(0x1049A76Au,0xFFFFFFFFu);
store<u16>(0x1049A78Eu,0xFFFFFFFFu);
store<u16>(0x1049A794u,0x00000000u);
store<u32>(0x1049A76Cu,0x026D37E0u);
store<u16>(0x1049A796u,0xFFFFFFFFu);
store<u32>(0x1049A790u,0x026D37E8u);
store<u32>(0x1049A7A0u,0x10102690u);
store<u32>(0x1049A7A4u,0x10102118u);
store<u32>(0x1049A798u,0x026D37ECu);
store<u16>(0x1049A7D4u,0x00000000u);
store<u32>(0x1049A7ACu,0x026CF9B4u);
store<u32>(0x1049A7B4u,0x026CFFB4u);
store<u32>(0x1049A7BCu,0x026D37F4u);
store<u32>(0x1049A7C4u,0x026D37F8u);
store<u16>(0x1049A7D6u,0xFFFFFFFFu);
store<u16>(0x1049A7DCu,0x00000000u);
store<u16>(0x1049A7DEu,0xFFFFFFFFu);
store<u32>(0x1049A7C8u,value14);
store<u16>(0x1049A7E4u,0x00000000u);
const u32 value15=(value14+0x00000001u);
store<u32>(0x1049A7CCu,0x1010244Cu);
store<u32>(0x1049A7D0u,0x10102118u);
store<u32>(0x1049A7F4u,value15);
store<u16>(0x1049A7E6u,0xFFFFFFFFu);
store<u16>(0x1049A7A8u,0x00000000u);
store<u16>(0x1049A7ECu,0x00000000u);
store<u16>(0x1049A7AAu,0xFFFFFFFFu);
store<u16>(0x1049A7B0u,0x00000000u);
store<u16>(0x1049A7EEu,0xFFFFFFFFu);
store<u32>(0x1049A7D8u,0x026D0420u);
store<u32>(0x1049A7F8u,0x10102468u);
store<u32>(0x1049A7FCu,0x10102118u);
const u32 value16=(value15+0x00000001u);
store<u32>(0x1049A804u,0x026D0864u);
store<u32>(0x1049A80Cu,0x026D0978u);
store<u16>(0x1049A7B2u,0xFFFFFFFFu);
store<u16>(0x1049A7B8u,0x00000000u);
store<u32>(0x1049A814u,0x026D3808u);
store<u16>(0x1049A800u,0x00000000u);
store<u32>(0x1049A820u,value16);
store<u32>(0x1049A824u,0x10102480u);
store<u16>(0x1049A7BAu,0xFFFFFFFFu);
store<u32>(0x1049A828u,0x10102118u);
store<u16>(0x1049A82Cu,0x00000000u);
store<u16>(0x1049A82Eu,0xFFFFFFFFu);
store<u16>(0x1049A834u,0x00000000u);
store<u16>(0x1049A836u,0xFFFFFFFFu);
store<u16>(0x1049A83Cu,0x00000000u);
store<u16>(0x1049A802u,0xFFFFFFFFu);
store<u16>(0x1049A808u,0x00000000u);
store<u16>(0x1049A7C0u,0x00000000u);
store<u16>(0x1049A7C2u,0xFFFFFFFFu);
store<u16>(0x1049A80Au,0xFFFFFFFFu);
store<u16>(0x1049A810u,0x00000000u);
store<u32>(0x1049A830u,0x026D0A20u);
store<u16>(0x1049A812u,0xFFFFFFFFu);
store<u16>(0x1049A818u,0x00000000u);
store<u16>(0x1049A81Au,0xFFFFFFFFu);
store<u32>(0x1049A838u,0x026D0D84u);
store<u16>(0x1049A83Eu,0xFFFFFFFFu);
store<u16>(0x1049A844u,0x00000000u);
store<u32>(0x1049A7E0u,0x026D0608u);
const u32 value17=(value16+0x00000001u);
store<u32>(0x1049A7E8u,0x026D0848u);
store<u32>(0x1049A84Cu,value17);
store<u16>(0x1049A858u,0x00000000u);
store<u16>(0x1049A85Au,0xFFFFFFFFu);
store<u16>(0x1049A860u,0x00000000u);
store<u16>(0x1049A846u,0xFFFFFFFFu);
store<u32>(0x1049A850u,0x10102494u);
store<u32>(0x1049A840u,0x026D3814u);
store<u32>(0x1049A848u,0x026D3818u);
store<u16>(0x1049A862u,0xFFFFFFFFu);
store<u16>(0x1049A868u,0x00000000u);
store<u32>(0x1049A7F0u,0x026D3800u);
store<u16>(0x1049A86Au,0xFFFFFFFFu);
store<u32>(0x1049A854u,0x10102118u);
store<u32>(0x1049A85Cu,0x026D0FBCu);
const u32 value18=(value17+0x00000001u);
store<u32>(0x1049A81Cu,0x026D380Cu);
store<u32>(0x1049A864u,0x026D100Cu);
store<u32>(0x1049A878u,value18);
store<u32>(0x1049A87Cu,0x101026ACu);
store<u32>(0x1049A880u,0x10102118u);
store<u16>(0x1049A870u,0x00000000u);
store<u32>(0x1049A888u,0x026D11ACu);
store<u32>(0x1049A86Cu,0x026D3820u);
store<u32>(0x1049A874u,0x026D3824u);
store<u32>(0x1049A890u,0x026D12B0u);
store<u32>(0x1049A898u,0x026D382Cu);
store<u32>(0x1049A8A0u,0x026D3830u);
store<u16>(0x1049A872u,0xFFFFFFFFu);
store<u16>(0x1049A884u,0x00000000u);
store<u16>(0x1049A886u,0xFFFFFFFFu);
store<u16>(0x1049A88Cu,0x00000000u);
store<u16>(0x1049A88Eu,0xFFFFFFFFu);
const u32 value19=(value18+0x00000001u);
store<u16>(0x1049A894u,0x00000000u);
store<u16>(0x1049A896u,0xFFFFFFFFu);
store<u32>(0x1049A8A4u,value19);
store<u16>(0x1049A8B0u,0x00000000u);
store<u16>(0x1049A8B2u,0xFFFFFFFFu);
store<u16>(0x1049A8B8u,0x00000000u);
store<u16>(0x1049A89Cu,0x00000000u);
store<u16>(0x1049A8BAu,0xFFFFFFFFu);
store<u32>(0x101FDD50u,value19);
store<u16>(0x1049A8C0u,0x00000000u);
store<u16>(0x1049A89Eu,0xFFFFFFFFu);
store<u32>(0x1049A8A8u,0x101023B0u);
store<u16>(0x1049A8C2u,0xFFFFFFFFu);
store<u32>(0x1049A8ACu,0x10102118u);
store<u16>(0x1049A8C8u,0x00000000u);
store<u32>(0x1049A8B4u,0x026D15A8u);
store<u32>(0x1049A8BCu,0x026D15B8u);
store<u32>(0x1049A8C4u,0x026D3838u);
store<u16>(0x1049A8CAu,0xFFFFFFFFu);
store<u32>(0x1049A8CCu,0x026D383Cu);
}
VERIFY(0x026D2B68,fsStaticInit);
u32 fsCtor(u32 p,u32 service) {WWHD_FUNC(0x026D1634,u32,p,service);if(!p)p=call<u32>(0x0273AD10,0x2D8u);if(!p)return 0;call<void>(0x026F89E8,p);store<u32>(p+4,0x101026C0);store<u32>(p+0x2C,0x101027B8);store<u32>(p+0x30,0x101027C8);if(p+0x74u==0)call<void>(0x0273AD10,0x30u);if(p+0xA4u==0)call<void>(0x0273AD10,0x30u);if(p+0xD4u==0)call<void>(0x0273AD10,0x30u);if(p+0x104u==0)call<void>(0x0273AD10,0xC0u);store<u8>(p+0x1CC,0);store<u8>(p+0x1DC,0);store<u32>(p+0x1D0,0);store<u8>(p+0x1CD,0);store<u32>(p+0x1D4,0);if(p+0x1E0u==0)call<void>(0x0273AD10,0x24u);if(p+0x204u==0)call<void>(0x0273AD10,0x24u);if(p+0x228u==0)call<void>(0x0273AD10,0x24u);if(p+0x24Cu==0)call<void>(0x0273AD10,6u);store<u32>(p+0x270,0);store<u32>(p+0x26C,0);store<u32>(p+0x27C,1);store<u8>(p+0x278,0);store<u8>(p+0x284,0);store<u32>(p+0x280,0);if(p+0x28Cu==0)call<void>(0x0273AD10,0x18u);if(p+0x2A4u==0)call<void>(0x0273AD10,4u);if(p+0x2A8u==0)call<void>(0x0273AD10,0x28u);const f32 zero=load<f32>(0x101021CC);store<u32>(p+0x2D4,service);for(u32 i=0;i<3;i++){const u32 first=p+0x1E0+12*i,second=p+0x228+12*i;store<f32>(first+8,zero);store<f32>(first,zero);store<f32>(first+4,zero);store<f32>(second,zero);store<f32>(second+4,zero);store<f32>(second+8,zero);}return p;}
VERIFY(0x026D1634,fsCtor);
u32 fsTransitionMove(u32 p) {WWHD_FUNC(0x026CCB8C,u32,p);u32 stage=load<u32>(p+0x2C);if(stage==0){u32 delay=load<u32>(p+0x28);if(delay){--delay;store<u32>(p+0x28,delay);if(s32(delay)>0)return 0;stage=load<u32>(p+0x2C);}store<u32>(p+0x2C,stage+1);return 0;}if(stage!=1)return 0;f32 speed=fmuls_ppc(load<f32>(p+0x1C),load<f32>(0x101021C0));const f32 v=load<f32>(p+0x18),limit=fadds_ppc(v,v);if(speed>limit)speed=limit;const f32 steps=load<f32>(0x101021C8),ratio=load<f32>(0x101021C4);store<f32>(p+0x1C,speed);const f32 remain=call<f32>(0x0200EE00,p,p+0xC,ratio,speed,steps);const bool done=remain==load<f32>(0x101021CC);const u32 a=u32(s32(load<s16>(p+0x22))),b=u32(s32(load<s16>(p+0x24)));const u32 alpha=call<u32>(0x0200F564,p+0x20,a,b);return done&&alpha!=0;}
VERIFY(0x026CCB8C,fsTransitionMove);
void fsRefreshSlots(u32 p) {WWHD_FUNC(0x026CCF48,void,p);for(u32 i=0;i<3;i++)if(call<u32>(0x026CCECC,p,i))call<void>(0x02644074,fsPane(p,i+6));}
VERIFY(0x026CCF48,fsRefreshSlots);
void fsSelectInit(u32 p) {WWHD_FUNC(0x026CCFD8,void,p);call<void>(0x026CBF4C,p);call<void>(0x026CCF48,p);call<void>(0x026F17BC,fsPane(p,3));call<void>(0x026CBFCC,p);call<void>(0x026CBF4C,p);if(load<u8>(p+0x278)){store<u32>(p+0x27C,9);call<void>(0x02005708,fsAnim(p),5u,3u);const u32 q=fsPane(p,5);call_ptr<void>(load<u32>(load<u32>(q+4)+0x1F4),q,0u);store<u32>(p+0x270,0);store<u32>(p+0x1D8,3);store<u8>(p+0x1DC,0);store<u8>(p+0x284,0);}else{const u32 count=load<u32>(p+0x48);store<u32>(p+0x27C,1);u32 array=load<u32>(p+0x4C);if(count>5)array+=20;const u32 q=load<u32>(array);call_ptr<void>(load<u32>(load<u32>(q+4)+0x1F4),q,1u);store<u8>(p+0x284,0);store<u32>(p+0x1D8,3);store<u32>(p+0x270,0);store<u8>(p+0x1DC,0);}}
VERIFY(0x026CCFD8,fsSelectInit);
struct FsEvent {u32 data[10];};
u32 fsTakeEvent(u32 p) {WWHD_FUNC(0x026CD0F4,u32,p);Local<FsEvent> incoming,outgoing;call<void>(0x020013C0,incoming.a,load<u32>(0x101F4FF0)+0x30);if(!call<u32>(0x026F620C,load<u32>(incoming.a)))return 0;for(u32 i=0;i<8;i++){const u32 index=load<u32>(0x10102074+4*i),q=fsPane(p,index);if(!q)continue;const u32 id=call<u32>(0x026F5B74,q);if(load<u32>(incoming.a+4)!=id)continue;store<u8>(p+0x284,0);const u32 event=load<u32>(0x10102094+4*i),target=call<u32>(0x026F5B74,q);call<void>(0x02001338,outgoing.a,event,target,0u);call<void>(0x02001FF4,load<u32>(0x101F4FF0)+0x14,outgoing.a);call<void>(0x02001490,outgoing.a);return index==5?0u:event;}return 0;}
VERIFY(0x026CD0F4,fsTakeEvent);
u32 fsSelectCancel(u32 p) {WWHD_FUNC(0x026CD384,u32,p);if(!call<u32>(0x026F6340,load<u32>(0x101F4FF0)+0x30))return 0;if(!load<u8>(p+0x1DC))call<void>(0x025E1988,0x832u);call<void>(0x026CBF4C,p);store<u8>(p+0x1DC,1);call<void>(0x020063C0,p+0x18,0x1049A580u);return 1;}
VERIFY(0x026CD384,fsSelectCancel);
void fsEnableSlots(u32 p) {WWHD_FUNC(0x026CD694,void,p);if(load<u8>(p+0x284))return;u32 a=load<u32>(p+0x4C)+(load<u32>(p+0x48)>6?24:0),q=load<u32>(a);call_ptr<void>(load<u32>(load<u32>(q+4)+0xCC),q,cpu->r[4],a,cpu->r[6]);a=load<u32>(p+0x4C)+(load<u32>(p+0x48)>7?28:0);q=load<u32>(a);call_ptr<void>(load<u32>(load<u32>(q+4)+0xCC),q,cpu->r[4],cpu->r[5],a);q=fsPane(p,8);call_ptr<void>(load<u32>(load<u32>(q+4)+0xCC),q,cpu->r[4],cpu->r[5],cpu->r[6]);}
VERIFY(0x026CD694,fsEnableSlots);
void fsEnableButtons(u32 p) {WWHD_FUNC(0x026CDAEC,void,p);u32 enabled=1;if(call<u32>(0x026CCECC,p,u32(load<u8>(p+0x283)))&&!call<u32>(0x026CDA70,p,u32(load<u8>(p+0x283))))enabled=0;for(u32 i=1;i<=2;i++){const u32 q=fsPane(p,i);call_ptr<void>(load<u32>(load<u32>(q+4)+0x244),q,enabled);}}
VERIFY(0x026CDAEC,fsEnableButtons);
void fsDataInit(u32 p) {WWHD_FUNC(0x026CDBFC,void,p);const u32 save=call<u32>(0x027200A0,load<u32>(0x101F84DC)+0x12C0,u32(load<u8>(p+0x283)));const bool present=call<u32>(0x0271FC5C,save)!=0;const u32 q=fsPane(p,5);if(present)call<void>(0x0262DEF4,q);else call<void>(0x0262DF4C,q);store<u32>(p+0x27C,load<u8>(p+0x278)?10u:1u);call<void>(0x026CBF4C,p);call<void>(0x026CBFCC,p);const u32 index=load<u32>(p+0x280);store<u32>(p+0x274,(index<<2)+4);if(index==0)store<u32>(p+0x274,8);store<u32>(p+0x270,0);call<void>(0x026CD8E4,p,index);if(!load<u8>(p+0x278))call<void>(0x026CDAEC,p);}
VERIFY(0x026CDBFC,fsDataInit);
void fsDeleteButtons(u32 p) {WWHD_FUNC(0x026CDEE4,void,p);Local<u32[2]> name;u32 q=fsPane(p,2);store<u32>(name.a+4,0x1010205C);store<u32>(name.a,0x101021FC);call<void>(0x026F5E80,q,name.a,0x1049A494u);q=fsPane(p,3);store<u32>(name.a+4,0x1010205C);store<u32>(name.a,0x101021FC);call<void>(0x026F5E80,q,name.a,0x1049A49Cu);u32 part=load<u32>(p+0x2A8);store<u8>(part+0x44,(load<u8>(part+0x44)&0xFEu)+1);part=load<u32>(p+0x2AC);const u32 flags=(load<u8>(part+0x44)&0xFEu)+1;const f32 f=load<f32>(0x101021A8);store<u8>(part+0x44,flags);call<void>(0x020053E4,fsAnim(p),1u,1u,f);call<void>(0x026CDCF4,p);}
VERIFY(0x026CDEE4,fsDeleteButtons);
void fsLinkButtons(u32 p) {WWHD_FUNC(0x026CF068,void,p);u32 count=load<u32>(p+0x48),array=load<u32>(p+0x4C);u32 a=load<u32>(array+(count>2?8:0)),b=load<u32>(array+(count>3?12:0));call<void>(0x026F5C08,a,0u,0u,0u,0u,0u,0u,0u,load<u32>(b+0x50));count=load<u32>(p+0x48);array=load<u32>(p+0x4C);a=load<u32>(array+(count>3?12:0));b=load<u32>(array+(count>2?8:0));call<void>(0x026F5C08,a,0u,0u,0u,0u,0u,0u,load<u32>(b+0x50),0u);}
VERIFY(0x026CF068,fsLinkButtons);
void fsEnableDelete(u32 p) {WWHD_FUNC(0x026CF1E0,void,p);if(load<u8>(p+0x284))return;for(u32 i=2;i<=3;i++){const u32 q=fsPane(p,i);call_ptr<void>(load<u32>(load<u32>(q+4)+0xCC),q);}}
VERIFY(0x026CF1E0,fsEnableDelete);
static void fsWaitService(u32 p,u32 state) {fsDraw(p);const u32 ticks=load<u32>(p+0x274);if(ticks)store<u32>(p+0x274,ticks-1);if(call<u32>(0x026CCC94,p)&&load<s32>(p+0x274)<=0){const u32 status=load<u32>(load<u32>(0x101F852C)+0x1C);if(status!=2&&status!=3)call<void>(0x020063C0,p+0x18,state);}}
void fsWaitCopy(u32 p) {WWHD_FUNC(0x026CF4E0,void,p);fsWaitService(p,0x1049A770);}
VERIFY(0x026CF4E0,fsWaitCopy);
void fsWaitDelete(u32 p) {WWHD_FUNC(0x026D0978,void,p);fsWaitService(p,0x1049A820);}
VERIFY(0x026D0978,fsWaitDelete);
void fsEnableCopySlots(u32 p) {WWHD_FUNC(0x026CFF10,void,p);if(load<u8>(p+0x284))return;const u32 second=load<u32>(p+0x1C8)+6,first=load<u32>(p+0x1C4)+6;u32 q=fsPane(p,first);call_ptr<void>(load<u32>(load<u32>(q+4)+0xCC),q);q=fsPane(p,second);call_ptr<void>(load<u32>(load<u32>(q+4)+0xCC),q);}
VERIFY(0x026CFF10,fsEnableCopySlots);
void fsSetPanelOrigins(u32 p,u32 i,u32 pos) {WWHD_FUNC(0x026D1C7C,void,p,i,pos);f32 y=fsRawTransfer(pos+4),x=fsRawTransfer(pos),z=fsRawTransfer(pos+8);const u32 off=i<4?12*i:0;u32 out=p+0x24+off;store<f32>(out+8,z);store<f32>(out,x);store<f32>(out+4,y);z=fsRawTransfer(pos+8);x=fsRawTransfer(pos);const f32 distance=load<f32>(0x10102278);y=fsRawTransfer(pos+4);out=p+0x54+off;store<f32>(out+8,z);store<f32>(out+4,y);store<f32>(out,fadds_ppc(x,distance));x=fsRawTransfer(pos);z=fsRawTransfer(pos+8);y=fsRawTransfer(pos+4);out=p+0x84+off;store<f32>(out+8,z);store<f32>(out+4,y);store<f32>(out,fsubs_ppc(x,distance));}
VERIFY(0x026D1C7C,fsSetPanelOrigins);
void fsFindParts(u32 p) {WWHD_FUNC(0x026D1A14,void,p);const u32 screen=load<u32>(load<u32>(load<u32>(p+0x44)+4)+0xC);for(u32 i=0;i<10;i++){const u32 str=0x1049A4B4+8*i,vt=load<u32>(str+4),fn=load<u32>(vt+0x14),screenVt=load<u32>(screen+8);call_ptr<void>(fn,str,fn);const u32 q=call_ptr<u32>(load<u32>(screenVt+0x5C),screen,load<u32>(str),1u);store<u32>(p+0x2A8+4*i,q);}}
VERIFY(0x026D1A14,fsFindParts);
void fsShowSlots(u32 p) {WWHD_FUNC(0x026D230C,void,p);const u32 count=load<u32>(p+0x48);store<u8>(p+0x1CD,0);u32 array=load<u32>(p+0x4C);store<u8>(p+0x1CC,0);call<void>(0x026F226C,load<u32>(array+(count>3?12:0)));call<void>(0x026F17BC,fsPane(p,load<u32>(p+0x280)+6));for(u32 i=0;i<3;i++)call<void>(0x026CC22C,p+0x50,i,0u,0u,255u,255u,0u,8u);call<void>(0x020063C0,p+0x18,0x1049A5E4u);}
VERIFY(0x026D230C,fsShowSlots);
static void fsBusyPosition(u32 p,bool nameMode) {const u32 q=load<u32>(p+0x298);const f32 y=fsTransfer(p+0x198);const u32 flags=load<u8>(q+0x44);f32 x,z;if(nameMode){x=fsTransfer(p+0x194);z=fsTransfer(p+0x19C);store<f32>(q+0x20,y);store<u8>(q+0x44,flags|0x10);store<f32>(q+0x24,z);}else{z=fsTransfer(p+0x19C);x=fsTransfer(p+0x194);store<f32>(q+0x24,z);store<f32>(q+0x20,y);store<u8>(q+0x44,flags|0x10);}store<f32>(q+0x1C,x);const u32 next=load<u32>(p+0x298);store<u8>(next+0x45,load<u8>(p+0x1B5));}
void fsCopyStart(u32 p) {WWHD_FUNC(0x026CF408,void,p);const u32 manager=call<u32>(0x02035D78);call<void>(0x02035ED4,manager,0u);store<u8>(fsPane(p,9)+0x102,1);call<void>(0x027225AC,load<u32>(0x101F852C),u32(load<u8>(p+0x283)));call<void>(0x026CBFCC,p);call<void>(0x026CBF4C,p);store<u32>(p+0x27C,3);call<void>(0x026CC22C,p+0x50,3u,7u,8u,255u,0u,0u,10u);fsBusyPosition(p,false);store<u32>(p+0x274,30);}
VERIFY(0x026CF408,fsCopyStart);
void fsCopyFinish(u32 p) {WWHD_FUNC(0x026CF588,void,p);const u32 manager=call<u32>(0x02035D78);call<void>(0x02035ED4,manager,1u);store<u8>(fsPane(p,9)+0x102,0);const u32 q=fsPane(p,3);call_ptr<void>(load<u32>(load<u32>(q+4)+0x23C),q);call<void>(0x0262DF4C,fsPane(p,5));store<u32>(p+0x27C,4);call<void>(0x025E1988,0x89Bu);const u32 index=load<u32>(p+0x280);call<void>(0x026CC22C,p+0x50,index,7u,8u,255u,0u,0u,10u);store<u32>(p+0x270,0);}
VERIFY(0x026CF588,fsCopyFinish);
void fsDeleteStart(u32 p) {WWHD_FUNC(0x026D0864,void,p);const u32 manager=call<u32>(0x02035D78);call<void>(0x02035ED4,manager,0u);store<u8>(fsPane(p,9)+0x102,1);const u32 answer=call<u32>(0x026CE368,p);const u32 save=call<u32>(0x027200A0,load<u32>(0x101F84DC)+0x12C0,u32(load<u8>(p+0x283)));call<void>(0x0271FC70,save,answer);const u32 index=load<u32>(p+0x1D4),selected=load<u8>(p+0x283);call<void>(0x027227A8,load<u32>(0x101F852C),(index-6)&255u,selected);call<void>(0x026CBF4C,p);store<u32>(p+0x27C,7);store<u32>(p+0x26C,0);call<void>(0x026CC22C,p+0x50,3u,9u,10u,255u,0u,0u,10u);fsBusyPosition(p,false);store<u32>(p+0x274,30);}
VERIFY(0x026D0864,fsDeleteStart);
void fsNameStart(u32 p) {WWHD_FUNC(0x026D11AC,void,p);const u32 manager=call<u32>(0x02035D78);call<void>(0x02035ED4,manager,0u);const u32 q=fsPane(p,9);const f32 rate=load<f32>(0x101021A8);store<u8>(q+0x102,1);store<u8>(0x1047B07C,0);store<u32>(p+0x27C,11);call<void>(0x020053E4,fsAnim(p),2u,1u,rate);call<void>(0x0272249C,load<u32>(0x101F852C),u32(load<u8>(p+0x283)));call<void>(0x026CBF4C,p);call<void>(0x026CC22C,p+0x50,3u,7u,12u,255u,0u,0u,10u);fsBusyPosition(p,true);store<u32>(p+0x270,0);store<u32>(p+0x274,30);}
VERIFY(0x026D11AC,fsNameStart);
struct FsVec {f32 x,y,z;};
static void fsMotionVector(u32 p,u32 index,u32 mode,u32 out) {const u32 item=index<4?index:0;u32 base=0;switch(mode){case 0:base=p+0x24+12*item;break;case 1:base=p+0x54+12*item;break;case 2:base=p+0x84+12*item;break;case 3:case 5:base=p+0xC;break;case 4:case 6:base=p+0x18;break;case 7:case 12:base=p+0x24;break;case 8:base=p+0x54;break;case 9:base=p+0x30;break;case 10:base=p+0x60;break;case 11:base=p+0xB4+48*item;break;default:return;}f32 x=fsRawTransfer(base),y=fsRawTransfer(base+4),z=fsRawTransfer(base+8);if(mode==5||mode==6)x=fadds_ppc(x,load<f32>(0x101021B0));if(mode==12)y=fsubs_ppc(y,load<f32>(0x101021B4));store<f32>(out,x);store<f32>(out+4,y);store<f32>(out+8,z);}
void fsSetTransition(u32 p,u32 index,u32 from,u32 to,u32 alphaStart,u32 alphaEnd,u32 delay,u32 frames) {WWHD_FUNC(0x026CC22C,void,p,index,from,to,alphaStart,alphaEnd,delay,frames);Local<FsVec> start,end;fsMotionVector(p,index,from,start.a);fsMotionVector(p,index,to,end.a);const f32 x=fsubs_ppc(load<f32>(end.a),load<f32>(start.a)),y=fsubs_ppc(load<f32>(end.a+4),load<f32>(start.a+4)),z=fsubs_ppc(load<f32>(end.a+8),load<f32>(start.a+8));const f32 yy=fmuls_ppc(y,y);const f32 xy=fmadds(x,x,yy);const f32 norm=fmadds(z,z,xy);const f64 distance=call<f64>(0x028F4384,f64(norm));const f32 count=f32(s32(frames)),delta=f32(s32(alphaEnd-alphaStart));const f32 alphaRate=f32(std::fabs(f64(delta))/f64(count));const u32 step=u32(s32(s16(ftoi(alphaRate))));const f32 speed=f32(distance/f64(count));const u32 receiver=p+0xB4+(index<4?48*index:0);call<void>(0x026CC1CC,receiver,delay,start.a,end.a,alphaStart,alphaEnd,step,speed);}
VERIFY(0x026CC22C,fsSetTransition);
void fsChoiceButtons(u32 p) {WWHD_FUNC(0x026CDFE0,void,p);Local<u32[2]> str;store<u32>(str.a+4,0x1010205C);store<u32>(str.a,0x10102208);call<void>(0x026F5E80,fsPane(p,2),str.a,0x1049A4ACu);store<u32>(str.a+4,0x1010205C);store<u32>(str.a,0x10102208);call<void>(0x026F5E80,fsPane(p,3),str.a,0x1049A4A4u);u32 q=load<u32>(p+0x2A8);const f32 rate=load<f32>(0x101021A8);store<u8>(q+0x44,load<u8>(q+0x44)&254);q=load<u32>(p+0x2AC);store<u8>(q+0x44,load<u8>(q+0x44)&254);call<void>(0x020053E4,fsAnim(p),1u,1u,rate);call<void>(0x026F2540,fsPane(p,2),0x104A0CD8u,0x832u);call<void>(0x026F2540,fsPane(p,3),0x104A0CD8u,0x89Au);}
VERIFY(0x026CDFE0,fsChoiceButtons);
void fsReturnMove(u32 p) {WWHD_FUNC(0x026CE620,void,p);u32 done=call<u32>(0x02005840,fsAnim(p),2u);if(done){const u32 state=load<u32>(p+0x24), fn=load<u32>(load<u32>(state+8)+0x14), savedTable=load<u32>(0x1049A778);const u32 current=call_ptr<u32>(fn,state);const u32 wanted=call_ptr<u32>(load<u32>(savedTable+0x14),0x1049A770u);if(current!=wanted)done=call<u32>(0x02005840,fsAnim(p),4u);}if(call<u32>(0x026CCC94,p)&&done)call<void>(0x020063C0,p+0x18,0x1049A5E4u);fsDraw(p);}
VERIFY(0x026CE620,fsReturnMove);
void fsAnimateSlotsIn(u32 p) {WWHD_FUNC(0x026CEAD4,void,p);u32 delay=4;for(u32 i=0;i<3;++i){if(i==load<u32>(p+0x280))call<void>(0x026CC22C,p+0x50,i,7u,0u,255u,255u,0u,8u);else {call<void>(0x026CC22C,p+0x50,i,1u,0u,0u,255u,delay,8u);delay+=4;}const u32 q=load<u32>(p+0x2C0+4*i),v=p+0x104+48*i;const f32 x=fsTransfer(v);const u8 flag=load<u8>(q+0x44);const f32 y=fsTransfer(v+4),z=fsTransfer(v+8);store<u8>(q+0x44,flag|16);store<f32>(q+0x24,z);store<f32>(q+0x20,y);store<f32>(q+0x1C,x);store<u8>(load<u32>(p+0x2C0+4*i)+0x45,load<u8>(v+0x21));}call<void>(0x026CC22C,p+0x50,3u,7u,7u,255u,255u,0u,8u);const u32 q=load<u32>(p+0x298);const f32 z=fsTransfer(p+0x19C);const u8 flag=load<u8>(q+0x44);const f32 y=fsTransfer(p+0x198),x=fsTransfer(p+0x194);store<f32>(q+0x20,y);store<f32>(q+0x1C,x);store<u8>(q+0x44,flag|16);store<f32>(q+0x24,z);store<u8>(load<u32>(p+0x298)+0x45,load<u8>(p+0x1B5));}
VERIFY(0x026CEAD4,fsAnimateSlotsIn);
void fsEnableSelection(u32 p) {WWHD_FUNC(0x026CEC18,void,p);if(!load<u8>(p+0x284)){for(u32 i=0;i<4;++i)fsVcall(fsPane(p,i),0xCC);fsVcall(fsPane(p,5),0xCC);}}
VERIFY(0x026CEC18,fsEnableSelection);
void fsMotionInit(u32 p) {WWHD_FUNC(0x026D1B34,void,p);const f32 zero=load<f32>(0x101021CC);store<f32>(p+0x1C,zero);store<f32>(p+0x10,zero);store<f32>(p+0x14,zero);store<f32>(p+0xC,zero);store<f32>(p+0x18,zero);store<f32>(p+0x20,zero);for(u32 i=0;i<4;++i){const u32 a=p+0x24+12*i,b=p+0x54+12*i,d=p+0x84+12*i;store<f32>(a+4,zero);store<f32>(a+8,zero);store<f32>(a,zero);store<f32>(b+8,zero);store<f32>(b+4,zero);store<f32>(b,zero);store<f32>(d+8,zero);store<f32>(d+4,zero);store<f32>(d,zero);call<void>(0x026D1AF0,p+0xB4+48*i);}}
VERIFY(0x026D1B34,fsMotionInit);
u32 fsCreateScreen(u32 p,u32 resource,u32 archive,u32 options,u32 a,u32 b) {WWHD_FUNC(0x026D2A58,u32,p,resource,archive,options,a,b);u32 q=call<u32>(0x0273B050,0x110u,load<u32>(load<u32>(0x1018C404)+0x10),4u);if(q)q=call<u32>(0x026FADB0,q);store<u32>(p+0x44,q);if(!q)return 0;call_ptr<void>(load<u32>(load<u32>(q+0xE0)+0x14),q,resource,archive,options,a,7u,4u,b);for(u32 i=0;i<7;++i)call<void>(0x02004E04,fsAnim(p),i,0x1049A44Cu+8*i,0x1049A42Cu+8*load<u32>(0x101020B4+4*i));return 1;}
VERIFY(0x026D2A58,fsCreateScreen);
void fsClearNavigation(u32 p) {WWHD_FUNC(0x026CBFCC,void,p);constexpr u32 indices[]={6,7,8,0,1,2,3,5};for(u32 i:indices)call<void>(0x026F5C08,fsPane(p,i),0u,0u,0u,0u,0u,0u,0u,0u);}
VERIFY(0x026CBFCC,fsClearNavigation);
u32 fsUpdateTransitions(u32 p) {WWHD_FUNC(0x026CCC94,u32,p);u32 done=1;for(u32 i=0;i<4;++i){if(!call<u32>(0x026CCB8C,p+0x104+48*i))done=0;if(i<3){const u32 q=load<u32>(p+0x2C0+4*i),v=p+0x104+48*i;const u8 flag=load<u8>(q+0x44);const f32 x=fsTransfer(v),z=fsTransfer(v+8),y=fsTransfer(v+4);store<u8>(q+0x44,flag|16);store<f32>(q+0x24,z);store<f32>(q+0x20,y);store<f32>(q+0x1C,x);store<u8>(load<u32>(p+0x2C0+4*i)+0x45,load<u8>(v+0x21));}else{const u32 q=load<u32>(p+0x298);const f32 z=fsTransfer(p+0x19C);const u8 flag=load<u8>(q+0x44);const f32 y=fsTransfer(p+0x198),x=fsTransfer(p+0x194);store<f32>(q+0x20,y);store<f32>(q+0x1C,x);store<u8>(q+0x44,flag|16);store<f32>(q+0x24,z);store<u8>(load<u32>(p+0x298)+0x45,load<u8>(p+0x1B5));}}return done;}
VERIFY(0x026CCC94,fsUpdateTransitions);
void fsGrowInit(u32 p) {WWHD_FUNC(0x026CC974,void,p);call<void>(0x026F17BC,fsPane(p,3));call<void>(0x026CBFCC,p);call<void>(0x026CBF4C,p);const u32 count=load<u32>(p+0x48);store<u8>(p+0x284,0);fsVcall(load<u32>(load<u32>(p+0x4C)+(count>6?24:0)),0x23C);fsVcall(fsPane(p,7),0x23C);fsVcall(fsPane(p,8),0x23C);const u8 mode=load<u8>(p+0x278);const u32 screen=load<u32>(p+0x44);store<u32>(p+0x27C,mode?9:1);call<void>(0x02005708,load<u32>(screen+0xD4),0u,0u);for(u32 i=0;i<3;++i){call<void>(0x026CC22C,p+0x50,i,1u,0u,0u,255u,4*i,8u);const u32 q=load<u32>(p+0x2C0+4*i),v=p+0x104+48*i;const f32 x=fsTransfer(v);const u8 flag=load<u8>(q+0x44);const f32 y=fsTransfer(v+4),z=fsTransfer(v+8);store<f32>(q+0x1C,x);store<f32>(q+0x20,y);store<f32>(q+0x24,z);store<u8>(q+0x44,flag|16);store<u8>(load<u32>(p+0x2C0+4*i)+0x45,load<u8>(v+0x21));}call<void>(0x026CC22C,p+0x50,3u,7u,8u,0u,0u,0u,8u);const u32 q=load<u32>(p+0x298);const f32 y=fsTransfer(p+0x198);const u8 flag=load<u8>(q+0x44);const f32 x=fsTransfer(p+0x194),z=fsTransfer(p+0x19C);store<f32>(q+0x1C,x);store<f32>(q+0x20,y);store<u8>(q+0x44,flag|16);store<f32>(q+0x24,z);store<u8>(load<u32>(p+0x298)+0x45,load<u8>(p+0x1B5));store<u32>(p+0x274,0);fsDraw(p);}
VERIFY(0x026CC974,fsGrowInit);
void fsLinkSlots(u32 p) {WWHD_FUNC(0x026CD200,void,p);constexpr u32 a[]={6,7,8},left[]={8,6,7},right[]={7,8,6};for(u32 i=0;i<3;++i){const u32 n=load<u32>(p+0x48),arr=load<u32>(p+0x4C),q=load<u32>(arr+(n>a[i]?4*a[i]:0)),l=load<u32>(arr+(n>left[i]?4*left[i]:0)),r=load<u32>(arr+(n>right[i]?4*right[i]:0));call<void>(0x026F5C08,q,load<u32>(l+0x50),0u,0u,load<u32>(r+0x50),0u,0u,0u,0u);}}
VERIFY(0x026CD200,fsLinkSlots);
void fsSelectionMove(u32 p) {WWHD_FUNC(0x026CD740,void,p);fsDraw(p);if(!load<u32>(0x101F4FF0))return;if(!load<u32>(p+0x270)){if(!call<u32>(0x026CCC94,p))return;store<u32>(p+0x270,load<u32>(p+0x270)+1);call<void>(0x026CD200,p);const u32 q=fsPane(p,load<u32>(p+0x1D0));call<void>(0x02615154,load<u32>(0x101F4FF0),load<u32>(q+0x50));}if(call<u32>(0x026CD384,p))return;const u32 event=call<u32>(0x026CD0F4,p);if(!event){call<void>(0x026CD694,p);return;}store<u32>(p+0x280,event-0x56);store<u32>(p+0x1D0,event-0x50);call<void>(0x026CD420,p);const u32 done=call<u32>(0x026CD628,p);call<void>(0x020063C0,p+0x18,done?0x1049A610u:0x1049A63Cu);}
VERIFY(0x026CD740,fsSelectionMove);
void fsLoadSlotData(u32 p,u32 slot) {WWHD_FUNC(0x026CD8E4,void,p,slot);call<void>(0x02726FC4,load<u32>(p+0x2D4));const u32 data=call<u32>(0x027271F0,load<u32>(p+0x2D4),slot&255);if(!data)return;for(u32 i=0;i<8;++i)call<void>(0x025B7BE8,load<u32>(0x101F84DC)+0xD4,i);for(u32 i=0;i<3;++i)call<void>(0x025B7D28,load<u32>(0x101F84DC)+0xD4,i);const u32 save=load<u32>(0x101F84DC);if(!load<u16>(data+0x24)){store<u16>(save+0x20,12);store<u16>(load<u32>(0x101F84DC)+0x22,12);}else{store<u16>(save+0x20,load<u8>(data+1));const u16 health=load<u16>(data+2);store<u16>(load<u32>(0x101F84DC)+0x22,health);for(u32 i=0;i<8;++i)if(load<u8>(data+0x22)&(1u<<i))call<void>(0x025B7B80,load<u32>(0x101F84DC)+0xD4,i);for(u32 i=0;i<3;++i)if(load<u8>(data+0x23)&(1u<<i))call<void>(0x025B7CC0,load<u32>(0x101F84DC)+0xD4,i);}const u32 panel=fsPane(p,4);call<void>(0x026476AC,panel);call<void>(0x02644F0C,fsPane(panel,3));}
VERIFY(0x026CD8E4,fsLoadSlotData);
void fsSelectionTransition(u32 p) {WWHD_FUNC(0x026CD420,void,p);const f32 ten=load<f32>(0x101021D8);const f64 factor=load<f64>(0x101021D0);for(u32 i=0;i<3;++i){if(i==load<u32>(p+0x280))call<void>(0x026CC22C,p+0x50,i,0u,7u,255u,255u,4*i,4u);else {const s32 n=s32(ftoi(f64(s32(i))*factor));const u32 frames=ftoi(fsubs_ppc(ten,f32(n)));call<void>(0x026CC22C,p+0x50,i,0u,1u,255u,0u,4*i,frames);}const u32 q=load<u32>(p+0x2C0+4*i),v=p+0x104+48*i;const f32 x=fsTransfer(v);const u8 flag=load<u8>(q+0x44);const f32 y=fsTransfer(v+4),z=fsTransfer(v+8);store<f32>(q+0x1C,x);store<u8>(q+0x44,flag|16);store<f32>(q+0x20,y);store<f32>(q+0x24,z);store<u8>(load<u32>(p+0x2C0+4*i)+0x45,load<u8>(v+0x21));}call<void>(0x026CC22C,p+0x50,3u,7u,7u,255u,255u,0u,8u);const u32 q=load<u32>(p+0x298);const f32 x=fsTransfer(p+0x194);const u8 flag=load<u8>(q+0x44);const f32 y=fsTransfer(p+0x198),z=fsTransfer(p+0x19C);store<f32>(q+0x20,y);store<f32>(q+0x24,z);store<u8>(q+0x44,flag|16);store<f32>(q+0x1C,x);store<u8>(load<u32>(p+0x298)+0x45,load<u8>(p+0x1B5));call<void>(0x02005708,fsAnim(p),5u,3u);call<void>(0x02005674,fsAnim(p),3u,2u);}
VERIFY(0x026CD420,fsSelectionTransition);
void fsLinkButtonsIn(u32 p) {WWHD_FUNC(0x026CE728,void,p);constexpr u32 rows[][5]={{0,5,5,3,1},{1,5,5,0,2},{2,5,5,1,3},{3,5,5,2,0}};for(auto &r:rows){const u32 n=load<u32>(p+0x48),arr=load<u32>(p+0x4C);const auto pane=[&](u32 i){return load<u32>(arr+(i<n?4*i:0));};const u32 q=pane(r[0]),a=pane(r[1]),b=pane(r[2]),c=pane(r[3]),d=pane(r[4]);call<void>(0x026F5C08,q,load<u32>(a+0x50),0u,0u,load<u32>(b+0x50),0u,0u,load<u32>(c+0x50),load<u32>(d+0x50));}const u32 n=load<u32>(p+0x48),arr=load<u32>(p+0x4C),q=load<u32>(arr+(n>5?20:0)),a=load<u32>(arr+(n>3?12:0));call<void>(0x026F5C08,q,load<u32>(a+0x50),0u,0u,load<u32>(a+0x50),0u,0u,0u,0u);}
VERIFY(0x026CE728,fsLinkButtonsIn);
void fsDataMove(u32 p) {WWHD_FUNC(0x026CE118,void,p);const u32 stage=load<u32>(p+0x270);if(stage==0){const u32 ticks=load<u32>(p+0x274);if(ticks){store<u32>(p+0x274,ticks-1);if(s32(ticks-1)<=0){if(load<u8>(p+0x278))call<void>(0x026CDFE0,p);else call<void>(0x026CDEE4,p);call<void>(0x02005708,fsAnim(p),5u,3u);call<void>(0x020053E4,fsAnim(p),3u,2u,load<f32>(0x101021A8));if(load<u8>(p+0x278)){const u32 q=fsPane(p,5);call_ptr<void>(load<u32>(load<u32>(q+4)+0x1F4),q,0u);}store<u32>(p+0x270,load<u32>(p+0x270)+1);}}call<void>(0x026CCC94,p);}else if(stage==1){if(call<u32>(0x02005840,fsAnim(p),3u))store<u32>(p+0x270,load<u32>(p+0x270)+1);call<void>(0x026CCC94,p);}else if(stage==2){if(call<u32>(0x026CCC94,p)&&call<u32>(0x02005840,fsAnim(p),1u))call<void>(0x020063C0,p+0x18,load<u8>(p+0x278)?0x1049A84Cu:0x1049A694u);}fsDraw(p);}
VERIFY(0x026CE118,fsDataMove);
static u32 fsStateKey(u32 p,u32 wanted) {const u32 state=load<u32>(p+0x24),fn=load<u32>(load<u32>(state+8)+0x14),saved=load<u32>(wanted+8);const u32 a=call_ptr<u32>(fn,state);return a==call_ptr<u32>(load<u32>(saved+0x14),wanted);}
void fsSelectionInput(u32 p) {WWHD_FUNC(0x026CED0C,void,p);fsDraw(p);if(!load<u32>(0x101F4FF0))return;const u32 state=call<u32>(0x02006478,fsPane(p,5)+0x18),fn=load<u32>(load<u32>(state+8)+0x14),saved=load<u32>(0x1048F96C);const u32 a=call_ptr<u32>(fn,state),b=call_ptr<u32>(load<u32>(saved+0x14),0x1048F964u);if(a!=b&&call<u32>(0x026F6340,load<u32>(0x101F4FF0)+0x30)){call<void>(0x020053E4,fsAnim(p),4u,2u,load<f32>(0x101021A8));call<void>(0x026CEAD4,p);call<void>(0x020063C0,p+0x18,0x1049A668u);call<void>(0x025E1988,0x832u);return;}const u32 event=call<u32>(0x026CD0F4,p);if(!event){call<void>(0x026CEC18,p);return;}call<void>(0x026CBF4C,p);call<void>(0x026CBFCC,p);switch(event){case 0x59:call<void>(0x026CEAD4,p);call<void>(0x020063C0,p+0x18,0x1049A668u);break;case 0x5A:store<u32>(p+0x1D8,1);call<void>(0x020063C0,p+0x18,0x1049A718u);break;case 0x5B:store<u32>(p+0x1D8,2);store<u32>(p+0x1D4,6);call<void>(0x020063C0,p+0x18,0x1049A79Cu);break;case 0x5C:call<void>(0x020063C0,p+0x18,0x1049A6C0u);break;}}
VERIFY(0x026CED0C,fsSelectionInput);
void fsReturnInit(u32 p) {WWHD_FUNC(0x026CE394,void,p);call<void>(0x026CBF4C,p);call<void>(0x026CBFCC,p);call<void>(0x026F17BC,fsPane(p,load<u32>(p+0x1D0)));if(fsStateKey(p,0x1049A770)){call<void>(0x02005708,fsAnim(p),4u,2u);call<void>(0x02005708,fsAnim(p),2u,1u);}else {const bool deleting=fsStateKey(p,0x1049A820);const u32 anim=fsAnim(p);const f32 rate=load<f32>(0x101021A8);if(deleting){call<void>(0x02005708,anim,2u,1u);call<void>(0x020053E4,fsAnim(p),4u,2u,rate);}else {call<void>(0x020053E4,anim,4u,2u,rate);call<void>(0x020053E4,fsAnim(p),2u,1u,rate);}}const u32 vt=load<u32>(p+4);store<u32>(p+0x270,0);call_ptr<void>(load<u32>(vt+0xB4),p);fsVcall(p,0x8C);const u32 answer=call<u32>(0x026CE368,p),save=call<u32>(0x027200A0,load<u32>(0x101F84DC)+0x12C0,u32(load<u8>(p+0x283)));call<void>(0x0271FC70,save,answer);}
VERIFY(0x026CE394,fsReturnInit);
static void fsSetLabel(u32 q,u32 name,u32 slot) {Local<u32[2]> label;store<u32>(label.a+4,0x1010205C);store<u32>(label.a,name);call<void>(0x026F6524,q,label.a,0x830u);call<void>(0x026F6534,q,0x104A0CD8u,0x830u);if(slot)call<void>(0x026F2540,q,0x104A0CD8u,slot);}
void fsDataLabels(u32 p) {WWHD_FUNC(0x026CDCF4,void,p);Local<u32[2]> first,others,last,three;u32 array=load<u32>(p+0x4C),count=load<u32>(p+0x48);for(u32 i=0;i<8;++i){const u32 index=i?load<u32>(0x10102074+4*i):5,q=load<u32>(array+(index<count?4*index:0));if(!q)continue;const u32 str=i?others.a:first.a;store<u32>(str+4,0x1010205C);store<u32>(str,0x101021DC);call<void>(0x026F6524,q,str,0x830u);call<void>(0x026F6534,q,0x104A0CD8u,0x830u);if(i)call<void>(0x026F2540,q,0x104A0CD8u,0x831u);array=load<u32>(p+0x4C);count=load<u32>(p+0x48);}u32 q=load<u32>(array);if(q){store<u32>(last.a+4,0x1010205C);store<u32>(last.a,0x101021DC);call<void>(0x026F6524,q,last.a,0x830u);call<void>(0x026F6534,q,0x104A0CD8u,0x830u);call<void>(0x026F2540,q,0x104A0CD8u,0x832u);count=load<u32>(p+0x48);array=load<u32>(p+0x4C);}q=load<u32>(array+(count>3?12:0));if(q){store<u32>(three.a+4,0x1010205C);store<u32>(three.a,0x101021DC);call<void>(0x026F6524,q,three.a,0x830u);call<void>(0x026F6534,q,0x104A0CD8u,0x830u);store<u32>(three.a+4,0x1010205C);store<u32>(three.a,0x101021EC);call<void>(0x026F2540,q,three.a,0u);}}
VERIFY(0x026CDCF4,fsDataLabels);
void fsDeleteInput(u32 p) {WWHD_FUNC(0x026CF264,void,p);if(!load<u32>(p+0x270)){store<u32>(p+0x270,1);call<void>(0x026CF1B8,p);}const u32 event=call<u32>(0x026CD0F4,p);const bool cancel=event==0x5B,accept=event==0x5C;if(!event)call<void>(0x026CF1E0,p);const bool escape=call<u32>(0x026F6340,load<u32>(0x101F4FF0)+0x30)!=0;if(escape||cancel){call<void>(0x026F17BC,fsPane(p,1));call<void>(0x026CEAD4,p);call<void>(0x020063C0,p+0x18,0x1049A668u);if(!cancel)call<void>(0x025E1988,0x832u);return;}if(accept)call<void>(0x020063C0,p+0x18,0x1049A744u);fsDraw(p);}
VERIFY(0x026CF264,fsDeleteInput);
void fsCopyCompleteMove(u32 p) {WWHD_FUNC(0x026CF660,void,p);const u32 stage=load<u32>(p+0x270);if(stage==0){if(call<u32>(0x026CCC94,p)){const u32 next=load<u32>(p+0x270)+1,service=load<u32>(p+0x2D4);store<u32>(p+0x270,next);call<void>(0x02726FC4,service);call<void>(0x02642EEC,fsPane(p,load<u32>(p+0x280)+6));call<void>(0x026CC22C,p+0x50,load<u32>(p+0x280),8u,7u,0u,255u,0u,8u);}}else if(stage==1){if(call<u32>(0x026CCC94,p)){const u32 next=load<u32>(p+0x270)+1;store<u32>(p+0x26C,30);store<u32>(p+0x270,next);}}else if(stage==2){const u32 ticks=load<u32>(p+0x26C);if(ticks){const u32 vt=load<u32>(p+4);store<u32>(p+0x26C,ticks-1);call_ptr<void>(load<u32>(vt+0xB4),p);fsVcall(p,0x8C);return;}call<void>(0x026CEAD4,p);call<void>(0x020063C0,p+0x18,0x1049A668u);}fsDraw(p);}
VERIFY(0x026CF660,fsCopyCompleteMove);
void fsCopyChoicesInit(u32 p) {WWHD_FUNC(0x026CF838,void,p);u32 seen=0,from=5,to=3;for(u32 i=0;i<3;++i){if(i==load<u32>(p+0x280))continue;u32 delay;if(!seen){delay=4;store<u32>(p+0x1C4,i);seen=1;}else{from=6;to=4;delay=seen*4;store<u32>(p+0x1C8,i);}call<void>(0x026CC22C,p+0x50,i,from,to,0u,255u,delay,8u);const u32 q=load<u32>(p+0x2C0+4*i),v=p+0x104+48*i;const f32 z=fsTransfer(v+8);const u8 flag=load<u8>(q+0x44);const f32 y=fsTransfer(v+4),x=fsTransfer(v);store<f32>(q+0x20,y);store<f32>(q+0x1C,x);store<u8>(q+0x44,flag|16);store<f32>(q+0x24,z);const u32 again=load<u32>(p+0x2C0+4*i);const u8 alpha=load<u8>(v+0x21);++seen;store<u8>(again+0x45,alpha);}call<void>(0x026CC22C,p+0x50,3u,7u,8u,255u,0u,0u,10u);const u32 q=load<u32>(p+0x298);const f32 z=fsTransfer(p+0x19C);const u8 flag=load<u8>(q+0x44);const f32 y=fsTransfer(p+0x198),x=fsTransfer(p+0x194);store<f32>(q+0x20,y);store<f32>(q+0x1C,x);store<u8>(q+0x44,flag|16);store<f32>(q+0x24,z);store<u8>(load<u32>(p+0x298)+0x45,load<u8>(p+0x1B5));}
VERIFY(0x026CF838,fsCopyChoicesInit);
void fsLinkCopyChoices(u32 p) {WWHD_FUNC(0x026CFA2C,void,p);const u32 second=load<u32>(p+0x1C8)+6,first=load<u32>(p+0x1C4)+6;const u32 n=load<u32>(p+0x48),array=load<u32>(p+0x4C),q=load<u32>(array+(first<n?4*first:0)),other=load<u32>(array+(second<n?4*second:0));call<void>(0x026F5C08,q,load<u32>(other+0x50),0u,0u,load<u32>(other+0x50),0u,0u,0u,0u);const u32 b=load<u32>(p+0x1C8)+6,n2=load<u32>(p+0x48),ar2=load<u32>(p+0x4C),q2=load<u32>(ar2+(b<n2?4*b:0)),a2=load<u32>(ar2+(first<n2?4*first:0));call<void>(0x026F5C08,q2,load<u32>(a2+0x50),0u,0u,load<u32>(a2+0x50),0u,0u,0u,0u);}
VERIFY(0x026CFA2C,fsLinkCopyChoices);
void fsCopyChoicesMove(u32 p) {WWHD_FUNC(0x026CFFB4,void,p);fsDraw(p);if(!load<u32>(0x101F4FF0))return;const u32 stage=load<u32>(p+0x270);if(stage==0){if(!call<u32>(0x026CCC94,p))return;call<void>(0x026CFA2C,p);u32 q=fsPane(p,load<u32>(p+0x1D4));call<void>(0x02615154,load<u32>(0x101F4FF0),load<u32>(q+0x50));const u32 next=load<u32>(p+0x270)+1,idx=load<u32>(p+0x1C4)+6,n=load<u32>(p+0x48);store<u32>(p+0x270,next);q=load<u32>(load<u32>(p+0x4C)+(idx<n?4*idx:0));call<void>(0x02615154,load<u32>(0x101F4FF0),load<u32>(q+0x50));}else if(stage==1){if(call<u32>(0x026CFB74,p))return;const u32 event=call<u32>(0x026CD0F4,p);if(event){store<u32>(p+0x1D4,event-0x50);call<void>(0x020063C0,p+0x18,0x1049A7C8u);}else call<void>(0x026CFF10,p);}}
VERIFY(0x026CFFB4,fsCopyChoicesMove);
void fsCopyCancelInit(u32 p) {WWHD_FUNC(0x026D0490,void,p);call<void>(0x026CC22C,p+0x50,3u,9u,10u,255u,0u,0u,10u);const u32 busy=load<u32>(p+0x298);const f32 z=fsTransfer(p+0x19C);const u8 flags=load<u8>(busy+0x44);const f32 x=fsTransfer(p+0x194),y=fsTransfer(p+0x198);store<f32>(busy+0x1C,x);store<f32>(busy+0x20,y);store<u8>(busy+0x44,flags|16);store<f32>(busy+0x24,z);store<u8>(load<u32>(p+0x298)+0x45,load<u8>(p+0x1B5));const u32 selected=load<u32>(p+0x1D4)-6;for(u32 i=0;i<3;++i){const u32 original=load<u32>(p+0x280);if(i==original)call<void>(0x026CC22C,p+0x50,original,7u,0u,255u,255u,0u,8u);else if(i==selected)call<void>(0x026CC22C,p+0x50,selected,9u,0u,255u,255u,0u,8u);else call<void>(0x026CC22C,p+0x50,i,1u,0u,0u,255u,4u,8u);const u32 q=load<u32>(p+0x2C0+4*i),v=p+0x104+48*i;const f32 xx=fsTransfer(v);const u8 flag=load<u8>(q+0x44);const f32 zz=fsTransfer(v+8),yy=fsTransfer(v+4);store<f32>(q+0x1C,xx);store<f32>(q+0x24,zz);store<u8>(q+0x44,flag|16);store<f32>(q+0x20,yy);store<u8>(load<u32>(p+0x2C0+4*i)+0x45,load<u8>(v+0x21));}}
VERIFY(0x026D0490,fsCopyCancelInit);
void fsCopyConfirmMove(u32 p) {WWHD_FUNC(0x026D0608,void,p);fsDraw(p);const u32 stage=load<u32>(p+0x270);if(stage==0){if(call<u32>(0x026CCC94,p)){store<u32>(p+0x270,load<u32>(p+0x270)+1);call<void>(0x026CF068,p);}}else if(stage==1){call<void>(0x026CF1B8,p);store<u32>(p+0x270,load<u32>(p+0x270)+1);}const u32 event=call<u32>(0x026CD0F4,p);const bool cancel=event==0x5B,accept=event==0x5C;if(!event)call<void>(0x026CF1E0,p);const bool escape=call<u32>(0x026F6340,load<u32>(0x101F4FF0)+0x30)!=0;if(escape||cancel){call<void>(0x026F17BC,fsPane(p,load<u32>(p+0x1D4)));call<void>(0x026F17BC,fsPane(p,load<u32>(p+0x1D0)));call<void>(0x026CBF4C,p);call<void>(0x026D0490,p);const u32 answer=call<u32>(0x026CE368,p),save=call<u32>(0x027200A0,load<u32>(0x101F84DC)+0x12C0,u32(load<u8>(p+0x283)));call<void>(0x0271FC70,save,answer);call<void>(0x020063C0,p+0x18,0x1049A5E4u);if(!cancel)call<void>(0x025E1988,0x832u);}else if(accept)call<void>(0x020063C0,p+0x18,0x1049A7F4u);}
VERIFY(0x026D0608,fsCopyConfirmMove);
void fsDeleteSlotsIn(u32 p) {WWHD_FUNC(0x026D0C14,void,p);u32 delay=4;for(u32 i=0;i<3;++i){if(i==load<u32>(p+0x280))call<void>(0x026CC22C,p+0x50,i,7u,0u,255u,255u,0u,8u);else if(i==load<u32>(p+0x1D4)-6)call<void>(0x026CC22C,p+0x50,i,9u,0u,255u,255u,0u,8u);else {call<void>(0x026CC22C,p+0x50,i,1u,0u,0u,255u,delay,8u);delay+=4;}const u32 q=load<u32>(p+0x2C0+4*i),v=p+0x104+48*i;const f32 x=fsTransfer(v);const u8 flag=load<u8>(q+0x44);const f32 y=fsTransfer(v+4),z=fsTransfer(v+8);store<u8>(q+0x44,flag|16);store<f32>(q+0x24,z);store<f32>(q+0x20,y);store<f32>(q+0x1C,x);store<u8>(load<u32>(p+0x2C0+4*i)+0x45,load<u8>(v+0x21));}call<void>(0x026CC22C,p+0x50,3u,9u,9u,255u,255u,0u,8u);const u32 q=load<u32>(p+0x298);const f32 z=fsTransfer(p+0x19C);const u8 flag=load<u8>(q+0x44);const f32 y=fsTransfer(p+0x198),x=fsTransfer(p+0x194);store<f32>(q+0x20,y);store<f32>(q+0x1C,x);store<u8>(q+0x44,flag|16);store<f32>(q+0x24,z);store<u8>(load<u32>(p+0x298)+0x45,load<u8>(p+0x1B5));}
VERIFY(0x026D0C14,fsDeleteSlotsIn);
void fsNameConfirmMove(u32 p) {WWHD_FUNC(0x026D100C,void,p);fsDraw(p);if(!load<u32>(0x101F4FF0))return;const u32 event=call<u32>(0x026CD0F4,p);const bool cancel=event==0x5B,accept=event==0x5C;if(!event)call<void>(0x026CF1E0,p);const bool escape=call<u32>(0x026F6340,load<u32>(0x101F4FF0)+0x30)!=0;if(escape||cancel){call<void>(0x026F17BC,fsPane(p,1));call<void>(0x026CEAD4,p);call<void>(0x020063C0,p+0x18,0x1049A668u);if(!cancel)call<void>(0x025E1988,0x832u);return;}if(accept)call<void>(0x020063C0,p+0x18,0x1049A878u);fsDraw(p);}
VERIFY(0x026D100C,fsNameConfirmMove);
void fsDeleteCompleteMove(u32 p) {WWHD_FUNC(0x026D0D84,void,p);const u32 stage=load<u32>(p+0x270);if(stage==0){if(call<u32>(0x026CCC94,p)){store<u32>(p+0x270,load<u32>(p+0x270)+1);call<void>(0x026CC22C,p+0x50,3u,9u,9u,255u,255u,0u,8u);const f32 x=fsTransfer(p+0x194),y=fsTransfer(p+0x198);const u32 q=load<u32>(p+0x298);const f32 z=fsTransfer(p+0x19C);const u8 flag=load<u8>(q+0x44);store<f32>(q+0x1C,x);store<f32>(q+0x24,z);store<u8>(q+0x44,flag|16);store<f32>(q+0x20,y);store<u8>(load<u32>(p+0x298)+0x45,load<u8>(p+0x1B5));call<void>(0x02005708,fsAnim(p),5u,3u);call<void>(0x020053E4,fsAnim(p),3u,2u,load<f32>(0x101021A8));}}else if(stage==1){if(call<u32>(0x02005840,fsAnim(p),3u)){const u32 next=load<u32>(p+0x270)+1;store<u32>(p+0x26C,30);store<u32>(p+0x270,next);}}else if(stage==2){u32 ticks=load<u32>(p+0x26C);if(ticks){--ticks;store<u32>(p+0x26C,ticks);}if(s32(ticks)<=0){call<void>(0x026F17BC,fsPane(p,load<u32>(p+0x1D4)));call<void>(0x026D0C14,p);call<void>(0x020063C0,p+0x18,0x1049A668u);}}fsDraw(p);}
VERIFY(0x026D0D84,fsDeleteCompleteMove);
void fsNameCompleteMove(u32 p) {WWHD_FUNC(0x026D12B0,void,p);const u32 stage=load<u32>(p+0x270);if(stage==0){if(call<u32>(0x026CCC94,p)){const u32 next=load<u32>(p+0x270)+1,vt=load<u32>(p+4);store<u32>(p+0x270,next);call_ptr<void>(load<u32>(vt+0xB4),p);fsVcall(p,0x8C);return;}}else if(stage==1){const u32 ticks=load<u32>(p+0x274);if(ticks){store<u32>(p+0x274,ticks-1);return;}const u32 status=load<u32>(load<u32>(0x101F852C)+0x1C);if(status!=2&&status!=3){call<void>(0x026CC22C,p+0x50,load<u32>(p+0x280),7u,12u,255u,0u,0u,10u);store<u32>(p+0x270,load<u32>(p+0x270)+1);fsDraw(p);return;}}else if(stage==2){if(call<u32>(0x026CCC94,p)){call<void>(0x02726FC4,load<u32>(p+0x2D4));call<void>(0x02642EEC,fsPane(p,load<u32>(p+0x280)+6));call<void>(0x0262DF4C,fsPane(p,5));const u32 q=fsPane(p,5);call_ptr<void>(load<u32>(load<u32>(q+4)+0x1F4),q,0u);store<u8>(fsPane(p,9)+0x102,0);call<void>(0x026CC22C,p+0x50,load<u32>(p+0x280),8u,7u,0u,255u,0u,10u);call<void>(0x026CC22C,p+0x50,3u,8u,7u,0u,255u,3u,10u);call<void>(0x026CD8E4,p,load<u32>(p+0x280));store<u32>(p+0x27C,12);call<void>(0x025E1988,0x89Bu);call<void>(0x02035ED4,call<u32>(0x02035D78),1u);const u32 next=load<u32>(p+0x270)+1,vt=load<u32>(p+4);store<u32>(p+0x270,next);call_ptr<void>(load<u32>(vt+0xB4),p);fsVcall(p,0x8C);return;}}else if(stage==3){if(call<u32>(0x026CCC94,p))call<void>(0x020063C0,p+0x18,0x1049A8A4u);}fsDraw(p);}
VERIFY(0x026D12B0,fsNameCompleteMove);
void fsFindScreenParts(u32 p) {WWHD_FUNC(0x026D1804,void,p);if(!load<u32>(0x1049A54C)){store<u32>(0x1049A54C,1);constexpr u32 names[]={0x10102224,0x1010225C,0x10102214,0x10102268,0x10102234,0x10102248};for(u32 i=0;i<6;++i){store<u32>(0x1049A3F0+8*i,0x1010205C);store<u32>(0x1049A3EC+8*i,names[i]);}call<void>(0x028F026C,0x101F69BCu);}const u32 screen=load<u32>(load<u32>(load<u32>(p+0x44)+4)+0xC);for(u32 i=0;i<6;++i){const u32 str=0x1049A3EC+8*i,fn=load<u32>(load<u32>(str+4)+0x14),vt=load<u32>(screen+8);call_ptr<void>(fn,str);const u32 q=call_ptr<u32>(load<u32>(vt+0x5C),screen,load<u32>(str),1u);store<u32>(p+0x28C+4*i,q);}}
VERIFY(0x026D1804,fsFindScreenParts);
void fsDrawPanels(u32 p,u32 draw) {WWHD_FUNC(0x026D2148,void,p,draw);const u32 state=call<u32>(0x02006478,p+0x18),fn=load<u32>(load<u32>(state+8)+0x14),saved=load<u32>(0x1049A828);const u32 a=call_ptr<u32>(fn,state),b=call_ptr<u32>(load<u32>(saved+0x14),0x1049A820u);u32 count=load<u32>(p+0x48);if(a==b){u32 array=load<u32>(p+0x4C),selected=load<u32>(p+0x1D4);for(u32 i=0;s32(i)<s32(count);++i){if(i!=selected){u32 q=load<u32>(array+(i<count?4*i:0));if(q){q=load<u32>(array+(i<count?4*i:0));call_ptr<void>(load<u32>(load<u32>(q+4)+0x6C),q,draw);count=load<u32>(p+0x48);array=load<u32>(p+0x4C);selected=load<u32>(p+0x1D4);}}}const u32 q=load<u32>(array+(selected<count?4*selected:0));call_ptr<void>(load<u32>(load<u32>(q+4)+0x6C),q,draw);}else{for(u32 i=0;s32(i)<s32(count);++i){const u32 array=load<u32>(p+0x4C);u32 q=load<u32>(array+(i<count?4*i:0));if(q){q=load<u32>(array+(i<count?4*i:0));call_ptr<void>(load<u32>(load<u32>(q+4)+0x6C),q,draw);count=load<u32>(p+0x48);}}}}
VERIFY(0x026D2148,fsDrawPanels);
/* the original computes these with paired singles through its frame (psq_st of the x/y pair at sp+pair,
 * the sum also at sp+sum2 for the next psq_l); the frame keeps them (gabi::NativeFrame, see fsCreate) */
static void fsVectorDifference(u32 dst,u32 a,u32 b,u32 pair) {const f32 x=fsubs_ppc(load<f32>(a+0x1C),load<f32>(b+0x1C)),y=fsubs_ppc(load<f32>(a+0x20),load<f32>(b+0x20));store<f32>(pair,x);store<f32>(pair+4,y);const f32 bz=load<f32>(b+0x24),az=load<f32>(a+0x24);store<f32>(dst,x);store<f32>(dst+4,y);store<f32>(dst+8,fsubs_ppc(az,bz));}
static void fsVectorSumDifference(u32 dst,u32 a,u32 b,u32 c,u32 sum,u32 sum2,u32 res) {const f32 x=fadds_ppc(load<f32>(a+0x1C),load<f32>(b+0x1C)),y=fadds_ppc(load<f32>(a+0x20),load<f32>(b+0x20));store<f32>(sum,x);store<f32>(sum+4,y);store<f32>(sum2,x);store<f32>(sum2+4,y);const f32 bz=load<f32>(b+0x24),az=load<f32>(a+0x24),cx=load<f32>(c+0x1C),cy=load<f32>(c+0x20),cz=load<f32>(c+0x24);const f32 rx=fsubs_ppc(x,cx),ry=fsubs_ppc(y,cy);store<f32>(res,rx);store<f32>(res+4,ry);store<f32>(dst,rx);store<f32>(dst+4,ry);store<f32>(dst+8,fsubs_ppc(fadds_ppc(az,bz),cz));}
u32 fsCreate(u32 p,u32 resource,u32 archive,u32 options) {WWHD_FUNC(0x026D1D14,u32,p,resource,archive,options);/* frame as the original (stwu r1,-0x90): callees run at its sp, and its frame stores stay on the stack (game test 2026-10-05: they end up in the empty slots of cking.sav through initdata_to_card) */NativeFrame<0x90> f;const u32 sp=f.sp();store<u32>(f.entry()+4,cpu->lr);store<u32>(sp,f.entry());for(u32 r=25;r<32;++r)store<u32>(sp+0x74+4*(r-25),cpu->r[r]);call<void>(0x026F90D8,p,0x1049A504u,9u);u32 fn=load<u32>(load<u32>(p+4)+0x7C);if(!call_ptr<u32>(fn,p,0x1049A5B0u,load<u32>(p+0x34),resource,archive,options))return 0;if(!call_ptr<u32>(load<u32>(load<u32>(p+4)+0xA4),p))return 0;call<void>(0x026D1804,p);call<void>(0x026D1A14,p);call<void>(0x026CD200,p);fsVectorDifference(p+0x254,load<u32>(p+0x29C),load<u32>(p+0x28C),sp+0x28);fsVectorSumDifference(p+0x260,load<u32>(p+0x29C),load<u32>(p+0x2A0),load<u32>(p+0x28C),sp+0x34,sp+0x10,sp+0x40);call<void>(0x02005674,fsAnim(p),0u,0u);fsVcall(p,0x8C);call<void>(0x026CDCF4,p);call<void>(0x020063C0,fsPane(p,9)+0x18,0x10494824u);call<void>(0x026D1B34,p+0x50);fsVectorDifference(p+0x5C,load<u32>(p+0x29C),load<u32>(p+0x28C),sp+0x4C);fsVectorSumDifference(p+0x68,load<u32>(p+0x29C),load<u32>(p+0x2A0),load<u32>(p+0x28C),sp+0x58,sp+0x1C,sp+0x64);for(u32 i=0;i<3;++i)call<void>(0x026D1C7C,p+0x50,i,load<u32>(p+0x2C0+4*i)+0x1C);call<void>(0x026D1C7C,p+0x50,3u,load<u32>(p+0x2C4)+0x1C);const u32 name=sp+8;for(u32 i=0;i<4;++i){const u32 q=fsPane(p,i);store<u32>(name+4,0x1010205C);store<u32>(name,0x1010227C);call<void>(0x026F5E80,q,name,0x1049A484u+8*i);}for(u32 i=0;i<3;++i)store<u32>(fsPane(p,i)+0x78,2);const u32 state=load<u32>(p+0x18),created=call_ptr<u32>(load<u32>(load<u32>(state)+0x14),state,0x1049A5B8u);store<u32>(p+0x20,created);call_ptr<void>(load<u32>(load<u32>(created)+0x1C),created);return 1;}
VERIFY(0x026D1D14,fsCreate);
static u32 fsPartIndex(u32 p,u32 i) {return load<u32>(p+0x2A8+(i<10?4*i:0));}
static void fsRestoreIndexed(u32 p,u32 offset,u32 vectorIndex,bool reloadVector) {const u32 q=fsPartIndex(p,load<u32>(p+offset)+(offset==0x1D4?0:6)),v=p+0x104+(vectorIndex<4?48*vectorIndex:0);u8 flag=0;if(vectorIndex<4)flag=load<u8>(q+0x44);const f32 y=fsTransfer(v+4),x=fsTransfer(v),z=fsTransfer(v+8);store<f32>(q+0x1C,x);if(vectorIndex>=4)flag=load<u8>(q+0x44);store<f32>(q+0x20,y);store<f32>(q+0x24,z);store<u8>(q+0x44,flag|16);const u32 actual=load<u32>(p+offset),q2=fsPartIndex(p,actual+(offset==0x1D4?0:6)),vi=reloadVector?actual:vectorIndex;store<u8>(q2+0x45,load<u8>(p+0x104+(vi<4?48*vi:0)+0x21));}
void fsDeleteFinishInit(u32 p) {WWHD_FUNC(0x026D0A20,void,p);call<void>(0x02035ED4,call<u32>(0x02035D78),1u);store<u8>(fsPane(p,9)+0x102,0);fsVcall(fsPane(p,3),0x23C);store<u32>(p+0x27C,8);call<void>(0x025E1988,0x89Bu);call<void>(0x02726FC4,load<u32>(p+0x2D4));call<void>(0x02642EEC,fsPane(p,load<u32>(p+0x1D4)));call<void>(0x02643108,fsPane(p,load<u32>(p+0x1D4)));const u32 index=load<u32>(p+0x1D4)-6;call<void>(0x026CC22C,p+0x50,index,7u,9u,0u,255u,0u,8u);fsRestoreIndexed(p,0x1D4,index,false);call<void>(0x026CD8E4,p,load<u32>(p+0x1D4)-6);store<u32>(p+0x270,0);}
VERIFY(0x026D0A20,fsDeleteFinishInit);
static void fsTouch(u32 p,bool targetR4=false) {const u32 fn=load<u32>(load<u32>(p+4)+0x14);if(targetR4)call_ptr<void>(fn,p,fn);else call_ptr<void>(fn,p);}
static bool fsSameString(u32 a,u32 b) {if(a==b)return true;for(u32 i=0;i<0x40001;++i){const u8 x=load<u8>(a+i),y=load<u8>(b+i);if(x!=y)return false;if(!x)return true;}return false;}
u32 fsCreatePanel(u32 p,u32 info) {WWHD_FUNC(0x026D2474,u32,p,info);/* frame as the original (stwu r1,-0x38; stmw r21,0xc): callees run at its sp (game test 2026-10-05, empty save slots) */NativeFrame<0x38> f;const u32 sp=f.sp();store<u32>(f.entry()+4,cpu->lr);store<u32>(sp,f.entry());for(u32 r=21;r<32;++r)store<u32>(sp+0xC+4*(r-21),cpu->r[r]);const u32 heap=load<u32>(load<u32>(0x1018C404)+0x10);for(u32 i=0;i<10;++i){const u32 type=info+0x14,table=0x1049A4B4+8*i;fsTouch(type);fsTouch(type);const u32 before=load<u32>(type);fsTouch(table,true);if(before!=load<u32>(table)&&!fsSameString(load<u32>(type),load<u32>(table)))continue;constexpr u32 types[]={0x10490D0C,0x104910F0,0x10494884,0x1048F95C},sizes[]={0xC4,0x50,0x104,0x94},ctors[]={0x02642870,0x02647250,0x0266D9F0,0x0262DE3C},creates[]={0x02643F7C,0x026474A8,0x0266DB04,0x0262DED8};for(u32 kind=0;kind<4;++kind){fsTouch(info+8);fsTouch(info+8,kind==1);const u32 old=load<u32>(info+8);fsTouch(types[kind]);if(old!=load<u32>(types[kind])&&!fsSameString(load<u32>(info+8),load<u32>(types[kind])))continue;const u32 count=load<u32>(p+0x48),slot=load<u32>(p+0x4C)+(i<count?4*i:0);u32 q=call<u32>(0x0273B050,sizes[kind],heap,4u);if(q){if(kind==0)q=call<u32>(ctors[kind],q,load<u32>(p+0x2D4),(i-6)&255);else if(kind==3)q=call<u32>(ctors[kind],q,load<u32>(p+0x2D4));else q=call<u32>(ctors[kind],q);}store<u32>(slot,q);const u32 n=load<u32>(p+0x48),array=load<u32>(p+0x4C),made=load<u32>(array+(i<n?4*i:0));if(!made)return 0;return call<u32>(creates[kind],load<u32>(array+(i<n?4*i:0)),0u,0u,info);}return call<u32>(0x026F92C0,p,i,info);}return 1;}
VERIFY(0x026D2474,fsCreatePanel);
u32 fsCancelCopy(u32 p) {WWHD_FUNC(0x026CFB74,u32,p);if(!call<u32>(0x026F6340,load<u32>(0x101F4FF0)+0x30))return 0;call<void>(0x026CBFCC,p);call<void>(0x026CBF4C,p);call<void>(0x026F17BC,fsPane(p,load<u32>(p+0x1D0)));call<void>(0x020063C0,p+0x18,0x1049A5E4u);call<void>(0x025E1988,0x832u);call<void>(0x026CC22C,p+0x50,load<u32>(p+0x280),7u,0u,255u,255u,0u,8u);call<void>(0x026CC22C,p+0x50,load<u32>(p+0x1C4),3u,0u,255u,255u,0u,8u);fsRestoreIndexed(p,0x1C4,load<u32>(p+0x1C4),true);call<void>(0x026CC22C,p+0x50,load<u32>(p+0x1C8),4u,0u,255u,255u,0u,8u);fsRestoreIndexed(p,0x1C8,load<u32>(p+0x1C8),true);call<void>(0x026CC22C,p+0x50,3u,8u,8u,0u,0u,0u,8u);const u32 q=load<u32>(p+0x298);const f32 z=fsTransfer(p+0x19C);const u8 flag=load<u8>(q+0x44);const f32 x=fsTransfer(p+0x194),y=fsTransfer(p+0x198);store<f32>(q+0x1C,x);store<f32>(q+0x24,z);store<u8>(q+0x44,flag|16);store<f32>(q+0x20,y);store<u8>(load<u32>(p+0x298)+0x45,load<u8>(p+0x1B5));return 1;}
VERIFY(0x026CFB74,fsCancelCopy);
void fsCopyConfirmInit(u32 p) {WWHD_FUNC(0x026D0130,void,p);const u32 selected=load<u32>(p+0x1D4)-6,side=selected==load<u32>(p+0x1C8)?4:3;call<void>(0x026CC22C,p+0x50,selected,side,9u,255u,255u,0u,8u);fsRestoreIndexed(p,0x1D4,selected,false);u32 other=load<u32>(p+0x1C4),from=3,to=5;if(selected==other){other=load<u32>(p+0x1C8);from=4;to=6;}call<void>(0x026CC22C,p+0x50,other,from,to,255u,0u,0u,10u);const u32 q=fsPartIndex(p,other+6),v=p+0x104+(other<4?48*other:0);const u8 flag=load<u8>(q+0x44);const f32 x=fsTransfer(v),y=fsTransfer(v+4),z=fsTransfer(v+8);store<u8>(q+0x44,flag|16);store<f32>(q+0x20,y);store<f32>(q+0x24,z);store<f32>(q+0x1C,x);store<u8>(fsPartIndex(p,other+6)+0x45,load<u8>(v+0x21));call<void>(0x026CD8E4,p,selected);call<void>(0x026CC22C,p+0x50,3u,10u,9u,0u,255u,0u,8u);const u32 busy=load<u32>(p+0x298);const f32 bz=fsTransfer(p+0x19C);const u8 bf=load<u8>(busy+0x44);const f32 by=fsTransfer(p+0x198),bx=fsTransfer(p+0x194);store<f32>(busy+0x20,by);store<f32>(busy+0x1C,bx);store<u8>(busy+0x44,bf|16);store<f32>(busy+0x24,bz);store<u8>(load<u32>(p+0x298)+0x45,load<u8>(p+0x1B5));}
VERIFY(0x026D0130,fsCopyConfirmInit);
