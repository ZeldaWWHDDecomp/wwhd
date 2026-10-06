/* Retained HD message routing and renderer selection, 025F7980..025F85A8 exclusive. */
#include "wwhd.h"
u32 mesgroute_025F7980(u32 a3,u32 a4,u32 a5) {
WWHD_FUNC(0x025F7980,u32,a3,a4,a5);
gabi::Local<u8[96]> frame;
u32 v0=0,v1=gabi::ea(frame.get()),v3=a3,v4=a4,v5=a5,v6=0,v7=0,v8=0,v9=0,v10=0,v11=0,v12=0,v23=0,v24=0,v25=0,v26=0,v27=0,v28=0,v29=0,v30=0,v31=0;

bool less=false,greater=false,equal=false;u32 count=0;u8 carry=0;
v25 = v3;
v24 = v5;
v29 = v4;
v30 = 0u + 0xFFFFFFFFu;
v27 = 0u + 0x00000000u;
v9 = v1 + 0x0000001Cu;
v11 = 0u + 0x00000020u;
gabi::store<u8>(v1 + 0x0000003Bu, v27);
v0 = 0u + 0x100E0000u;
v31 = 0u + 0x100E0000u;
gabi::store<u32>(v1 + 0x00000018u, v11);
{ uint64_t t = (uint64_t)v0 + 0x00000BA4u; v0 = (uint32_t)t; carry = (uint8_t)(t >> 32); }
v4 = 0u + 0x100E0000u;
gabi::store<u8>(v1 + 0x0000001Cu, v27);
v5 = v29 & 0x0000FFFFu;
v3 = v1 + 0x00000010u;
gabi::store<u32>(v1 + 0x00000014u, v0);
v4 = v4 + 0x00000C70u;
v31 = v31 + 0x00000AE4u;
gabi::store<u32>(v1 + 0x00000010u, v9);
v3=gabi::call<u32>(0x02759C28,v3,v4,v5);
v8 = 0u + 0x10490000u;
{ uint32_t ea = v8 + 0xFFFFD774u; v5 = gabi::load<u32>(ea); v8 = ea; }
v28 = 0u + 0x10490000u;
less=((int32_t)v5)<(0);greater=((int32_t)v5)>(0);equal=((int32_t)v5)==(0);
v28 = v28 + 0xFFFFD760u;
if (equal) { goto L_025F7A28; }
v3 = 0u + 0x101F0000u;
v4 = v25 + 0x00000014u;
v3 = gabi::load<u32>(v3 + 0x00004AE8u);
v5 = v28;
v6 = v1 + 0x00000010u;
v3=gabi::call<u32>(0x025F50E8,v3,v4,v5,v6);
v26 = gabi::load<u32>(v25 + 0x00000240u);
less=((int32_t)v26)<(0);greater=((int32_t)v26)>(0);equal=((int32_t)v26)==(0);
if (equal) { goto L_025F7B1C; }
goto L_025F7B34;
L_025F7A28: ;
v12 = 0u + 0x100E0000u;
gabi::store<u32>(v1 + 0x0000000Cu, v31);
v12 = v12 + 0x00000BECu;
gabi::store<u8>(v28 + 0x00000011u, v27);
v6 = 0u + 0x00000001u;
gabi::store<u32>(v28 + 0x00000004u, v12);
v26 = v28 + 0x0000000Cu;
v9 = 0u + 0x00000006u;
gabi::store<u32>(v8 + 0x00000000u, v6);
v8 = 0u + 0x100E0000u;
gabi::store<u32>(v28 + 0x00000008u, v9);
v8 = v8 + 0x00000C68u;
gabi::store<u32>(v28 + 0x00000000u, v26);
v3 = v1 + 0x00000008u;
gabi::store<u32>(v1 + 0x00000008u, v8);
v3=gabi::call<u32>(0x025F8F80,v3);
v8 = gabi::load<u32>(v1 + 0x00000008u);
v7 = gabi::load<u8>(v8 + 0x00000000u);
v31 = 0u + 0x00000000u;
less=((int32_t)v7)<(0);greater=((int32_t)v7)>(0);equal=((int32_t)v7)==(0);
v10 = 0u + 0x00040000u;
if (equal) { goto L_025F7AA4; }
L_025F7A80: ;
v31 = v31 + 0x00000001u;
less=((int32_t)v31)<((int32_t)v10);greater=((int32_t)v31)>((int32_t)v10);equal=((int32_t)v31)==((int32_t)v10);
v8 = v8 + 0x00000001u;
if (greater) { goto L_025F7AA0; }
v9 = gabi::load<u8>(v8 + 0x00000000u);
less=((int32_t)v9)<(0);greater=((int32_t)v9)>(0);equal=((int32_t)v9)==(0);
if (!equal) { goto L_025F7A80; }
goto L_025F7AA4;
L_025F7AA0: ;
v31 = 0u + 0x00000000u;
L_025F7AA4: ;
v12 = gabi::load<u32>(v28 + 0x00000008u);
less=((int32_t)v31)<((int32_t)v12);greater=((int32_t)v31)>((int32_t)v12);equal=((int32_t)v31)==((int32_t)v12);
if (less) { goto L_025F7AB4; }
v31 = v12 + 0xFFFFFFFFu;
L_025F7AB4: ;
v12 = gabi::load<u32>(v1 + 0x0000000Cu);
v0 = gabi::load<u32>(v12 + 0x00000014u);
count = v0;
v3 = v1 + 0x00000008u;
v3=gabi::call_ptr<u32>(count,v3);
v4 = gabi::load<u32>(v1 + 0x00000008u);
v3 = v26;
v5 = v31;
v6 = 0u + 0x00000000u;
v3=gabi::call<u32>(0xC0009988,v3,v4,v5,v6);
v0 = 0u + 0x100E0000u;
v3 = 0u + 0x101F0000u;
gabi::store<u8>(v26 + v31, v27);
{ uint64_t t = (uint64_t)v0 + 0x00000C04u; v0 = (uint32_t)t; carry = (uint8_t)(t >> 32); }
v3 = v3 + 0x00004B14u;
gabi::store<u32>(v28 + 0x00000004u, v0);
v3=gabi::call<u32>(0x028F026C,v3);
v3 = 0u + 0x101F0000u;
v4 = v25 + 0x00000014u;
v3 = gabi::load<u32>(v3 + 0x00004AE8u);
v5 = v28;
v6 = v1 + 0x00000010u;
v3=gabi::call<u32>(0x025F50E8,v3,v4,v5,v6);
v26 = gabi::load<u32>(v25 + 0x00000240u);
less=((int32_t)v26)<(0);greater=((int32_t)v26)>(0);equal=((int32_t)v26)==(0);
if (!equal) { goto L_025F7B34; }
L_025F7B1C: ;
v0 = gabi::load<u32>(v1 + 0x00000064u);
v3 = 0u + 0xFFFFFFFFu;
return v3;
L_025F7B34: ;
v31 = gabi::load<u32>(v25 + 0x00000938u);
gabi::store<u32>(v25 + 0x00000938u, v29);
v3 = v25;
v28 = gabi::load<u32>(v25 + 0x0000012Cu);
v3=gabi::call<u32>(0x025F795C);
less=((int32_t)v3)<(0);greater=((int32_t)v3)>(0);equal=((int32_t)v3)==(0);
if (!equal) { goto L_025F7B74; }
v23 = gabi::load<u8>(v26 + 0x0000000Eu);
v3=gabi::call<u32>(0x025200D4);
gabi::store<u8>(v3 + 0x00005BC5u, v23);
v23 = gabi::load<u8>(v26 + 0x0000000Bu);
v3=gabi::call<u32>(0x025200D4);
gabi::store<u32>(v3 + 0x00005C34u, v23);
v23 = (uint32_t)(int32_t)(int16_t)gabi::load<u16>(v26 + 0x00000004u);
v3=gabi::call<u32>(0x025200D4);
gabi::store<u16>(v3 + 0x00005BA4u, v23);
L_025F7B74: ;
v10 = gabi::load<u8>(v26 + 0x00000001u);
less=(v10)<(0x0007u);greater=(v10)>(0x0007u);equal=(v10)==(0x0007u);
if (!less) { goto L_025F7BD0; }
less=(v10)<(0x0001u);greater=(v10)>(0x0001u);equal=(v10)==(0x0001u);
if (equal) { goto L_025F7BF4; }
less=(v10)<(0x0002u);greater=(v10)>(0x0002u);equal=(v10)==(0x0002u);
if (equal) { goto L_025F7C18; }
less=(v10)<(0x0006u);greater=(v10)>(0x0006u);equal=(v10)==(0x0006u);
if (equal) { goto L_025F7C40; }
less=(v29)<(0x0266u);greater=(v29)>(0x0266u);equal=(v29)==(0x0266u);
if (!equal) { goto L_025F7CD8; }
L_025F7BA0: ;
v0 = 0u + 0x00000005u;
v3 = gabi::load<u32>(v25 + 0x00000960u);
v4 = v28;
v5 = 0u + 0x00000266u;
v7 = 0u + 0x00000000u;
v6 = 0u + 0x00000002u;
gabi::store<u32>(v25 + 0x0000094Cu, v0);
v3=gabi::call<u32>(0x026AE194,v3,v4,v5,v6,v7);
less=((int32_t)v3)<(0);greater=((int32_t)v3)>(0);equal=((int32_t)v3)==(0);
if (!equal) { goto L_025F7D94; }
L_025F7BC8: ;
gabi::store<u32>(v25 + 0x00000938u, v31);
goto L_025F7D98;
L_025F7BD0: ;
less=(v10)<(0x0007u);greater=(v10)>(0x0007u);equal=(v10)==(0x0007u);
if (equal) { goto L_025F7C68; }
less=(v10)<(0x0009u);greater=(v10)>(0x0009u);equal=(v10)==(0x0009u);
if (equal) { goto L_025F7C90; }
less=(v10)<(0x000Eu);greater=(v10)>(0x000Eu);equal=(v10)==(0x000Eu);
if (equal) { goto L_025F7CB4; }
less=(v29)<(0x0266u);greater=(v29)>(0x0266u);equal=(v29)==(0x0266u);
if (equal) { goto L_025F7BA0; }
goto L_025F7CD8;
L_025F7BF4: ;
v5 = v28;
v3 = gabi::load<u32>(v25 + 0x0000095Cu);
v4 = 0u + 0x00000003u;
v6 = v29;
gabi::store<u32>(v25 + 0x0000094Cu, v27);
v3=gabi::call<u32>(0x026B3F18,v3,v4,v5,v6);
less=((int32_t)v3)<(0);greater=((int32_t)v3)>(0);equal=((int32_t)v3)==(0);
if (equal) { goto L_025F7BC8; }
goto L_025F7D94;
L_025F7C18: ;
v0 = 0u + 0x00000002u;
v3 = gabi::load<u32>(v25 + 0x00000950u);
v5 = v28;
v6 = v29;
v4 = 0u + 0x00000000u;
gabi::store<u32>(v25 + 0x0000094Cu, v0);
v3=gabi::call<u32>(0x026BCBF0,v3,v4,v5,v6);
less=((int32_t)v3)<(0);greater=((int32_t)v3)>(0);equal=((int32_t)v3)==(0);
if (equal) { goto L_025F7BC8; }
goto L_025F7D94;
L_025F7C40: ;
v7 = 0u + 0x00000004u;
v3 = gabi::load<u32>(v25 + 0x00000950u);
v5 = v28;
v6 = v29;
v4 = 0u + 0x00000002u;
gabi::store<u32>(v25 + 0x0000094Cu, v7);
v3=gabi::call<u32>(0x026BCBF0,v3,v4,v5,v6);
less=((int32_t)v3)<(0);greater=((int32_t)v3)>(0);equal=((int32_t)v3)==(0);
if (equal) { goto L_025F7BC8; }
goto L_025F7D94;
L_025F7C68: ;
v10 = 0u + 0x00000003u;
v3 = gabi::load<u32>(v25 + 0x00000950u);
v5 = v28;
v6 = v29;
v4 = 0u + 0x00000001u;
gabi::store<u32>(v25 + 0x0000094Cu, v10);
v3=gabi::call<u32>(0x026BCBF0,v3,v4,v5,v6);
less=((int32_t)v3)<(0);greater=((int32_t)v3)>(0);equal=((int32_t)v3)==(0);
if (equal) { goto L_025F7BC8; }
goto L_025F7D94;
L_025F7C90: ;
v5 = v28;
v3 = gabi::load<u32>(v25 + 0x0000095Cu);
v4 = 0u + 0x00000001u;
v6 = v29;
gabi::store<u32>(v25 + 0x0000094Cu, v27);
v3=gabi::call<u32>(0x026B3F18,v3,v4,v5,v6);
less=((int32_t)v3)<(0);greater=((int32_t)v3)>(0);equal=((int32_t)v3)==(0);
if (equal) { goto L_025F7BC8; }
goto L_025F7D94;
L_025F7CB4: ;
v5 = v28;
v3 = gabi::load<u32>(v25 + 0x0000095Cu);
v4 = 0u + 0x00000000u;
v6 = v29;
gabi::store<u32>(v25 + 0x0000094Cu, v27);
v3=gabi::call<u32>(0x026B3F18,v3,v4,v5,v6);
less=((int32_t)v3)<(0);greater=((int32_t)v3)>(0);equal=((int32_t)v3)==(0);
if (equal) { goto L_025F7BC8; }
goto L_025F7D94;
L_025F7CD8: ;
less=(v29)<(0x0EDCu);greater=(v29)>(0x0EDCu);equal=(v29)==(0x0EDCu);
if (equal) { goto L_025F7D18; }
less=(v29)<(0x0EDDu);greater=(v29)>(0x0EDDu);equal=(v29)==(0x0EDDu);
if (equal) { goto L_025F7D44; }
less=(v29)<(0x0EE2u);greater=(v29)>(0x0EE2u);equal=(v29)==(0x0EE2u);
if (less) { goto L_025F7D70; }
less=(v29)<(0x0EE3u);greater=(v29)>(0x0EE3u);equal=(v29)==(0x0EE3u);
if (!greater) { goto L_025F7D44; }
v9 = 0u + 0x00000001u;
v3 = gabi::load<u32>(v25 + 0x00000954u);
v4 = v28;
v5 = v29;
v6 = v24;
gabi::store<u32>(v25 + 0x0000094Cu, v9);
v3=gabi::call<u32>(0x026B699C,v3,v4,v5,v6);
goto L_025F7D8C;
L_025F7D18: ;
v0 = 0u + 0x00000005u;
v3 = gabi::load<u32>(v25 + 0x00000960u);
v4 = v28;
v5 = 0u + 0x00000EDCu;
v7 = 0u + 0x00000000u;
v6 = 0u + 0x00000003u;
gabi::store<u32>(v25 + 0x0000094Cu, v0);
v3=gabi::call<u32>(0x026AE194,v3,v4,v5,v6,v7);
less=((int32_t)v3)<(0);greater=((int32_t)v3)>(0);equal=((int32_t)v3)==(0);
if (equal) { goto L_025F7BC8; }
goto L_025F7D94;
L_025F7D44: ;
v3 = gabi::load<u32>(v25 + 0x00000960u);
v5 = 0u + 0x00000005u;
v6 = 0u + 0x00000001u;
gabi::store<u32>(v25 + 0x0000094Cu, v5);
v7 = v6;
v5 = v29;
v4 = v28;
v3=gabi::call<u32>(0x026AE194,v3,v4,v5,v6,v7);
less=((int32_t)v3)<(0);greater=((int32_t)v3)>(0);equal=((int32_t)v3)==(0);
if (equal) { goto L_025F7BC8; }
goto L_025F7D94;
L_025F7D70: ;
v9 = 0u + 0x00000001u;
v3 = gabi::load<u32>(v25 + 0x00000954u);
v4 = v28;
v5 = v29;
v6 = v24;
gabi::store<u32>(v25 + 0x0000094Cu, v9);
v3=gabi::call<u32>(0x026B699C,v3,v4,v5,v6);
L_025F7D8C: ;
less=((int32_t)v3)<(0);greater=((int32_t)v3)>(0);equal=((int32_t)v3)==(0);
if (equal) { goto L_025F7BC8; }
L_025F7D94: ;
v30 = 0u + 0x00000000u;
L_025F7D98: ;
v3 = v30;
return v3;
return v3;
}
VERIFY(0x025F7980,mesgroute_025F7980);
u32 mesgroute_025F7DB0(u32 a3,u32 a4,u32 a5) {
WWHD_FUNC(0x025F7DB0,u32,a3,a4,a5);
gabi::Local<u8[24]> frame;
u32 v0=0,v3=a3,v4=a4,v5=a5,v28=0,v29=0,v30=0,v31=0;

bool less=false,greater=false,equal=false;u32 count=0;u8 carry=0;
v28 = v4;
v29 = v5;
v30 = v3;
v31 = 0u + 0xFFFFFFFFu;
v3=gabi::call<u32>(0x025F795C);
less=(v3)<(0x0000u);greater=(v3)>(0x0000u);equal=(v3)==(0x0000u);
if (equal) { goto L_025F7DF4; }
less=(v3)<(0x000Fu);greater=(v3)>(0x000Fu);equal=(v3)==(0x000Fu);
if (equal) { goto L_025F7E30; }
goto L_025F7E44;
L_025F7DF4: ;
v3=gabi::call<u32>(0x025200D4);
v0 = 0u + 0x000000FFu;
gabi::store<u8>(v3 + 0x00005BC6u, v0);
v3=gabi::call<u32>(0x025200D4);
v0 = 0u + 0xFFFFFFFFu;
v5 = v29;
gabi::store<u32>(v3 + 0x00005C30u, v0);
v3 = v30;
v4 = v28;
v3=gabi::call<u32>(0x025F7980,v3,v4,v5);
v31 = v3;
v4 = 0u + 0x00000001u;
v3 = v30;
v3=gabi::call<u32>(0x025F74D0,v3,v4);
goto L_025F7E44;
L_025F7E30: ;
v5 = v29;
v4 = v28;
v3 = v30;
v3=gabi::call<u32>(0x025F7980,v3,v4,v5);
v31 = v3;
L_025F7E44: ;
v3 = v31;
return v3;
return v3;
}
VERIFY(0x025F7DB0,mesgroute_025F7DB0);
u32 mesgroute_025F7E68(u32 a3,u32 a4) {
WWHD_FUNC(0x025F7E68,u32,a3,a4);
gabi::Local<u8[88]> frame;
u32 v0=0,v1=gabi::ea(frame.get()),v3=a3,v4=a4,v5=0,v6=0,v7=0,v8=0,v9=0,v10=0,v12=0,v25=0,v26=0,v27=0,v28=0,v29=0,v30=0,v31=0;

bool less=false,greater=false,equal=false;u32 count=0;u8 carry=0;
v26 = v3;
v27 = v4;
v31 = 0u + 0xFFFFFFFFu;
v0 = 0u + 0x00000020u;
v12 = v1 + 0x0000001Cu;
v25 = 0u + 0x00000000u;
gabi::store<u32>(v1 + 0x00000018u, v0);
v4 = 0u + 0x100E0000u;
v6 = 0u + 0x100E0000u;
gabi::store<u8>(v1 + 0x0000003Bu, v25);
v4 = v4 + 0x00000C80u;
v30 = 0u + 0x100E0000u;
gabi::store<u32>(v1 + 0x00000010u, v12);
v6 = v6 + 0x00000BA4u;
v5 = v27 & 0x0000FFFFu;
gabi::store<u8>(v1 + 0x0000001Cu, v25);
v3 = v1 + 0x00000010u;
v30 = v30 + 0x00000AE4u;
gabi::store<u32>(v1 + 0x00000014u, v6);
v3=gabi::call<u32>(0x02759C28,v3,v4,v5);
v7 = 0u + 0x10490000u;
{ uint32_t ea = v7 + 0xFFFFD778u; v8 = gabi::load<u32>(ea); v7 = ea; }
v29 = 0u + 0x10490000u;
less=((int32_t)v8)<(0);greater=((int32_t)v8)>(0);equal=((int32_t)v8)==(0);
v29 = v29 + 0xFFFFD738u;
if (equal) { goto L_025F7F0C; }
v3 = 0u + 0x101F0000u;
v4 = v26 + 0x00000014u;
v3 = gabi::load<u32>(v3 + 0x00004AE8u);
v5 = v29;
v6 = v1 + 0x00000010u;
v3=gabi::call<u32>(0x025F50E8,v3,v4,v5,v6);
v29 = gabi::load<u32>(v26 + 0x00000240u);
less=((int32_t)v29)<(0);greater=((int32_t)v29)>(0);equal=((int32_t)v29)==(0);
if (equal) { goto L_025F8070; }
goto L_025F8000;
L_025F7F0C: ;
gabi::store<u32>(v1 + 0x0000000Cu, v30);
v9 = 0u + 0x00000001u;
v12 = 0u + 0x100E0000u;
gabi::store<u8>(v29 + 0x00000011u, v25);
v12 = v12 + 0x00000BECu;
gabi::store<u32>(v7 + 0x00000000u, v9);
v0 = 0u + 0x00000006u;
gabi::store<u32>(v29 + 0x00000004u, v12);
v10 = 0u + 0x100E0000u;
v28 = v29 + 0x0000000Cu;
gabi::store<u32>(v29 + 0x00000008u, v0);
v10 = v10 + 0x00000C78u;
gabi::store<u32>(v29 + 0x00000000u, v28);
v3 = v1 + 0x00000008u;
gabi::store<u32>(v1 + 0x00000008u, v10);
v3=gabi::call<u32>(0x025F8F80,v3);
v9 = gabi::load<u32>(v1 + 0x00000008u);
v0 = gabi::load<u8>(v9 + 0x00000000u);
v30 = 0u + 0x00000000u;
less=((int32_t)v0)<(0);greater=((int32_t)v0)>(0);equal=((int32_t)v0)==(0);
v10 = 0u + 0x00040000u;
if (equal) { goto L_025F7F88; }
L_025F7F64: ;
v30 = v30 + 0x00000001u;
less=((int32_t)v30)<((int32_t)v10);greater=((int32_t)v30)>((int32_t)v10);equal=((int32_t)v30)==((int32_t)v10);
v9 = v9 + 0x00000001u;
if (greater) { goto L_025F7F84; }
v6 = gabi::load<u8>(v9 + 0x00000000u);
less=((int32_t)v6)<(0);greater=((int32_t)v6)>(0);equal=((int32_t)v6)==(0);
if (!equal) { goto L_025F7F64; }
goto L_025F7F88;
L_025F7F84: ;
v30 = 0u + 0x00000000u;
L_025F7F88: ;
v12 = gabi::load<u32>(v29 + 0x00000008u);
v7 = gabi::load<u32>(v1 + 0x0000000Cu);
less=((int32_t)v30)<((int32_t)v12);greater=((int32_t)v30)>((int32_t)v12);equal=((int32_t)v30)==((int32_t)v12);
v8 = gabi::load<u32>(v7 + 0x00000014u);
if (less) { goto L_025F7FA0; }
v30 = v12 + 0xFFFFFFFFu;
L_025F7FA0: ;
count = v8;
v3 = v1 + 0x00000008u;
v3=gabi::call_ptr<u32>(count,v3);
v4 = gabi::load<u32>(v1 + 0x00000008u);
v3 = v28;
v5 = v30;
v6 = 0u + 0x00000000u;
v3=gabi::call<u32>(0xC0009988,v3,v4,v5,v6);
v0 = 0u + 0x100E0000u;
v3 = 0u + 0x101F0000u;
gabi::store<u8>(v28 + v30, v25);
{ uint64_t t = (uint64_t)v0 + 0x00000C04u; v0 = (uint32_t)t; carry = (uint8_t)(t >> 32); }
v3 = v3 + 0x00004B20u;
gabi::store<u32>(v29 + 0x00000004u, v0);
v3=gabi::call<u32>(0x028F026C,v3);
v3 = 0u + 0x101F0000u;
v4 = v26 + 0x00000014u;
v3 = gabi::load<u32>(v3 + 0x00004AE8u);
v5 = v29;
v6 = v1 + 0x00000010u;
v3=gabi::call<u32>(0x025F50E8,v3,v4,v5,v6);
v29 = gabi::load<u32>(v26 + 0x00000240u);
less=((int32_t)v29)<(0);greater=((int32_t)v29)>(0);equal=((int32_t)v29)==(0);
if (equal) { goto L_025F8070; }
L_025F8000: ;
v3=gabi::call<u32>(0x025200D4);
v0 = gabi::load<u32>(v3 + 0x00005CD8u);
v9 = v0 & 0x00200000u; less=(s32)v9<0;greater=(s32)v9>0;equal=v9==0;
if (equal) { goto L_025F8050; }
v3=gabi::call<u32>(0x025200D4);
v0 = gabi::load<u8>(v3 + 0x00005BB3u);
less=((int32_t)v0)<(11);greater=((int32_t)v0)>(11);equal=((int32_t)v0)==(11);
if (!equal) { goto L_025F8050; }
v30 = gabi::load<u8>(v29 + 0x0000000Eu);
v3=gabi::call<u32>(0x025200D4);
gabi::store<u8>(v3 + 0x00005BC5u, v30);
v30 = gabi::load<u8>(v29 + 0x0000000Bu);
v3=gabi::call<u32>(0x025200D4);
gabi::store<u32>(v3 + 0x00005C34u, v30);
v30 = (uint32_t)(int32_t)(int16_t)gabi::load<u16>(v29 + 0x00000004u);
v3=gabi::call<u32>(0x025200D4);
gabi::store<u16>(v3 + 0x00005BA4u, v30);
v3=gabi::call<u32>(0x025200D4);
v0 = 0u + 0x00000002u;
gabi::store<u8>(v3 + 0x00005BB3u, v0);
L_025F8050: ;
v4 = gabi::load<u32>(v26 + 0x0000012Cu);
v5 = v27;
v3 = gabi::load<u32>(v26 + 0x00000958u);
v3=gabi::call<u32>(0x026B8FD0,v3,v4,v5);
less=((int32_t)v3)<(0);greater=((int32_t)v3)>(0);equal=((int32_t)v3)==(0);
if (equal) { goto L_025F8070; }
v31 = 0u + 0x00000000u;
gabi::store<u32>(v26 + 0x00000938u, v27);
L_025F8070: ;
v3 = v31;
return v3;
return v3;
}
VERIFY(0x025F7E68,mesgroute_025F7E68);
u32 mesgroute_025F8088(u32 a3) {
WWHD_FUNC(0x025F8088,u32,a3);
gabi::Local<u8[32]> frame;
u32 v0=0,v1=gabi::ea(frame.get()),v3=a3,v4=0,v5=0,v10=0,v11=0,v30=0,v31=0;
f64 q0=0;
bool less=false,greater=false,equal=false;u32 count=0;u8 carry=0;
v11 = 0u + 0x100E0000u;
q0=gabi::load<f32>(v11 + 0x00000C4Cu);
{ uint32_t ea = v1 + 0x0000000Cu; gabi::store<f32>(ea, q0); }
v31 = 0u + 0xFFFFFFFFu;
{ uint32_t ea = v1 + 0x00000008u; gabi::store<f32>(ea, q0); }
v30 = v3;
{ uint32_t ea = v1 + 0x00000010u; gabi::store<f32>(ea, q0); }
v3=gabi::call<u32>(0x025F795C);
less=(v3)<(0x0000u);greater=(v3)>(0x0000u);equal=(v3)==(0x0000u);
if (equal) { goto L_025F80E0; }
less=(v3)<(0x000Fu);greater=(v3)>(0x000Fu);equal=(v3)==(0x000Fu);
if (equal) { goto L_025F810C; }
v3=gabi::call<u32>(0x025200D4);
v10 = gabi::load<u8>(v3 + 0x00005BB3u);
less=((int32_t)v10)<(0);greater=((int32_t)v10)>(0);equal=((int32_t)v10)==(0);
if (equal) { goto L_025F8138; }
goto L_025F812C;
L_025F80E0: ;
v0 = 0u + 0x00000001u;
v3 = v30;
v4 = 0u + 0x000005ACu;
v5 = v1 + 0x00000008u;
gabi::store<u8>(v30 + 0x00000920u, v0);
v3=gabi::call<u32>(0x025F7980,v3,v4,v5);
v31 = v3;
v4 = 0u + 0x00000002u;
v3 = v30;
v3=gabi::call<u32>(0x025F74D0,v3,v4);
goto L_025F8138;
L_025F810C: ;
v0 = 0u + 0x00000001u;
v3 = v30;
v4 = 0u + 0x000005ACu;
v5 = v1 + 0x00000008u;
gabi::store<u8>(v30 + 0x00000920u, v0);
v3=gabi::call<u32>(0x025F7980,v3,v4,v5);
v31 = v3;
goto L_025F8138;
L_025F812C: ;
v3 = v30;
v4 = 0u + 0x00000000u;
v3=gabi::call<u32>(0x025F74D0,v3,v4);
L_025F8138: ;
v3 = v31;
return v3;
return v3;
}
VERIFY(0x025F8088,mesgroute_025F8088);
u32 mesgroute_025F8154(u32 a3,u32 a4) {
WWHD_FUNC(0x025F8154,u32,a3,a4);
gabi::Local<u8[88]> frame;
u32 v0=0,v1=gabi::ea(frame.get()),v3=a3,v4=a4,v5=0,v6=0,v7=0,v8=0,v9=0,v10=0,v11=0,v12=0,v25=0,v26=0,v27=0,v28=0,v29=0,v30=0,v31=0;

bool less=false,greater=false,equal=false;u32 count=0;u8 carry=0;
v28 = v3;
v25 = v4;
v3=gabi::call<u32>(0x025200D4);
v0 = 0u + 0x000000FFu;
gabi::store<u8>(v3 + 0x00005BC6u, v0);
v3=gabi::call<u32>(0x025200D4);
v30 = 0u + 0xFFFFFFFFu;
gabi::store<u32>(v3 + 0x00005C30u, v30);
v3 = v28;
v3=gabi::call<u32>(0x025F795C);
less=(v3)<(0x0000u);greater=(v3)>(0x0000u);equal=(v3)==(0x0000u);
if (equal) { goto L_025F819C; }
less=(v3)<(0x000Fu);greater=(v3)>(0x000Fu);equal=(v3)==(0x000Fu);
if (!equal) { goto L_025F8370; }
L_025F819C: ;
v5 = v1 + 0x0000001Cu;
v7 = 0u + 0x100E0000u;
gabi::store<u32>(v1 + 0x00000010u, v5);
v29 = 0u + 0x100E0000u;
v27 = 0u + 0x00000000u;
v29 = v29 + 0x00000AE4u;
v7 = v7 + 0x00000BA4u;
gabi::store<u8>(v1 + 0x0000003Bu, v27);
v0 = 0u + 0x00000020u;
v4 = 0u + 0x100E0000u;
gabi::store<u8>(v1 + 0x0000001Cu, v27);
v3 = v1 + 0x00000010u;
v4 = v4 + 0x00000C90u;
gabi::store<u32>(v1 + 0x00000014u, v7);
v5 = v25 & 0x0000FFFFu;
gabi::store<u32>(v1 + 0x00000018u, v0);
v3=gabi::call<u32>(0x02759C28,v3,v4,v5);
v12 = 0u + 0x10490000u;
{ uint32_t ea = v12 + 0xFFFFD77Cu; v8 = gabi::load<u32>(ea); v12 = ea; }
v31 = 0u + 0x10490000u;
less=((int32_t)v8)<(0);greater=((int32_t)v8)>(0);equal=((int32_t)v8)==(0);
v31 = v31 + 0xFFFFD74Cu;
if (equal) { goto L_025F8228; }
v3 = 0u + 0x101F0000u;
v4 = v28 + 0x00000014u;
v3 = gabi::load<u32>(v3 + 0x00004AE8u);
v5 = v31;
v6 = v1 + 0x00000010u;
v3=gabi::call<u32>(0x025F50E8,v3,v4,v5,v6);
v31 = gabi::load<u32>(v28 + 0x00000240u);
less=((int32_t)v31)<(0);greater=((int32_t)v31)>(0);equal=((int32_t)v31)==(0);
v29 = gabi::load<u32>(v28 + 0x0000012Cu);
if (equal) { goto L_025F8370; }
goto L_025F8320;
L_025F8228: ;
gabi::store<u32>(v1 + 0x0000000Cu, v29);
v9 = 0u + 0x00000001u;
gabi::store<u8>(v31 + 0x00000011u, v27);
v0 = 0u + 0x100E0000u;
gabi::store<u32>(v12 + 0x00000000u, v9);
{ uint64_t t = (uint64_t)v0 + 0x00000BECu; v0 = (uint32_t)t; carry = (uint8_t)(t >> 32); }
v12 = 0u + 0x00000006u;
gabi::store<u32>(v31 + 0x00000004u, v0);
v0 = 0u + 0x100E0000u;
v26 = v31 + 0x0000000Cu;
gabi::store<u32>(v31 + 0x00000008u, v12);
{ uint64_t t = (uint64_t)v0 + 0x00000C88u; v0 = (uint32_t)t; carry = (uint8_t)(t >> 32); }
gabi::store<u32>(v31 + 0x00000000u, v26);
v3 = v1 + 0x00000008u;
gabi::store<u32>(v1 + 0x00000008u, v0);
v3=gabi::call<u32>(0x025F8F80,v3);
v11 = gabi::load<u32>(v1 + 0x00000008u);
v0 = gabi::load<u8>(v11 + 0x00000000u);
v5 = 0u + 0x00040000u;
less=((int32_t)v0)<(0);greater=((int32_t)v0)>(0);equal=((int32_t)v0)==(0);
v29 = 0u + 0x00000000u;
if (equal) { goto L_025F82A4; }
L_025F8280: ;
v29 = v29 + 0x00000001u;
less=((int32_t)v29)<((int32_t)v5);greater=((int32_t)v29)>((int32_t)v5);equal=((int32_t)v29)==((int32_t)v5);
v11 = v11 + 0x00000001u;
if (greater) { goto L_025F82A0; }
v0 = gabi::load<u8>(v11 + 0x00000000u);
less=((int32_t)v0)<(0);greater=((int32_t)v0)>(0);equal=((int32_t)v0)==(0);
if (!equal) { goto L_025F8280; }
goto L_025F82A4;
L_025F82A0: ;
v29 = 0u + 0x00000000u;
L_025F82A4: ;
v7 = gabi::load<u32>(v31 + 0x00000008u);
less=((int32_t)v29)<((int32_t)v7);greater=((int32_t)v29)>((int32_t)v7);equal=((int32_t)v29)==((int32_t)v7);
if (less) { goto L_025F82B4; }
v29 = v7 + 0xFFFFFFFFu;
L_025F82B4: ;
v10 = gabi::load<u32>(v1 + 0x0000000Cu);
v0 = gabi::load<u32>(v10 + 0x00000014u);
count = v0;
v3 = v1 + 0x00000008u;
v3=gabi::call_ptr<u32>(count,v3);
v4 = gabi::load<u32>(v1 + 0x00000008u);
v3 = v26;
v5 = v29;
v6 = 0u + 0x00000000u;
v3=gabi::call<u32>(0xC0009988,v3,v4,v5,v6);
v0 = 0u + 0x100E0000u;
v3 = 0u + 0x101F0000u;
gabi::store<u8>(v26 + v29, v27);
{ uint64_t t = (uint64_t)v0 + 0x00000C04u; v0 = (uint32_t)t; carry = (uint8_t)(t >> 32); }
v3 = v3 + 0x00004B2Cu;
gabi::store<u32>(v31 + 0x00000004u, v0);
v3=gabi::call<u32>(0x028F026C,v3);
v3 = 0u + 0x101F0000u;
v4 = v28 + 0x00000014u;
v3 = gabi::load<u32>(v3 + 0x00004AE8u);
v5 = v31;
v6 = v1 + 0x00000010u;
v3=gabi::call<u32>(0x025F50E8,v3,v4,v5,v6);
v31 = gabi::load<u32>(v28 + 0x00000240u);
less=((int32_t)v31)<(0);greater=((int32_t)v31)>(0);equal=((int32_t)v31)==(0);
v29 = gabi::load<u32>(v28 + 0x0000012Cu);
if (equal) { goto L_025F8370; }
L_025F8320: ;
v30 = gabi::load<u8>(v31 + 0x0000000Eu);
v3=gabi::call<u32>(0x025200D4);
gabi::store<u8>(v3 + 0x00005BC5u, v30);
v30 = gabi::load<u8>(v31 + 0x0000000Bu);
v3=gabi::call<u32>(0x025200D4);
gabi::store<u32>(v3 + 0x00005C34u, v30);
v31 = (uint32_t)(int32_t)(int16_t)gabi::load<u16>(v31 + 0x00000004u);
v3=gabi::call<u32>(0x025200D4);
v6 = v25;
v5 = v29;
gabi::store<u16>(v3 + 0x00005BA4u, v31);
v3 = gabi::load<u32>(v28 + 0x0000095Cu);
v4 = 0u + 0x00000002u;
gabi::store<u32>(v28 + 0x0000094Cu, v27);
v3=gabi::call<u32>(0x026B3F18,v3,v4,v5,v6);
v3 = v28;
v4 = 0u + 0x00000002u;
gabi::store<u32>(v28 + 0x00000938u, v25);
v3=gabi::call<u32>(0x025F74D0,v3,v4);
v30 = 0u + 0x00000000u;
L_025F8370: ;
v3 = v30;
return v3;
return v3;
}
VERIFY(0x025F8154,mesgroute_025F8154);
u32 mesgroute_025F8388(u32 a3,u32 a4,u32 a5,u32 a6,u32 a7) {
WWHD_FUNC(0x025F8388,u32,a3,a4,a5,a6,a7);
gabi::Local<u8[208]> frame;
u32 v0=0,v1=gabi::ea(frame.get()),v3=a3,v4=a4,v5=a5,v6=a6,v7=a7,v8=0,v9=0,v10=0,v11=0,v12=0,v21=0,v22=0,v23=0,v24=0,v25=0,v26=0,v27=0,v28=0,v29=0,v30=0,v31=0;

bool less=false,greater=false,equal=false;u32 count=0;u8 carry=0;
v23 = v3;
v24 = v4;
v25 = v5;
v26 = v6;
v27 = v7;
v28 = v1 + 0x00000014u;
gabi::store<u32>(v1 + 0x00000008u, v28);
v11 = 0u + 0x00000040u;
gabi::store<u32>(v1 + 0x00000010u, v11);
v12 = gabi::load<u32>(v24 + 0x00000004u);
v29 = 0u + 0x100E0000u;
v22 = 0u + 0x00000000u;
v29 = v29 + 0x00000B2Cu;
gabi::store<u8>(v1 + 0x00000053u, v22);
gabi::store<u32>(v1 + 0x0000000Cu, v29);
v0 = gabi::load<u32>(v12 + 0x00000014u);
count = v0;
v3 = v24;
v30 = 0u + 0xFFFFFFFFu;
v3=gabi::call_ptr<u32>(count,v3);
v10 = gabi::load<u32>(v24 + 0x00000000u);
v8 = gabi::load<u8>(v10 + 0x00000000u);
v31 = v22;
less=((int32_t)v8)<(0);greater=((int32_t)v8)>(0);equal=((int32_t)v8)==(0);
v12 = 0u + 0x00040000u;
if (equal) { goto L_025F8424; }
L_025F8400: ;
v31 = v31 + 0x00000001u;
less=((int32_t)v31)<((int32_t)v12);greater=((int32_t)v31)>((int32_t)v12);equal=((int32_t)v31)==((int32_t)v12);
v10 = v10 + 0x00000001u;
if (greater) { goto L_025F8420; }
v9 = gabi::load<u8>(v10 + 0x00000000u);
less=((int32_t)v9)<(0);greater=((int32_t)v9)>(0);equal=((int32_t)v9)==(0);
if (!equal) { goto L_025F8400; }
goto L_025F8424;
L_025F8420: ;
v31 = 0u + 0x00000000u;
L_025F8424: ;
v8 = gabi::load<u32>(v1 + 0x00000010u);
v10 = gabi::load<u32>(v24 + 0x00000004u);
less=((int32_t)v31)<((int32_t)v8);greater=((int32_t)v31)>((int32_t)v8);equal=((int32_t)v31)==((int32_t)v8);
v0 = gabi::load<u32>(v10 + 0x00000014u);
if (less) { goto L_025F843C; }
v31 = v8 + 0xFFFFFFFFu;
L_025F843C: ;
count = v0;
v3 = v24;
v3=gabi::call_ptr<u32>(count,v3);
v4 = gabi::load<u32>(v24 + 0x00000000u);
v6 = 0u + 0x00000000u;
v5 = v31;
v3 = v28;
v3=gabi::call<u32>(0xC0009988,v3,v4,v5,v6);
gabi::store<u8>(v28 + v31, v22);
v28 = v1 + 0x00000060u;
gabi::store<u32>(v1 + 0x00000058u, v29);
v9 = 0u + 0x00000040u;
gabi::store<u32>(v1 + 0x00000054u, v28);
v21 = 0u + 0x100E0000u;
v11 = gabi::load<u32>(v25 + 0x00000004u);
v21 = v21 + 0x00000B44u;
gabi::store<u32>(v1 + 0x0000005Cu, v9);
gabi::store<u32>(v1 + 0x0000000Cu, v21);
gabi::store<u8>(v1 + 0x0000009Fu, v22);
v0 = gabi::load<u32>(v11 + 0x00000014u);
count = v0;
v3 = v25;
v3=gabi::call_ptr<u32>(count,v3);
v12 = gabi::load<u32>(v25 + 0x00000000u);
v0 = gabi::load<u8>(v12 + 0x00000000u);
v31 = 0u + 0x00000000u;
less=((int32_t)v0)<(0);greater=((int32_t)v0)>(0);equal=((int32_t)v0)==(0);
v8 = 0u + 0x00040000u;
if (equal) { goto L_025F84D4; }
L_025F84B0: ;
v31 = v31 + 0x00000001u;
less=((int32_t)v31)<((int32_t)v8);greater=((int32_t)v31)>((int32_t)v8);equal=((int32_t)v31)==((int32_t)v8);
v12 = v12 + 0x00000001u;
if (greater) { goto L_025F84D0; }
v0 = gabi::load<u8>(v12 + 0x00000000u);
less=((int32_t)v0)<(0);greater=((int32_t)v0)>(0);equal=((int32_t)v0)==(0);
if (!equal) { goto L_025F84B0; }
goto L_025F84D4;
L_025F84D0: ;
v31 = 0u + 0x00000000u;
L_025F84D4: ;
v10 = gabi::load<u32>(v1 + 0x0000005Cu);
v12 = gabi::load<u32>(v25 + 0x00000004u);
less=((int32_t)v31)<((int32_t)v10);greater=((int32_t)v31)>((int32_t)v10);equal=((int32_t)v31)==((int32_t)v10);
v0 = gabi::load<u32>(v12 + 0x00000014u);
if (less) { goto L_025F84EC; }
v31 = v10 + 0xFFFFFFFFu;
L_025F84EC: ;
count = v0;
v3 = v25;
v3=gabi::call_ptr<u32>(count,v3);
v4 = gabi::load<u32>(v25 + 0x00000000u);
v3 = v28;
v5 = v31;
v6 = 0u + 0x00000000u;
v3=gabi::call<u32>(0xC0009988,v3,v4,v5,v6);
gabi::store<u8>(v28 + v31, v22);
v3 = v23 + 0x00000014u;
v4 = v24;
v5 = v25;
gabi::store<u32>(v1 + 0x00000058u, v21);
v3=gabi::call<u32>(0x025F6D70,v3,v4,v5);
v3 = 0u + 0x101F0000u;
v4 = v23 + 0x00000014u;
v3 = gabi::load<u32>(v3 + 0x00004AE8u);
v5 = v1 + 0x00000008u;
v6 = v1 + 0x00000054u;
v3=gabi::call<u32>(0x025F50E8,v3,v4,v5,v6);
v31 = gabi::load<u32>(v23 + 0x00000240u);
v0 = 0u + 0xFFFFFFF0u;
less=((int32_t)v31)<(0);greater=((int32_t)v31)>(0);equal=((int32_t)v31)==(0);
gabi::store<u32>(v23 + 0x00000130u, v0);
if (equal) { goto L_025F8590; }
v30 = gabi::load<u8>(v31 + 0x0000000Eu);
v3=gabi::call<u32>(0x025200D4);
gabi::store<u8>(v3 + 0x00005BC5u, v30);
v30 = gabi::load<u8>(v31 + 0x0000000Bu);
v3=gabi::call<u32>(0x025200D4);
gabi::store<u32>(v3 + 0x00005C34u, v30);
v31 = (uint32_t)(int32_t)(int16_t)gabi::load<u16>(v31 + 0x00000004u);
v3=gabi::call<u32>(0x025200D4);
v4 = v26;
v10 = 0u + 0x00000005u;
gabi::store<u16>(v3 + 0x00005BA4u, v31);
v3 = gabi::load<u32>(v23 + 0x00000960u);
v5 = v27;
gabi::store<u32>(v23 + 0x0000094Cu, v10);
v3=gabi::call<u32>(0x026AE330,v3,v4,v5);
v30 = 0u + 0x00000000u;
L_025F8590: ;
v3 = v30;
return v3;
return v3;
}
VERIFY(0x025F8388,mesgroute_025F8388);
