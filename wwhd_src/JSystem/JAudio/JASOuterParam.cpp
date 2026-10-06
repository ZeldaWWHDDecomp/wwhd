#include "gabi.h"
using namespace gabi;
void* outerCtor(void* obj){WWHD_FUNC(0x02816BE0,void*,obj);u32 a=ea(obj);if(!a)a=call<u32>(0x0273AD10,44);if(!a)return nullptr;u32 zero=load<u32>(0x10170520);store<u16>(a,0);store<u16>(a+2,0);for(u32 i=4;i<=24;i+=4)store<u32>(a+i,zero);for(u32 i=0;i<8;i++)store<u16>(a+28+i*2,0);return at<void>(a);}
VERIFY(0x02816BE0,outerCtor);
void outerInit(u32 a){WWHD_FUNC(0x02816C58,void,a);store<u16>(a,0);store<u16>(a+2,0);}
VERIFY(0x02816C58,outerInit);
void outerSetSwitch(u32 a,u32 flags){WWHD_FUNC(0x02816C68,void,a,flags);store<u16>(a,(u16)flags);}
VERIFY(0x02816C68,outerSetSwitch);
u32 outerCheckSwitch(u32 a,u32 flags){WWHD_FUNC(0x02816C70,u32,a,flags);return (load<u16>(a)&flags)!=0;}
VERIFY(0x02816C70,outerCheckSwitch);
void outerSetUpdate(u32 a,u32 flags){WWHD_FUNC(0x02816C84,void,a,flags);store<u16>(a+2,(u16)flags);}
VERIFY(0x02816C84,outerSetUpdate);
u32 outerGetUpdate(u32 a){WWHD_FUNC(0x02816C8C,u32,a);return load<u16>(a+2);}
VERIFY(0x02816C8C,outerGetUpdate);
s32 outerGetFir(u32 a,u32 index){WWHD_FUNC(0x02816C94,s32,a,index);return (s16)load<u16>(a+28+index*2);}
VERIFY(0x02816C94,outerGetFir);
void outerSetParam(u32 a,u32 flag,f32 value){WWHD_FUNC(0x02816CA4,void,a,flag,value);u32 offset;switch(flag){case 1:offset=4;break;case 2:offset=8;break;case 4:offset=12;break;case 8:offset=20;break;case 16:offset=16;break;case 64:offset=24;break;default:return;}u16 update=load<u16>(a+2);store<f32>(a+offset,value);store<u16>(a+2,(u16)(update|flag));}
VERIFY(0x02816CA4,outerSetParam);
void outerOnSwitch(u32 a,u32 flags){WWHD_FUNC(0x02816D60,void,a,flags);u16 state=load<u16>(a),update=load<u16>(a+2);store<u16>(a,(u16)(state|flags));store<u16>(a+2,(u16)(update|flags));}
VERIFY(0x02816D60,outerOnSwitch);
void outerSetFir(u32 a,u32 input){WWHD_FUNC(0x02816D7C,void,a,input);u16 update=load<u16>(a+2),state=load<u16>(a);store<u16>(a+2,(u16)(update|128));store<u16>(a,(u16)(state|128));for(u32 i=0;i<8;i++)store<u16>(a+28+i*2,load<u16>(input+i*2));}
VERIFY(0x02816D7C,outerSetFir);
