// HD name scene.
#include "gabi.h"
#include <utility>
#include <initializer_list>
using namespace gabi;
static u32 U(u32 p,u32 n=0){return load<u32>(p+n);}
static float F(u32 p,u32 n=0){return load<float>(p+n);}
struct NamePair {be<u32>a,b;};
struct NameWords {be<u32> w[132];}; // 0276CAA4 allocates 0x210.
static u32 rel(u32 p){u32 n=U(p);return n?p+n:0;}
u32 namePhase0(u32 p){WWHD_FUNC(0x025AD9B8,u32,p);call<void>(0x025E18EC,0x8000001Eu);return call<u32>(0x02523A04,p,0u)?2:5;}
u32 namePhase1(){WWHD_FUNC(0x025ADA0C,u32);s32 n=call<s32>(0x02523D08);if(n<0)return 5;if(n>0)return 0;return call<u32>(0x026FBB8C,U(0x101F7274u),3u)?2:0;}
u32 namePhase2(){WWHD_FUNC(0x025ADA7C,u32);if(!call<u32>(0x026FBB7C,U(0x101F7274u)))return 0;call<void>(0x02031090);return 2;}
u32 namePhase3(){WWHD_FUNC(0x025ADAC8,u32);return call<u32>(0x020310A4)?4:0;}
void nameSetView(u32 p){WWHD_FUNC(0x025ADB04,void,p);call<void>(0x025200D4);call<void>(0x028E9948,p+0x2D8,F(p,0x2A8),F(p,0x2AC),F(p,0x2A0),F(p,0x2A4));call<void>(0x025F1C90,p+0x318,p+0x2B0,p+0x2BC,load<s16>(p+0x2D4));call<void>(0x028E91EC,p+0x318,p+0x348);call<void>(0x028E90D4,p+0x318,p+0x3B8);store<float>(p+0x3E4,0);store<float>(p+0x3C4,0);store<float>(p+0x3D4,0);float m[12];for(u32 i=0;i<12;i++)m[i]=F(p+0x318+4*i);for(u32 i=0;i<12;i++)store<float>(0x104B45F8u+4*i,m[i]);call<void>(0x025F1FC4,p+0x2D8,p+0x318,p+0x378);}
u32 nameDraw(u32 p){WWHD_FUNC(0x025ADC0C,u32,p);call<void>(0x025ADB04,p);u32 q=call<u32>(0x025DA7AC);while(q){call<void>(0x025DF904,U(q,12));q=call<u32>(0x025DA7D0,q);}return 1;}
u32 nameDrawWrapper(u32 p){WWHD_FUNC(0x025ADC5C,u32,p);return call<u32>(0x025ADC0C,p);}
void nameChangeScene(u32 p){WWHD_FUNC(0x025ADC60,void,p);if(call<u32>(0x025DBE00))return;call<void>(0x02522198);if(!call<u32>(0x025DC86C,p,load<u8>(p+0x534)?12u:7u,0u,5u,1u))return;store<u8>(U(0x101F84DCu)+0x7BC,255);store<u32>(U(0x101F84DCu)+0x116C,0);u32 q=call<u32>(0x025200D4)+0x5140;u32 g=call<u32>(0x025200D4);s32 a=load<s8>(g+0x514A);g=call<u32>(0x025200D4);s32 b=load<s8>(g+0x514B);call<void>(0x025E17CC,q,a,b);}
static bool stateEqual(u32 owner,u32 desc){u32 obj=call<u32>(0x02006478,owner+0x10);u32 vt=U(desc,8);u32 a=call_ptr<u32>(U(U(obj,8),0x14),obj);u32 b=call_ptr<u32>(U(vt,0x14),desc);return a==b;}
void nameNameInput(u32 p){WWHD_FUNC(0x025ADD1C,void,p);u32 q=U(0x101F82ECu);call<void>(0x0270A218,q);if(stateEqual(q,0x1049EAB0u)){call<void>(0x02520230,p,8u);return;}if(stateEqual(q,0x1049EB60u)){store<u8>(p+0x534,load<u8>(q+0x145));call<void>(0x025ADC60,p);}}
void nameSaveSelect(u32 p){WWHD_FUNC(0x025ADE28,void,p);u32 q=U(0x101F82C0u);call<void>(0x0270A218,q);if(stateEqual(q,0x1049E9C4u))call<void>(0x02520230,p,8u);}
void nameFormat(u32 p){WWHD_FUNC(0x025ADEBC,void,p);u32 q=U(0x101F83A4u);call<void>(0x0270A218,q);if(stateEqual(q,0x1049F580u))call<void>(0x02520230,p,8u);}
u32 nameExecute(u32 p){WWHD_FUNC(0x025ADF50,u32,p);if(!call<u32>(0x025DBE00))call<void>(0x025202F8,p);if(!U(U(0x101F4974u))){s16 n=load<s16>(p+8);if(n==9)call<void>(0x025ADD1C,p);else if(n==10)call<void>(0x025ADE28,p);else if(n==11)call<void>(0x025ADEBC,p);call<void>(0x0276C8B8,p+0x538);}return 1;}
u32 nameExecuteWrapper(u32 p){WWHD_FUNC(0x025ADFF8,u32,p);return call<u32>(0x025ADF50,p);}
void nameDtor(u32 p,u32 flags){WWHD_FUNC(0x025ADFFC,void,p,flags);if(!p)return;call<void>(0x027EC220,U(p,0x52C));call<void>(0x02524180,0x10052A70u);store<u8>(call<u32>(0x025200D4)+0x5AC9,0);call<void>(0x025B9864,U(0x101F84DCu)+0x1148,0u);s16 n=load<s16>(p+8);if(n==9){call<void>(0x0270A3D0,U(0x101F82ECu));call<void>(0x0270BF24);}else if(n==10){call<void>(0x0270A3D0,U(0x101F82C0u));call<void>(0x02709004);}else if(n==11){call<void>(0x0270A3D0,U(0x101F83A4u));call<void>(0x0271B458);}u32 heap=U(p,0x1D0);call_ptr<void>(U(U(heap,12),0x24),heap);u32 g=U(0x101F95D0u);store<u32>(U(U(g,0x1024)+(U(g,0x1020)>1?4:0))+0x48,0);store<u32>(U(U(g,0x1024)+(U(g,0x1020)>1?4:0))+0x4C,0);store<u32>(U(U(g,0x1024))+0x48,0);store<u32>(U(U(g,0x1024))+0x4C,0);call<void>(0x027C1F70,g,0u);for(u32 off:{0x5AF8u,0x5F9Cu,0x5FA0u,0x5FA4u})store<u32>(call<u32>(0x025200D4)+off,0);store<u32>(0x104B4708u,0);call<void>(0x0276B21C,p+0x538,2u);call<void>(0x02738A6C,p+0x468,2u);call<void>(0x0274CCB0,p+0x410,2u);call<void>(0x025DD630,p+0x1D4,0u);call<void>(0x025DD630,p,2u);if(flags&1)call<void>(0x0273AF40,p);}
u32 nameDelete(u32 p){WWHD_FUNC(0x025AE208,u32,p);if(U(U(0x101F4F7Cu),0x11248)==2)return 0;call<void>(0x025ADFFC,p,2u);return 1;}
u32 nameCtor(u32 p){WWHD_FUNC(0x025AE25C,u32,p);if(!p){p=call<u32>(0x0273AD10,0x93Cu);if(!p)return 0;}call<void>(0x025DD5F0,p);call<void>(0x025DD5F0,p+0x1D4);store<u32>(p+0x288,0x10052A5Cu);store<u32>(p+0x440,0x101450D0u);for(u32 i=0;i<12;i++)store<u32>(p+0x410+4*i,U(0x104A041Cu+4*i));store<u32>(p+0x440,0x101450F8u);u32 v=p+0x444;if(!v)v=call<u32>(0x0273AD10,12u);if(v){store<float>(v+4,0);store<float>(v,0);store<float>(v+8,10);}v=p+0x450;if(!v)v=call<u32>(0x0273AD10,12u);if(v){store<float>(v+4,0);store<float>(v,0);store<float>(v+8,0);}v=p+0x45C;if(!v)v=call<u32>(0x0273AD10,12u);if(v){store<float>(v+8,0);store<float>(v+4,1);store<float>(v,0);}call<void>(0x027389F8,p+0x468);call<void>(0x0276C1DC,p+0x538);return p;}
void nameCreateEnvironment(u32 p){WWHD_FUNC(0x025AE3C0,void,p);Local<NameWords> cfg;call<void>(0x0276CAA4,cfg.get());for(auto pair:{std::pair<u32,u32>{0x104A1F64u,1u},{0x104A1F70u,8u},{0x104A1460u,1u},{0x104A145Cu,1u}})call<void>(0x0276F9F0,cfg.get(),load<s16>(U(pair.first)),pair.second);u32 env=p+0x538;u32 q=call<u32>(0x0203E8F4);call<void>(0x0276C3F8,env,cfg.get(),q);Local<NamePair> left,right;left->b=0x10052A44u;right->b=0x10052A44u;left->a=0x10052A80u;right->a=0x10052A94u;call<void>(0x026124B0,U(0x101F4F7Cu),left.get(),right.get(),0u);q=call<u32>(0x027E2DC0);u32 e=call<u32>(0x027DFA24,rel(q+0x4C),0x10052A88u);Local<be<u32>> tmp,leftptr,rightptr;q=call<u32>(0x027A7558,tmp.get(),rel(e));*leftptr.get()=U(q);*rightptr.get()=U(q);call<void>(0x0276B710,env,leftptr.get(),rightptr.get(),-1,1.0f);call<void>(0x0276C8B8,env);store<u32>(0x104B4708u,env);}
void nameCreate(u32 p){WWHD_FUNC(0x025AE528,void,p);store<u8>(call<u32>(0x025200D4)+0x5AC9,1);u32 g=call<u32>(0x025200D4),w=g+0x5ACC;call<void>(0x0252CC8C,w,0.0f,0.0f,1280.0f,720.0f,0.0f,1.0f);call<void>(0x0252CCA8,w,0.0f,0.0f,1280.0f,720.0f);store<u8>(w+0x28,0);store<u8>(w+0x29,2);u32 camera=p+0x1D4;store<u32>(call<u32>(0x025200D4)+0x5AF8,camera);g=call<u32>(0x025200D4);store<float>(p+0x2A8,60);store<float>(p+0x2A4,160000);store<float>(p+0x2A0,5);float aspect=(float)(F(g,0x5AD4)/F(g,0x5AD8));store<float>(p+0x2D0,0);store<float>(p+0x2C8,0);store<float>(p+0x2B0,9377);store<float>(p+0x2C4,1000);store<float>(p+0x2BC,0);store<float>(p+0x2CC,1);store<float>(p+0x2B4,0);store<float>(p+0x2C0,6311);store<u16>(p+0x2D4,0);w=g+0x5ACC;store<float>(p+0x2AC,(float)(aspect*F(0x104873D4u)));store<float>(p+0x2B8,7644);store<u32>(call<u32>(0x025200D4)+0x5B2C,0);store<u32>(call<u32>(0x025200D4)+0x5F9C,w);store<u32>(call<u32>(0x025200D4)+0x5FA0,w);store<u32>(call<u32>(0x025200D4)+0x5FA4,camera);store<u8>(0x101F4828u,0);call<void>(0x025ADB04,p);call<void>(0x0255DA78);call<void>(0x025DAD4C,0x1DEu,0u,0u);call<void>(0x025DAD4C,0x1DFu,0u,0u);g=call<u32>(0x025DED64);call<void>(0x025E14A8,g,0x1B5u,0u,0u,0u);g=call<u32>(0x025DED64);call<void>(0x025E14A8,g,0x1B6u,0u,0u,0u);for(auto pair:{std::pair<u32,u8>{0xB90u,0x50},{0xB91u,0x78},{0xB92u,255},{0xB93u,255},{0xB98u,255},{0xB99u,255},{0xB9Au,255},{0xB9Bu,255},{0xBA0u,0xD2},{0xBA1u,0xE5},{0xBA2u,255},{0xBA3u,255}})store<u8>(call<u32>(0x02555D0C)+pair.first,pair.second);float eye[3]={F(p,0x2B0),F(p,0x2B4),F(p,0x2B8)},at[3]={F(p,0x2BC),F(p,0x2C0),F(p,0x2C4)},up[3]={F(p,0x2C8),F(p,0x2CC),F(p,0x2D0)};g=U(0x101F95D0u);for(u32 i=0;i<3;i++)store<float>(p+0x444+4*i,eye[i]);store<float>(camera+0x288,up[0]);store<float>(camera+0x280,at[1]);store<float>(camera+0x28C,up[1]);store<float>(camera+0x290,up[2]);store<float>(camera+0x284,at[2]);store<float>(camera+0x27C,at[0]);u32 view=camera+0x23C;call<void>(0x025155D8,camera+0x288);call_ptr<void>(U(U(camera,0x26C),0x24),view,view);u32 proj=camera+0x294;call<void>(0x0274E04C,proj,F(p,0x2A0),F(p,0x2A4),(float)(F(p,0x2A8)*F(0x10052AC4u)),F(p,0x2AC));store<u32>(U(U(g,0x1024)+(U(g,0x1020)>1?4:0))+0x48,view);store<u32>(U(U(g,0x1024)+(U(g,0x1020)>1?4:0))+0x4C,proj);store<u32>(U(U(g,0x1024))+0x48,view);store<u32>(U(U(g,0x1024))+0x4C,proj);call<void>(0x025AE3C0,p);}
u32 nameCreatePhases(u32 p){WWHD_FUNC(0x025AE8DC,u32,p);u8 n=load<u8>(p+0x938);if(!n){if(p){call<void>(0x025AE25C,p);n=load<u8>(p+0x938);}store<u8>(p+0x938,n+1);}store<u8>(call<u32>(0x025200D4)+0x514C,0);call<void>(0x0252012C,0x10052AD0u,0u,0u,-1,0u,1u,0u,0.0f);u32 stage=call<u32>(0x025200D4)+0x5140;u32 g=call<u32>(0x025200D4);for(u32 i=0;i<6;i++)store<u16>(g+0x5134+2*i,load<u16>(stage+2*i));store<u8>(call<u32>(0x025200D4)+0x514C,0);store<u8>(call<u32>(0x025200D4)+0x62F1,255);u32 result=call<u32>(0x02525FE4,p+0x1C8,0x101EA73Cu,0x10052AC8u);if(result!=4)return result;u32 h=call<u32>(0x025E2FBC);h=call<u32>(0x027EBCB0,0xB0000u,h,0u);store<u32>(p+0x1D0,h);if(!h){call<void>(0x0273AA24,0x10052AE4u,0xF0u,0x10052AD8u);h=U(p,0x1D0);}store<u32>(h+0x10,0x10052AF4u);h=call<u32>(0x025E3570,U(p,0x1D0));store<u32>(p+0x52C,h);call<void>(0x025AE528,p);g=U(0x101F8344u);if(g){store<u8>(0x101F8350u,0);call<void>(0x02715C10,g);call<void>(0x02717B70);}s16 kind=load<s16>(p+8);if(kind==9){store<u8>(U(0x101F84DCu)+0x12B1,0);store<u8>(U(0x101F84DCu)+0x12B2,0);call<void>(0x0270BE70,0u);call<void>(0x02709D24,U(0x101F82ECu));}else if(kind==10){call<void>(0x02708F64,0u);call<void>(0x02709D24,U(0x101F82C0u));}else if(kind==11){call<void>(0x0271B3B8,0u);call<void>(0x02709D24,U(0x101F83A4u));}g=call<u32>(0xC0009C80u);u32 ticks=U(g);call<void>(0x025F096C,(u32)(((uint64_t)(ticks>>2)*0x88888889u)>>36));store<u8>(p+0x938,0);return 4;}
u32 nameCreateWrapper(u32 p){WWHD_FUNC(0x025AEAFC,u32,p);return call<u32>(0x025AE8DC,p);}
void nameStaticInit(){WWHD_FUNC(0x025AEB00,void);store<u32>(0x1047B3DCu,0);store<u32>(0x1047B3D8u,0);store<u32>(0x1047B3D4u,0);store<u32>(0x1047B3D0u,0);call<void>(0x028F026C,0x101EA760u);store<float>(0x1047B3B4u,F(0x10052B08u));store<float>(0x1047B3B8u,F(0x10052B0Cu));call<void>(0x028ED6F8,0x1047B3CCu);call<void>(0x028F026C,0x101EA76Cu);call<void>(0x028EAB2C,0x1047B3CDu);call<void>(0x028F026C,0x101EA778u);store<float>(0x1047B3BCu,50000);store<float>(0x1047B3C4u,10000);store<float>(0x1047B3C0u,50000);store<float>(0x1047B3C8u,10000);}
u32 nameIsDelete(){WWHD_FUNC(0x025AEBC4,u32);return 1;}
void nameEmptyDtor(u32 p,u32 flags){WWHD_FUNC(0x025AEBCC,void,p,flags);if(p&&(flags&1))call<void>(0x0273AF40,p);}
VERIFY(0x025AD9B8,namePhase0);
VERIFY(0x025ADA0C,namePhase1);
VERIFY(0x025ADA7C,namePhase2);
VERIFY(0x025ADAC8,namePhase3);
VERIFY(0x025ADB04,nameSetView);
VERIFY(0x025ADC0C,nameDraw);
VERIFY(0x025ADC5C,nameDrawWrapper);
VERIFY(0x025ADC60,nameChangeScene);
VERIFY(0x025ADD1C,nameNameInput);
VERIFY(0x025ADE28,nameSaveSelect);
VERIFY(0x025ADEBC,nameFormat);
VERIFY(0x025ADF50,nameExecute);
VERIFY(0x025ADFF8,nameExecuteWrapper);
VERIFY(0x025ADFFC,nameDtor);
VERIFY(0x025AE208,nameDelete);
VERIFY(0x025AE25C,nameCtor);
VERIFY(0x025AE3C0,nameCreateEnvironment);
VERIFY(0x025AE528,nameCreate);
VERIFY(0x025AE8DC,nameCreatePhases);
VERIFY(0x025AEAFC,nameCreateWrapper);
VERIFY(0x025AEB00,nameStaticInit);
VERIFY(0x025AEBC4,nameIsDelete);
VERIFY(0x025AEBCC,nameEmptyDtor);
