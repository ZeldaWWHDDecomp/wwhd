/* Salvatore (Squid-Hunt), reconstructed from WWHD. */
#include "d/actor/d_a_npc_kg1.h"

template<class T> static T readAt(u32 p,u32 off) { return gabi::load<T>(p+off); }
template<class T> static void writeAt(u32 p,u32 off,T v) { gabi::store<T>(p+off,v); }
static u32 playObject() { return gabi::call<u32>(0x025200D4); }
static u32 modelOf(daNpc_Kg1_c* a) { return readAt<u32>(readAt<u32>(gabi::ea(a),0x44C),0x90); }
static void copyFloatMatrix(u32 dst,u32 src) {
    f32 values[12];
    for(int i=0;i<12;++i) values[i]=gabi::load<f32>(src+4*i);
    for(int i=0;i<12;++i) gabi::store<f32>(dst+4*i,values[i]);
}

void daNpc_Kg1_clr_seq_flag(daNpc_Kg1_c* a) {
    WWHD_FUNC(0x02265DC8,void,a);
    a->mSequenceFlags[5]=0; a->mSequenceFlags[1]=0; a->mSequenceFlags[4]=0;
    a->mSequenceFlags[3]=0; a->mSequenceFlags[2]=0; a->mSequenceFlags[0]=0;
}
VERIFY(0x02265DC8,daNpc_Kg1_clr_seq_flag);
void daNpc_Kg1_wait_action_init(daNpc_Kg1_c* a) {
    WWHD_FUNC(0x02265DE8,void,a);
    daNpc_Kg1_clr_seq_flag(a);
    a->mAction.i=-1; a->mAction.f=0x02266D60; a->mAction.d=0;
}
VERIFY(0x02265DE8,daNpc_Kg1_wait_action_init);
BOOL daNpc_Kg1IsDelete(daNpc_Kg1_c* a) { WWHD_FUNC(0x02267AB8,BOOL,a); return TRUE; }
VERIFY(0x02267AB8,daNpc_Kg1IsDelete);
BOOL daNpc_Kg1_CheckCreateHeap(daNpc_Kg1_c* a) { WWHD_FUNC(0x02265CC8,BOOL,a); return gabi::call<BOOL>(0x02265978,a); }
VERIFY(0x02265CC8,daNpc_Kg1_CheckCreateHeap);
void daNpc_Kg1_texVirtualEmpty(void* a) { WWHD_FUNC(0x02267B5C,void,a); }
VERIFY(0x02267B5C,daNpc_Kg1_texVirtualEmpty);
void daNpc_Kg1_hio_delete(void* self,u32 flags) {
    WWHD_FUNC(0x02267AA4,void,self,flags);
    if(self && (flags&1)) gabi::call(0x0273AF40,self);
}
VERIFY(0x02267AA4,daNpc_Kg1_hio_delete);
void daNpc_Kg1_delete(daNpc_Kg1_c* a,u32 flags) {
    WWHD_FUNC(0x02267AC0,void,a,flags);
    if(!a) return;
    u32 p=gabi::ea(a);
    gabi::call(0x02515A70,p+0x690,2);
    gabi::call(0x02515860,p+0x654,2);
    gabi::call(0x02018034,p+0x628,2);
    writeAt<u32>(p,0x470,0x1001BC5C); writeAt<u32>(p,0x464,0x1001BC6C);
    gabi::call(0x024EFD9C,p+0x450,0);
    gabi::call(0x025D50BC,a,0);
    if(flags&1) gabi::call(0x0273AF40,a);
}
VERIFY(0x02267AC0,daNpc_Kg1_delete);
BOOL daNpc_Kg1Delete(daNpc_Kg1_c* a) {
    WWHD_FUNC(0x02266320,BOOL,a);
    gabi::call(0x025204C8,&a->mPhs,STR(0x1001BF4C));
    u32 p=gabi::ea(a);
    if(readAt<u32>(p,0xF4)) {
        u32 morf=readAt<u32>(p,0x44C);
        if(morf) gabi::call(0x025E563C,morf);
    }
    s32 count=gabi::load<s32>(0x1046759C);
    if(count>=0) {
        --count; gabi::store<s32>(0x1046759C,count);
        if(count<0) gabi::call(0x025F0A18,gabi::load<s8>(0x10467598));
    }
    return TRUE;
}
VERIFY(0x02266320,daNpc_Kg1Delete);
void daNpc_Kg1_anmAtr(daNpc_Kg1_c* a,u16 attr) {
    WWHD_FUNC(0x02266C34,void,a,attr);
    if(attr==6) {
        u8 requested=gabi::load<u8>(playObject()+0x5BC5);
        if(requested<9) {
            a->mRequestedAnimation=gabi::load<s8>(0x1001BEB8+requested);
            gabi::store<u8>(playObject()+0x5BC5,0xFF);
        }
    }
}
VERIFY(0x02266C34,daNpc_Kg1_anmAtr);
void daNpc_Kg1_playTexPatternAnm(daNpc_Kg1_c* a) {
    WWHD_FUNC(0x022665C8,void,a);
    if(gabi::call<s16>(0x02055B64,&a->mBlinkTimer)!=0) return;
    u32 anm=gabi::ea(a->mpTexPattern.get());
    u32 vt=readAt<u32>(anm,4);
    s32 max=gabi::call_ptr<s32>(readAt<u32>(vt,0x14),anm);
    if((s32)a->mBtpFrame>=max) {
        anm=gabi::ea(a->mpTexPattern.get()); vt=readAt<u32>(anm,4);
        max=gabi::call_ptr<s32>(readAt<u32>(vt,0x14),anm);
        u8 frame=a->mBtpFrame;
        a->mBlinkTimer=120; a->mBtpFrame=(u8)(frame-max);
    } else a->mBtpFrame=(u8)(a->mBtpFrame+1);
}
VERIFY(0x022665C8,daNpc_Kg1_playTexPatternAnm);
void daNpc_Kg1_checkOrder(daNpc_Kg1_c* a) {
    WWHD_FUNC(0x022663A4,void,a);
    u16 condition=readAt<u16>(gabi::ea(a),0xF8);
    if(condition==2) {
        s16 event=a->mEventIds[0]; u32 play=playObject();
        if(gabi::call<s32>(0x0254407C,play+0x52C4,event) && a->mEventOrder==3) a->mEventOrder=0;
        event=a->mEventIds[0]; play=playObject();
        if(gabi::call<s32>(0x025440C8,play+0x52C4,event)) a->mEventOrder=0;
        event=a->mEventIds[2]; play=playObject();
        if(gabi::call<s32>(0x0254407C,play+0x52C4,event) && a->mEventOrder==5) a->mEventOrder=0;
    } else if(condition==1 && (a->mEventOrder==2 || a->mEventOrder==1)) a->mTalkActive=1;
}
VERIFY(0x022663A4,daNpc_Kg1_checkOrder);
void daNpc_Kg1_eventOrder(daNpc_Kg1_c* a) {
    WWHD_FUNC(0x022664A4,void,a);
    u32 p=gabi::ea(a); u8 order=a->mEventOrder;
    if(order==1 || order==2) {
        u16 flags=readAt<u16>(p,0xFA); order=a->mEventOrder;
        writeAt<u16>(p,0xFA,flags|1);
        if(order==2) gabi::call(0x025D76A8,a);
    } else if(order>=3 && order<=5) {
        s16 id=a->mEventIds[order-3];
        gabi::call(0x025D7A58,a,id,0xFF,0xFFFF,0,1);
        writeAt<u16>(p,0xFA,readAt<u16>(p,0xFA)|2);
    }
}
VERIFY(0x022664A4,daNpc_Kg1_eventOrder);

static u32 objectResource(s32 id) {
    struct ResourceName_l { u8 padding[8]; SafeString name; };
    gabi::Local<ResourceName_l> storage; SafeString* name=&storage->name;
    name->mStringTop=0x1001BF4C; name->__vtbl=0x1001BC44;
    u32 control=gabi::load<u32>(0x101F4F28);
    return gabi::call<u32>(0x026066C4,control,name,id);
}
BOOL daNpc_Kg1_initTexPatternAnm(daNpc_Kg1_c* a,s32 index,u32 modify) {
    WWHD_FUNC(0x0226586C,BOOL,a,index,modify);
    u32 model=modelOf(a); u32 data=readAt<u32>(model,0xAC);
    s32 resource=gabi::load<s32>(0x1001BC8C+4*(u32)index);
    u32 pattern=objectResource(resource); a->mpTexPattern=gabi::at<J3DAnmTexPattern>(pattern);
    if(!pattern) {
        gabi::call(0x0273AA24,STR(0x1001BCBC),0x1CF,STR(0x1001BCCC));
        pattern=gabi::ea(a->mpTexPattern.get());
    }
    if(!gabi::call<s32>(0x025E789C,a->mBtpAnm,data,pattern,1,2,1.0f,0,-1,(u32)modify,0)) return FALSE;
    a->mBtpFrame=0; a->mBlinkTimer=0; return TRUE;
}
VERIFY(0x0226586C,daNpc_Kg1_initTexPatternAnm);
void daNpc_Kg1_set_mtx(daNpc_Kg1_c* a) {
    WWHD_FUNC(0x02265CCC,void,a);
    u32 p=gabi::ea(a); u32 model=modelOf(a);
    f32 x=readAt<f32>(p,0x330),y=readAt<f32>(p,0x334),z=readAt<f32>(p,0x338);
    writeAt<f32>(model,0xBC,x); writeAt<f32>(model,0xC0,y); writeAt<f32>(model,0xC4,z);
    x=readAt<f32>(p,0x314); y=readAt<f32>(p,0x318); z=readAt<f32>(p,0x31C);
    gabi::call(0x028E93CC,0x1048D0CC,x,y,z);
    gabi::call(0x025F1C28,0x1048D0CC,readAt<s16>(p,0x322));
    model=modelOf(a); copyFloatMatrix(model+0xC8,0x1048D0CC);
    model=gabi::ea(a->mpPropModel.get());
    x=readAt<f32>(p,0x330); y=readAt<f32>(p,0x334); z=readAt<f32>(p,0x338);
    writeAt<f32>(model,0xBC,x); writeAt<f32>(model,0xC0,y); writeAt<f32>(model,0xC4,z);
}
VERIFY(0x02265CCC,daNpc_Kg1_set_mtx);
BOOL daNpc_Kg1_chkAttention(daNpc_Kg1_c* a) {
    WWHD_FUNC(0x02266668,BOOL,a);
    u32 p=gabi::ea(a), player=readAt<u32>(playObject(),0x5B2C);
    gabi::Local<cXyz> difference, horizontal;
    gabi::call(0x0201ADE0,player+0x314,difference.get(),p+0x314);
    horizontal->x=difference->x; horizontal->y=0.0f; horizontal->z=difference->z;
    f32 distance=gabi::call<f32>(0x028E8DD0,horizontal.get());
    distance=gabi::call<f32>(0x028F4384,distance);
    f32 radius=gabi::load<f32>(0x104675C4);
    s16 angleLimit=gabi::load<s16>(0x104675C0);
    if(!(distance<radius)) return FALSE;
    f32 x=readAt<f32>(player,0x314)-readAt<f32>(p,0x314);
    f32 z=readAt<f32>(player,0x31C)-readAt<f32>(p,0x31C);
    s32 angle=gabi::call<s32>(0x020195B0,x,z);
    s32 facing=readAt<s16>(p,0x322)+readAt<s16>(p,0x3AE)+readAt<s16>(p,0x3B2);
    s16 delta=(s16)(angle-facing);
    s16 magnitude=(s16)(delta<0?-(s32)delta:delta);
    return magnitude<angleLimit;
}
VERIFY(0x02266668,daNpc_Kg1_chkAttention);
void daNpc_Kg1_kg1_talk_camera(daNpc_Kg1_c* a) {
    WWHD_FUNC(0x02266C90,void,a);
    s8 cameraIndex=readAt<s8>(playObject(),0x5B30);
    u32 play=playObject();
    u8 enabled=readAt<u8>(gabi::ea(a),0x97D);
    u32 camera=readAt<u32>(play,(u32)((s32)cameraIndex*0x34)+0x5AF8);
    if(!enabled || !camera) return;
    gabi::call(0x02514F44,camera+0x248);
    gabi::Local<cXyz> center,eye;
    center->x=gabi::load<f32>(0x104675F4); center->y=gabi::load<f32>(0x104675F8); center->z=gabi::load<f32>(0x104675FC);
    eye->x=gabi::load<f32>(0x10467600); eye->y=gabi::load<f32>(0x10467604); eye->z=gabi::load<f32>(0x10467608);
    gabi::call(0x02514F88,camera+0x248,center.get(),eye.get(),0,40.0f);
    gabi::call(0x02515048,camera+0x248);
    gabi::call(0x02515280,camera+0x248,1);
}
VERIFY(0x02266C90,daNpc_Kg1_kg1_talk_camera);
BOOL daNpc_Kg1Draw(daNpc_Kg1_c* a) {
    WWHD_FUNC(0x02266B24,BOOL,a);
    u32 p=gabi::ea(a);
    gabi::call(0x025BED80,0x81,a,1.0f,1.0f,1.0f);
    u32 model=modelOf(a),data=readAt<u32>(model,0xAC);
    u32 environment=gabi::call<u32>(0x02555D0C);
    gabi::call(0x025626A4,environment,0,p+0x314,p+0x110);
    u32 morf=readAt<u32>(p,0x44C);
    environment=gabi::call<u32>(0x02555D0C);
    model=readAt<u32>(morf,0x90);
    gabi::call(0x02562F5C,environment,model,p+0x110);
    environment=gabi::call<u32>(0x02555D0C);
    gabi::call(0x02562F5C,environment,a->mpPropModel.get(),p+0x110);
    gabi::call(0x025E7B3C,a->mBtpAnm,data,(s32)a->mBtpFrame);
    gabi::call(0x025E54D8,readAt<u32>(p,0x44C));
    writeAt<u32>(data,0x38,0);
    if(a->mSpecialAnimation) {
        s16 frame=(s16)gabi::ftoi(readAt<f32>(p,0x7E4));
        u32 prop=gabi::ea(a->mpPropModel.get());
        gabi::call(0x025E7B3C,p+0x7E0,readAt<u32>(prop,0xAC),frame);
        gabi::call(0x025E2DE0,a->mpPropModel.get(),0);
    }
    if(readAt<u8>(p,0x9A5)) writeAt<u8>(playObject(),0x5BBA,25);
    return TRUE;
}
VERIFY(0x02266B24,daNpc_Kg1Draw);

static u32 jointNameTable(u32 modelData) {
    u32 header=gabi::call<u32>(0x027F68FC,modelData);
    u32 offset=readAt<u32>(header,0x10);
    return offset?header+0x10+offset:0;
}
BOOL daNpc_Kg1_CreateHeap(daNpc_Kg1_c* a) {
    WWHD_FUNC(0x02265978,BOOL,a);
    u32 p=gabi::ea(a),data=objectResource(5),animation=objectResource(17);
    u32 morf=gabi::call<u32>(0x025E4F64,0,data,0,0,animation,2,0,-1,1.0f,1,0,0x80000,0x11020022);
    writeAt<u32>(p,0x44C,morf);
    if(!morf || !readAt<u32>(morf,0x90)) return FALSE;
    u32 names=jointNameTable(data);
    s8 head=gabi::call<s8>(0x027DF9B0,names,STR(0x1001BCEC));
    writeAt<s8>(p,0x3B4,head);
    if(head<0) gabi::call(0x0273AA24,STR(0x1001BCF4),0x227,STR(0x1001BD10));
    names=jointNameTable(data);
    s8 spine=gabi::call<s8>(0x027DF9B0,names,STR(0x1001BD04));
    writeAt<s8>(p,0x3B5,spine);
    if(spine<0) gabi::call(0x0273AA24,STR(0x1001BCF4),0x22C,STR(0x1001BD2C));
    if(!daNpc_Kg1_initTexPatternAnm(a,3,0)) return FALSE;
    daNpc_Kg1_initTexPatternAnm(a,0,1);
    u32 prop=gabi::call<u32>(0x025E38E0,objectResource(6),0x80000,0x11020022);
    a->mpPropModel=gabi::at<J3DModel>(prop);
    if(!prop) return FALSE;
    u32 propData=objectResource(6),propPattern=objectResource(10);
    if(!gabi::call<s32>(0x025E789C,p+0x7E0,propData,propPattern,1,2,1.0f,0,-1,0,0)) return FALSE;
    u16 index=0;
    u32 jointHeader=gabi::call<u32>(0x027F3F94,data);
    while(index<readAt<u16>(jointHeader,8)) {
        if((u32)index==(u32)readAt<s8>(p,0x3B4) || (u32)index==(u32)readAt<s8>(p,0x3B5) || index==8) {
            u32 count=readAt<u32>(data,4),joints=readAt<u32>(data,8);
            if(index<count) joints+=(u32)index*0x1C;
            writeAt<u32>(joints,8,0x0226559C);
        }
        index=(u16)(index+1); jointHeader=gabi::call<u32>(0x027F3F94,data);
    }
    u32 model=modelOf(a); writeAt<u32>(model,0xB8,p);
    gabi::call(0x024EFF44,p+0x614,30.0f,30.0f);
    gabi::call(0x024F06B4,p+0x450,p+0x314,p+0x300,a,1,p+0x614,p+0x33C,p+0x320,p+0x328);
    return TRUE;
}
VERIFY(0x02265978,daNpc_Kg1_CreateHeap);


void* daNpc_Kg1_HIO_ctor(void* self) {
    WWHD_FUNC(0x022678B8,void*,self);
    u32 p=gabi::ea(self);
    if(!p) { p=gabi::call<u32>(0x0273AD10,0x3C); if(!p) return nullptr; }
    writeAt<u32>(p,0x38,0x1001BC7C);
    gabi::call(0x028EFFD0,p+0xC,1,0x28,0x0259DA18);
    writeAt<u8>(p,0x2A,0); writeAt<s16>(p,0x14,7000); writeAt<u8>(p,8,0);
    writeAt<s8>(p,0,-1); writeAt<s16>(p,0x16,8000); writeAt<s32>(p,4,-1);
    writeAt<s16>(p,0x18,-2500); writeAt<f32>(p,0xC,0.0f); writeAt<s16>(p,0x22,2000);
    writeAt<s16>(p,0x1E,-8000); writeAt<f32>(p,0x24,35.0f); writeAt<u8>(p,0x35,0);
    writeAt<s16>(p,0x28,0x4000); writeAt<s16>(p,0x12,2000); writeAt<f32>(p,0x2C,400.0f);
    writeAt<s16>(p,0x10,2500); writeAt<s16>(p,0x20,1000); writeAt<u8>(p,0x34,0);
    writeAt<s16>(p,0x1A,-2000); writeAt<s16>(p,0x1C,-7000);
    return gabi::at<void>(p);
}
VERIFY(0x022678B8,daNpc_Kg1_HIO_ctor);
void daNpc_Kg1_sinit() {
    WWHD_FUNC(0x022679B4,void);
    gabi::store<u32>(0x10467594,0); gabi::store<u32>(0x10467590,0);
    gabi::store<u32>(0x1046758C,0); gabi::store<u32>(0x10467588,0);
    gabi::call(0x028F026C,0x101BF0D8);
    gabi::store<f32>(0x1046757C,gabi::load<f32>(0x1001BF28));
    gabi::store<f32>(0x10467580,gabi::load<f32>(0x1001BF2C));
    gabi::call(0x028ED6F8,0x10467584); gabi::call(0x028F026C,0x101BF0E4);
    gabi::call(0x028EAB2C,0x10467585); gabi::call(0x028F026C,0x101BF0F0);
    daNpc_Kg1_HIO_ctor(gabi::at<void>(0x10467598));
    f32 cx=gabi::load<f32>(0x1001BF30),ez=gabi::load<f32>(0x1001BF44),ex=gabi::load<f32>(0x1001BF3C);
    f32 cy=gabi::load<f32>(0x1001BF34);
    gabi::store<f32>(0x10467600,ex); gabi::store<f32>(0x104675F4,cx); gabi::store<f32>(0x104675F8,cy);
    f32 ey=gabi::load<f32>(0x1001BF40),cz=gabi::load<f32>(0x1001BF38);
    gabi::store<f32>(0x10467604,ey); gabi::store<f32>(0x10467608,ez); gabi::store<f32>(0x104675FC,cz);
}
VERIFY(0x022679B4,daNpc_Kg1_sinit);

s32 daNpc_Kg1_CreateInit(daNpc_Kg1_c* a) {
    WWHD_FUNC(0x022660C4,s32,a);
    u32 p=gabi::ea(a);
    gabi::call(0x02515F14,p+0x654,0xFF,0xFF,a);
    gabi::call(0x02516518,p+0x690,0x101BF094);
    writeAt<u32>(p,0x6D4,p+0x654);
    gabi::call(0x025A15AC,a,60.0f,150.0f);
    writeAt<f32>(p,0x374,-9.0f);
    gabi::call(0x0259F814,p+0x3E0,STR(0x1001BE6C),a);
    daNpc_Kg1_set_mtx(a);
    a->mTalkActive=0; writeAt<s16>(p,0x958,0); writeAt<u8>(p,0x95B,0);
    u32 model=modelOf(a); writeAt<u32>(p,0x348,model?model+0xC8:0);
    daNpc_Kg1_wait_action_init(a);
    s32 users=gabi::load<s32>(0x1046759C);
    if(users<0) {
        s32 child=gabi::call<s32>(0x025F0A10,STR(0x1001BE74),0x10467598);
        users=gabi::load<s32>(0x1046759C); gabi::store<s8>(0x10467598,(s8)child);
    }
    gabi::store<s32>(0x1046759C,users+1); writeAt<u32>(p,0x39C,10);
    u32 play=playObject(); a->mEventIds[0]=gabi::call<s16>(0x02543F10,play+0x52C4,STR(0x1001BE84),0xFF);
    play=playObject(); a->mEventIds[1]=gabi::call<s16>(0x02543F10,play+0x52C4,STR(0x1001BE94),0xFF);
    play=playObject(); s16 finalId=gabi::call<s16>(0x02543F10,play+0x52C4,STR(0x1001BEA4),0xFF);
    a->mPreviousAnimation=0; a->mEventIds[2]=finalId; a->mRequestedAnimation=2;
    gabi::call(0x02265E24,a);
    u32 y=readAt<u32>(p,0x318),x=readAt<u32>(p,0x314);
    writeAt<u8>(p,0x3B8,1); writeAt<u32>(p,0x960,x); writeAt<u8>(p,0x97C,1); writeAt<u32>(p,0x964,y);
    u32 z=readAt<u32>(p,0x31C); writeAt<u8>(p,0x3B7,1); writeAt<u32>(p,0x968,z);
    return 4;
}
VERIFY(0x022660C4,daNpc_Kg1_CreateInit);
s32 daNpc_Kg1Create(daNpc_Kg1_c* a) {
    WWHD_FUNC(0x02266268,s32,a);
    u32 p=gabi::ea(a),flags=readAt<u32>(p,0x2E4);
    if(!(flags&8)) {
        if(a) {
            gabi::call(0x025A1458,a); writeAt<u32>(p,0xB4,0x1001BF50);
            gabi::call(0x025E7820,p+0x7E0); gabi::call(0x025E7820,p+0x86C); gabi::call(0x025E7820,p+0x8E0);
            flags=readAt<u32>(p,0x2E4);
        }
        writeAt<u32>(p,0x2E4,flags|8);
    }
    s32 phase=gabi::call<s32>(0x02520460,&a->mPhs,STR(0x1001BF4C));
    if(phase==4) {
        if(gabi::call<s32>(0x025D63E8,a,0x02265CC8,0x10000)) return daNpc_Kg1_CreateInit(a);
        return 5;
    }
    return phase;
}
VERIFY(0x02266268,daNpc_Kg1Create);
static void copyVectorWords(u32 dest,u32 src) {
    u32 x=readAt<u32>(src,0),y=readAt<u32>(src,4),z=readAt<u32>(src,8);
    writeAt<u32>(dest,0,x); writeAt<u32>(dest,4,y); writeAt<u32>(dest,8,z);
}
void daNpc_Kg1_lookBack(daNpc_Kg1_c* a) {
    WWHD_FUNC(0x0226676C,void,a);
    u32 p=gabi::ea(a),player=readAt<u32>(playObject(),0x5B2C);
    gabi::Local<cXyz> difference,horizontal,target,eye,playerEye;
    gabi::call(0x0201ADE0,player+0x314,difference.get(),p+0x314);
    horizontal->x=difference->x; horizontal->y=0.0f; horizontal->z=difference->z;
    f32 distance=gabi::call<f32>(0x028E8DD0,horizontal.get()); distance=gabi::call<f32>(0x028F4384,distance);
    u32 targetAddress=0; bool immediate=false;
    if(readAt<u8>(p,0x440)) {
        f32 x=readAt<f32>(p,0x434),y=readAt<f32>(p,0x438);
        writeAt<u8>(p,0x3B6,1); writeAt<u8>(p,0x95B,1);
        target->x=x; target->y=y; target->z=readAt<f32>(p,0x43C); targetAddress=gabi::ea(target.get());
    } else {
        if(a->mRequestedAnimation==8) {
            copyVectorWords(gabi::ea(target.get()),0x10467600); targetAddress=gabi::ea(target.get());
        } else if(distance<gabi::load<f32>(0x104675C4)) {
            gabi::call(0x0259D54C,playerEye.get(),gabi::load<f32>(0x104675A4));
            copyVectorWords(gabi::ea(target.get()),gabi::ea(playerEye.get())); targetAddress=gabi::ea(target.get());
            BOOL attention=daNpc_Kg1_chkAttention(a); writeAt<u8>(p,0x95B,(u8)attention);
        } else writeAt<u8>(p,0x95B,0);
        immediate=readAt<u8>(p,0x3B6)==0;
    }
    if(immediate) {
        eye->x=readAt<f32>(p,0x37C); eye->y=readAt<f32>(p,0x380); eye->z=readAt<f32>(p,0x384);
        s16 facing=readAt<s16>(p,0x322); writeAt<s16>(p,0x958,0);
        gabi::call(0x0259DED0,p+0x3AC,p+0x322,targetAddress,eye.get(),facing,0,1);
        return;
    }
    s16 speed=readAt<s16>(p,0x442),defaultSpeed=gabi::load<s16>(0x104675BA);
    if(speed==0) speed=defaultSpeed;
    gabi::call(0x0200F428,p+0x958,speed,4,0x800);
    eye->x=readAt<f32>(p,0x37C); eye->y=readAt<f32>(p,0x380); eye->z=readAt<f32>(p,0x384);
    s16 facing=readAt<s16>(p,0x322),tracking=readAt<s16>(p,0x958);
    gabi::call(0x0259DED0,p+0x3AC,p+0x322,targetAddress,eye.get(),facing,tracking,1);
}
VERIFY(0x0226676C,daNpc_Kg1_lookBack);
BOOL daNpc_Kg1Execute(daNpc_Kg1_c* a) {
    WWHD_FUNC(0x02266990,BOOL,a);
    u32 p=gabi::ea(a);
    s16 params[9]; for(int i=0;i<9;++i) params[i]=gabi::load<s16>(0x104675A8+2*i);
    gabi::call(0x0259E08C,p+0x3AC,params[1],params[3],params[5],params[7],params[0],params[2],params[4],params[6],params[8]);
    daNpc_Kg1_checkOrder(a);
    s16 index=a->mAction.i,delta=a->mAction.d;
    u32 receiver=p+(s32)delta;
    if(index<0) gabi::call_ptr(a->mAction.f,receiver);
    else {
        s16 vtableOffset=readAt<s16>(p,0x862);
        u32 vtable=readAt<u32>(receiver,(u32)(s32)vtableOffset);
        gabi::call_ptr(readAt<u32>(vtable,(u32)(s32)index*8+4),receiver);
    }
    daNpc_Kg1_eventOrder(a); daNpc_Kg1_playTexPatternAnm(a); gabi::call(0x02265E24,a);
    if(readAt<u8>(p,0x97C)) gabi::call(0x025E535C,readAt<u32>(p,0x44C),0,0,0);
    u32 play=playObject(); gabi::call(0x024F08A8,p+0x450,play+0x12A0);
    gabi::call(0x025A15AC,a,60.0f,150.0f);
    f32 y=readAt<f32>(p,0x318),z=readAt<f32>(p,0x31C),x=readAt<f32>(p,0x314);
    writeAt<f32>(p,0x390,x); writeAt<f32>(p,0x384,z); writeAt<f32>(p,0x394,y+190.0f);
    writeAt<f32>(p,0x398,z); writeAt<f32>(p,0x380,y+150.0f); writeAt<f32>(p,0x37C,x);
    daNpc_Kg1_lookBack(a); daNpc_Kg1_set_mtx(a); return FALSE;
}
VERIFY(0x02266990,daNpc_Kg1Execute);

static u32 jointMatrix(u32 model,u32 index) {
    u32 block=readAt<u32>(model,0x2C),matrices=readAt<u32>(block,0x10);
    u16 flags=readAt<u16>(block,4); writeAt<u16>(block,4,flags|0x10);
    return matrices+index*0x30;
}
BOOL daNpc_Kg1_nodeCallBack(J3DNode* node,s32 stage) {
    WWHD_FUNC(0x0226559C,BOOL,node,stage);
    if(stage!=0) return TRUE;
    u32 model=gabi::load<u32>(0x104B462C),actor=readAt<u32>(model,0xB8);
    u32 joint=gabi::call<u32>(0x027F7878,node); u32 index=readAt<u16>(joint,4);
    constexpr u32 matrix=0x1048D0CC;
    gabi::call(0x028E90D4,jointMatrix(model,index),matrix);
    if(index==(u32)readAt<s8>(actor,0x3B4)) {
        gabi::call(0x025F1BF4,matrix,readAt<s16>(actor,0x3AE));
        gabi::call(0x025F1C5C,matrix,(s16)-readAt<s16>(actor,0x3AC));
        if(!gabi::load<u32>(0x104675EC)) {
            gabi::store<f32>(0x104675DC,0.0f); gabi::store<u32>(0x104675EC,1);
            gabi::store<f32>(0x104675D8,5.0f); gabi::store<f32>(0x104675D4,24.0f);
        }
        if(!gabi::load<u32>(0x104675F0)) {
            gabi::store<f32>(0x104675E8,0.0f); gabi::store<u32>(0x104675F0,1);
            gabi::store<f32>(0x104675E4,-16.0f); gabi::store<f32>(0x104675E0,24.0f);
        }
        gabi::call(0x028E8F64,matrix,0x104675D4,actor+0x96C);
        gabi::call(0x025F1BF4,matrix,readAt<s16>(actor,0x3AE));
        gabi::call(0x025F1C5C,matrix,(s16)-readAt<s16>(actor,0x3AC));
        gabi::call(0x028E8F64,matrix,0x104675E0,actor+0x960);
        gabi::call(0x028E8F64,matrix,0x104675D4,actor+0x390);
        writeAt<f32>(actor,0x394,readAt<f32>(actor,0x394)+gabi::load<f32>(0x104675BC));
    }
    if(index==(u32)readAt<s8>(actor,0x3B5)) {
        gabi::call(0x025F1BF4,matrix,readAt<s16>(actor,0x3B2));
        gabi::call(0x025F1C5C,matrix,(s16)-readAt<s16>(actor,0x3B0));
    }
    copyFloatMatrix(jointMatrix(model,index),matrix);
    gabi::call(0x028E90D4,matrix,0x104B4868);
    if(index==8) {
        gabi::call(0x025F24E0,23.46f,-22.26f,-47.05f);
        gabi::call(0x025F19F8,matrix,0x1F4B,(s16)0xB100,0x1F4B);
        u32 prop=readAt<u32>(actor,0x7DC); copyFloatMatrix(prop+0xC8,matrix);
    }
    return TRUE;
}
VERIFY(0x0226559C,daNpc_Kg1_nodeCallBack);
static u32 saveEvent(u32 offset=0x644) { return gabi::load<u32>(0x101F84DC)+offset; }
u32 daNpc_Kg1_getMsg(daNpc_Kg1_c* a) {
    WWHD_FUNC(0x02267498,u32,a);
    u32 p=gabi::ea(a);
    if(a->mSequenceFlags[2]) {
        if(!a->mSequenceFlags[1]) return 0x1D5D;
        if(!a->mSequenceFlags[3]) return 0x1D5F;
        if(!a->mSequenceFlags[4]) {
            u32 score=gabi::call<u32>(0x025B8BB0,saveEvent(),0xBEFF);
            if(score>readAt<u8>(p,0x9A6)) {
                gabi::call(0x025B8B68,saveEvent(),0xE04);
                gabi::call(0x025B8AF4,saveEvent(),0xBEFF,readAt<u8>(p,0x9A6));
                u32 wins=gabi::call<u32>(0x025B8BB0,saveEvent(),0xFF07);
                if(wins<3) gabi::call(0x025B8AF4,saveEvent(),0xFF07,(u8)(wins+1));
                a->mSequenceFlags[5]=1; return 0x1D63;
            }
        }
        return 0x1D64;
    }
    return readAt<u8>(p,0x99C)?0x1D4E:0x1D4D;
}
VERIFY(0x02267498,daNpc_Kg1_getMsg);
u16 daNpc_Kg1_next_msgStatus(daNpc_Kg1_c* a,be<u32>* message) {
    WWHD_FUNC(0x022675BC,u16,a,message);
    u32 p=gabi::ea(a),msgObject=gabi::load<u32>(0x101F4B5C);
    u32 play=playObject(),current=*message,player=readAt<u32>(play,0x5B2C);
    switch(current) {
    case 0x1D4D:
        if(!gabi::call<u32>(0x025B8B94,saveEvent(),0xE04)) *message=0x1D52;
        else if(gabi::call<s32>(0x0258839C)<4) *message=0x1D4F;
        else if(gabi::call<u32>(0x025B8B94,saveEvent(0x1178),0x101)) *message=0x1D50;
        else { gabi::call(0x025B8B68,saveEvent(0x1178),0x101); *message=0x1D51; }
        writeAt<u8>(p,0x99C,1); break;
    case 0x1D4E: case 0x1D4F: case 0x1D50: case 0x1D51: *message=0x1D52; break;
    case 0x1D52: case 0x1D5E: {
        daNpc_Kg1_clr_seq_flag(a);
        u32 choice=readAt<u32>(msgObject,0x948);
        if(choice==0) {
            u16 rupees=readAt<u16>(gabi::load<u32>(0x101F84DC),0x24);
            if(rupees<10) *message=0x1D54;
            else {
                a->mRequestedAnimation=1;
                play=playObject(); u32 balance=readAt<u32>(play,0x5B48); writeAt<u32>(play,0x5B48,balance-10);
                gabi::Local<cXyz> position; position->x=0.0f; position->y=0.0f; position->z=250.0f;
                writeAt<u8>(p,0x3B7,1); writeAt<u8>(p,0x97D,1);
                u32 vt=readAt<u32>(player,0xB4);
                gabi::call_ptr(readAt<u32>(vt,0x114),player,position.get(),0x2000);
                *message=0x1D55;
            }
        } else if(choice==1) *message=0x1D53;
        break;
    }
    case 0x1D55: *message=readAt<u8>(p,0x99D)?0x1D5C:0x1D56; a->mSequenceFlags[0]=1; break;
    case 0x1D56: case 0x1D57: case 0x1D58: case 0x1D59: case 0x1D5D: *message=current+1; break;
    case 0x1D5A: { u32 choice=readAt<u32>(msgObject,0x948); if(choice==0) *message=0x1D5C; else if(choice==1) *message=0x1D5B; break; }
    case 0x1D5B: *message=0x1D57; break;
    case 0x1D5F: {
        u32 wins=gabi::call<u32>(0x025B8BB0,saveEvent(),0xFE07);
        *message=wins==0?0x1D60:wins==1?0x1D61:0x1D62;
        u32 event=saveEvent(); if(wins<3) wins=(u8)(wins+1);
        gabi::call(0x025B8AF4,event,0xFE07,wins); break;
    }
    case 0x1D5C: writeAt<u8>(p,0x99D,1); return 0x10;
    default: return 0x10;
    }
    return 0xF;
}
VERIFY(0x022675BC,daNpc_Kg1_next_msgStatus);

