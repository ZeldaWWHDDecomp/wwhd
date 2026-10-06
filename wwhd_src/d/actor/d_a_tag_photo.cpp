#include "d/actor/d_a_tag_photo.h"
#include "bindings.h"
// Event/message APIs differ from GameCube; bindings here describe the actual HD calls.
template<class T> static T* photoField(void* p,u32 offset) {
    return gabi::at<T>(gabi::ea(p)+offset);
}
static u32 photoPlay() {
    return gabi::call<u32>(0x025200D4);
}
static u32 photoEvents() {
    return photoPlay()+0x52c4;
}
static void photoReset() {
    u32 p=photoPlay();
    auto q=gabi::at<be<u16>>(p+0x52b8);
    *q=(u16)*q|8;
}
u32 photoPrm(daTagPhoto_c* p,u32 width,u32 shift) {
    WWHD_FUNC(0x024B26B0,u32,p,width,shift);
    u32 left=width&63, right=shift&63;
    return (right>=32?0:(u32)p->mParameters>>right) & ((left>=32?0:1u<<left)-1);
}
VERIFY(0x024B26B0,photoPrm);
u8 photoTag(daTagPhoto_c* p) {
    WWHD_FUNC(0x024B1CF4,u8,p);
    return (u8)photoPrm(p,8,0);
}
VERIFY(0x024B1CF4,photoTag);
daTagPhoto_c* photoCtor(daTagPhoto_c* p) {
    WWHD_FUNC(0x024B1D20,daTagPhoto_c*,p);
    if(!p) p=gabi::call<daTagPhoto_c*>(0x0273AD10,0x43c);
    if(p) {
        fopAc_ac_c_ct(p);
        p->__vtbl=0x1003fbac;
        gabi::call(0x0259F740,photoField<void>(p,0x3c0));
        p->mTagNo=photoTag(p);
        p->mWaitEntered=0;
        p->mMode=0;
    }
    return p;
}
VERIFY(0x024B1D20,photoCtor);
s32 photoPhase1(daTagPhoto_c* p) {
    WWHD_FUNC(0x024B1D94,s32,p);
    u32 flags=p->actor_condition;
    if(!(flags&8)) {
        if(p) {
            photoCtor(p);
            flags=p->actor_condition;
        }
        p->actor_condition=flags|8;
    }
    return 2;
}
VERIFY(0x024B1D94,photoPhase1);
s32 photoCreateInit(daTagPhoto_c* p) {
    WWHD_FUNC(0x024B1DE8,s32,p);
    p->mPhotoTalkEventIdx=gabi::call<s32>(0x02543F10,photoEvents(),STR(0x1003fbd0),0xff);
    p->mPhotoTalk2EventIdx=gabi::call<s32>(0x02543F10,photoEvents(),STR(0x1003fbc4),0xff);
    gabi::call(0x0259F7D4,photoField<void>(p,0x3c0),STR(0x1003fbbc),p);
    s16 x=p->current.angle.x,y=p->current.angle.y,z=p->current.angle.z;
    p->shape_angle.x=x;
    p->shape_angle.z=z;
    p->shape_angle.y=y;
    *photoField<be<u32>>(p,0x39c)=0x2000000a;
    *photoField<be<u8>>(p,0x389)=0x23;
    s16 event=p->mPhotoTalk2EventIdx;
    *photoField<be<u8>>(p,0x38b)=0x24;
    *photoField<be<s16>>(p,0xfc)=event;
    return 4;
}
VERIFY(0x024B1DE8,photoCreateInit);
s32 photoHeap(daTagPhoto_c* p) {
    WWHD_FUNC(0x024B1CEC,s32,p);
    return 1;
}
VERIFY(0x024B1CEC,photoHeap);
s32 photoPhase2(daTagPhoto_c* p) {
    WWHD_FUNC(0x024B1E9C,s32,p);
    if(!gabi::call<s32>(0x025D63E8,p,0x024B1CEC,0x10000))return 5;
    photoCreateInit(p);
    return 4;
}
VERIFY(0x024B1E9C,photoPhase2);
s32 photoCreate(daTagPhoto_c* p) {
    WWHD_FUNC(0x024B1F00,s32,p);
    return gabi::call<s32>(0x02525FE4,&p->mPhase,0x101d1ff4,p);
}
VERIFY(0x024B1F00,photoCreate);
s32 daTagPhotoCreate(daTagPhoto_c* p) {
    WWHD_FUNC(0x024B1F14,s32,p);
    return photoCreate(p);
}
VERIFY(0x024B1F14,daTagPhotoCreate);
s32 daTagPhotoDelete(daTagPhoto_c* p) {
    WWHD_FUNC(0x024B1F18,s32,p);
    return 1;
}
VERIFY(0x024B1F18,daTagPhotoDelete);
void photoSetMessage(daTagPhoto_c* p,u32 msg) {
    WWHD_FUNC(0x024B1F20,void,p,msg);
    p->mMsgNo=msg;
    p->mMsgID=~0u;
}
VERIFY(0x024B1F20,photoSetMessage);
void photoMesInit(daTagPhoto_c* p,s32 staff) {
    WWHD_FUNC(0x024B1F30,void,p,staff);
    auto msg=gabi::call<be<u32>*>(0x0254487C,photoEvents(),staff,STR(0x1003fbdc),3);
    p->mpMessageSequence=nullptr;
    u32 number=0;
    if(msg) {
        number=*msg;
        if(!number)number=*gabi::at<be<u32>>(0x101d1fd8+4*(u8)p->mTagNo);
    }
    photoSetMessage(p,number);
}
VERIFY(0x024B1F30,photoMesInit);
u32 photoGetMessage(daTagPhoto_c* p) {
    WWHD_FUNC(0x024B1FD4,u32,p);
    u8 tag=p->mTagNo;
    p->mpMessageSequence=nullptr;
    return *gabi::at<be<u32>>(0x101d1fd8+4*tag);
}
VERIFY(0x024B1FD4,photoGetMessage);
u16 photoNextStatus(daTagPhoto_c* p,be<u32>* out) {
    WWHD_FUNC(0x024B1FF0,u16,p,out);
    auto seq=p->mpMessageSequence.get();
    if(!seq)return 0x10;
    seq=gabi::at<be<u32>>(gabi::ea(seq)+4);
    p->mpMessageSequence=seq;
    u32 number=*seq;
    if(!number) {
        p->mpMessageSequence=nullptr;
        return 0x10;
    }
    *out=number;
    return 0xf;
}
VERIFY(0x024B1FF0,photoNextStatus);
u16 photoTalk(daTagPhoto_c* p,s32 mode) {
    WWHD_FUNC(0x024B2038,u16,p,mode);
    u32 id=p->mMsgID;
    u32 manager=*gabi::at<be<u32>>(0x101f4b5c);
    u16 status=0xff;
    if(id==~0u) {
        u32 msg;
        if(mode==1) {
            msg=photoGetMessage(p);
            p->mMsgNo=msg;
        }
        else msg=p->mMsgNo;
        s32 made=gabi::call<s32>(0x025F7DB0,manager,msg,&p->eyePos);
        p->mMsgID=made;
        if(made!=-1)p->mMessageActive=0;
    }
    else if(p->mMessageActive) {
        status=gabi::call<u16>(0x025F795C,manager);
        if(status==0xe) {
            u16 next=photoNextStatus(p,&p->mMsgNo);
            gabi::call(0x025F74D0,manager,next);
            if(gabi::call<s32>(0x025F795C,manager)==0xf) {
                u32 msg=p->mMsgNo;
                gabi::call(0x025F7DB0,manager,msg,0);
            }
        }
        else if(status==0x12) {
            gabi::call(0x025F74D0,manager,0x13);
            p->mMsgID=~0u;
        }
    }
    else p->mMessageActive=1;
    return status;
}
VERIFY(0x024B2038,photoTalk);
bool photoMesSet(daTagPhoto_c* p) {
    WWHD_FUNC(0x024B216C,bool,p);
    return photoTalk(p,0)==0x12;
}
VERIFY(0x024B216C,photoMesSet);
void photoPrivateCut(daTagPhoto_c* p) {
    WWHD_FUNC(0x024B219C,void,p);
    s32 staff=gabi::call<s32>(0x02542D88,photoEvents(),STR(0x1003fbe4),0,0);
    if(staff==-1)return;
    s8 act=gabi::call<s32>(0x02542EDC,photoEvents(),staff,0x101d2000,1,1,0);
    p->mActIdx=act;
    u32 manager=photoEvents();
    if(act!=-1) {
        if(gabi::call<s32>(0x025447C8,manager,staff)) {
            if(p->mActIdx!=0) {
                gabi::call(0x02543280,photoEvents(),staff);
                return;
            }
            photoMesInit(p,staff);
        }
        if(p->mActIdx==0&&!photoMesSet(p))return;
        manager=photoEvents();
    }
    gabi::call(0x02543280,manager,staff);
}
VERIFY(0x024B219C,photoPrivateCut);
void photoEventMove(daTagPhoto_c* p) {
    WWHD_FUNC(0x024B228C,void,p);
    s16 event=p->mPhotoTalkEventIdx;
    bool done=gabi::call<s32>(0x025440C8,photoEvents(),event)!=0;
    if(!done) {
        event=p->mPhotoTalk2EventIdx;
        done=gabi::call<s32>(0x025440C8,photoEvents(),event)!=0;
    }
    if(done) {
        photoReset();
        u32 save=*gabi::at<be<u32>>(0x101f84dc);
        gabi::call(0x025B8B68,save+0x644,0x1601);
    }
    else {
        u8 flag=*photoField<be<u8>>(p,0x420);
        if(gabi::call<s32>(0x0259F858,photoField<void>(p,0x3c0))) {
            if(!*photoField<be<u8>>(p,0x420))*photoField<be<u8>>(p,0x420)=flag;
        }
        else photoPrivateCut(p);
    }
}
VERIFY(0x024B228C,photoEventMove);
s32 photoExecute(daTagPhoto_c* p) {
    WWHD_FUNC(0x024B2374,s32,p);
    u32 play=photoPlay();
    if(!*gabi::at<be<u8>>(play+0x5292)) {
        u32 table=0x101d1fa8+8*(u8)p->mMode;
        s16 delta=*gabi::at<be<s16>>(table),index=*gabi::at<be<s16>>(table+2);
        auto self=gabi::at<void>(gabi::ea(p)+delta);
        u32 target;
        if(index<0)target=*gabi::at<be<u32>>(table+4);
        else {
            s16 vtoff=*gabi::at<be<s16>>(table+6);
            u32 vt=*gabi::at<be<u32>>(gabi::ea(self)+vtoff);
            target=*gabi::at<be<u32>>(vt+8*index+4);
        }
        gabi::call(target,self);
    }
    else photoEventMove(p);
    f32 z=p->current.pos.z,y=p->current.pos.y,x=p->current.pos.x;
    *photoField<be<f32>>(p,0x398)=z;
    *photoField<be<f32>>(p,0x390)=x;
    *photoField<be<f32>>(p,0x394)=y+50.0f;
    return 1;
}
VERIFY(0x024B2374,photoExecute);
s32 daTagPhotoExecute(daTagPhoto_c* p) {
    WWHD_FUNC(0x024B2480,s32,p);
    return photoExecute(p);
}
VERIFY(0x024B2480,daTagPhotoExecute);
s32 daTagPhotoDraw(daTagPhoto_c* p) {
    WWHD_FUNC(0x024B2484,s32,p);
    return 1;
}
VERIFY(0x024B2484,daTagPhotoDraw);
void photoSetMode(daTagPhoto_c* p,u32 mode) {
    WWHD_FUNC(0x024B248C,void,p,mode);
    if(mode==1)p->mMsgID=~0u;
    p->mMode=mode;
}
VERIFY(0x024B248C,photoSetMode);
// Keep the ABI linkage words before both vectors: real nonleaf calls save LR at SP+4.
struct PhotoWaitFrame {
    u8 linkage[8];
    cXyz horizontal;
    cXyz difference;
};
void photoWait(daTagPhoto_c* p) {
    WWHD_FUNC(0x024B24A4,void,p);
    gabi::Local<PhotoWaitFrame> frame;
    u32 play=photoPlay();
    u32 player=*gabi::at<be<u32>>(play+0x5b34);
    gabi::call(0x0201ADE0,gabi::at<cXyz>(player+0x314),&frame->difference,&p->current.pos);
    f32 x=frame->difference.x;
    frame->horizontal.x=x;
    f32 z=frame->difference.z;
    frame->horizontal.y=0;
    frame->horizontal.z=z;
    f32 squared=gabi::call<f32>(0x028E8DD0,&frame->horizontal);
    f32 distance=gabi::call<f32>(0x028F4384,squared);
    if(distance<160.0f) {
        u16 flags=*photoField<be<u16>>(p,0xfa);
        u8 entered=p->mWaitEntered;
        *photoField<be<u16>>(p,0xfa)=flags|1;
        if(!entered) {
            p->mWaitEntered=1;
            auto npc=gabi::call<void*>(0x025D9F38,STR(0x1003fc00),0xf,0);
            if(npc&&*photoField<be<u8>>(npc,0xb35)!=4)*photoField<be<s16>>(p,0xfc)=p->mPhotoTalkEventIdx;
        }
    }
}
VERIFY(0x024B24A4,photoWait);
void photoExecuteTalk(daTagPhoto_c* p) {
    WWHD_FUNC(0x024B256C,void,p);
    if(photoTalk(p,1)==0x12) {
        photoReset();
        photoSetMode(p,0);
    }
}
VERIFY(0x024B256C,photoExecuteTalk);
void photoStaticInit() {
    WWHD_FUNC(0x024B25C0,void);
    *gabi::at<be<u32>>(0x1046e62c)=0;
    *gabi::at<be<u32>>(0x1046e624)=0;
    *gabi::at<be<u32>>(0x1046e630)=0;
    *gabi::at<be<u32>>(0x1046e628)=0;
    gabi::call(0x028F026C,0x101d2004);
    *gabi::at<be<f32>>(0x1046e618)=-3.1415927410125732f;
    *gabi::at<be<f32>>(0x1046e61c)=3.1415927410125732f;
    gabi::call(0x028ED6F8,0x1046e620);
    gabi::call(0x028F026C,0x101d2010);
    gabi::call(0x028EAB2C,0x1046e621);
    gabi::call(0x028F026C,0x101d201c);
}
VERIFY(0x024B25C0,photoStaticInit);
s32 daTagPhotoIsDelete(daTagPhoto_c* p) {
    WWHD_FUNC(0x024B2654,s32,p);
    return 1;
}
VERIFY(0x024B2654,daTagPhotoIsDelete);
void photoDestructor(daTagPhoto_c* p,s32 flags) {
    WWHD_FUNC(0x024B265C,void,p,flags);
    if(p) {
        gabi::call(0x025D50BC,p,0);
        if(flags&1)gabi::call(0x0273AF40,p);
    }
}
VERIFY(0x024B265C,photoDestructor);
