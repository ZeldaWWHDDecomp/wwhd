// WWHD auction controller reconstruction.
#include "d/actor/d_a_auction.h"
#include "bindings.h"
namespace {
template<class T> T& field(u32 address) { return *gabi::at<T>(address); }
f32 constant(u32 address) { return field<be<f32>>(address); }
struct AuctionItem { be<s16> item, message, startingBid; be<u16> eventBit; };
}

void Auction_setMessage(daAuction_c* actor,u32 message) {
    WWHD_FUNC(0x020594AC,void,actor,message);
    actor->mCurrMsgNo=message;
    actor->mCurrMsgBsPcId=0xFFFFFFFF;
}
VERIFY(0x020594AC,Auction_setMessage);

void Auction_offCamera(daAuction_c* actor) {
    WWHD_FUNC(0x02059B40,void,actor);
    actor->mCameraFlags=(actor->mCameraFlags|2)&0xFE;
}
VERIFY(0x02059B40,Auction_offCamera);

void Auction_obtainItem(AuctionItem* item) {
    WWHD_FUNC(0x02059838,void,item);
    u16 event=item->eventBit;
    if(event) {
        u32 save=field<be<u32>>(0x101F84DC);
        gabi::call(0x025B8B68,gabi::at<void>(save+0x644),event);
    }
}
VERIFY(0x02059838,Auction_obtainItem);

BOOL Auction_itemObtained(AuctionItem* item) {
    WWHD_FUNC(0x0205A608,BOOL,item);
    u16 event=item->eventBit;
    if(event) {
        u32 save=field<be<u32>>(0x101F84DC);
        return gabi::call<s32>(0x025B8B94,gabi::at<void>(save+0x644),event)!=0;
    }
    if(item->item==0x77) {
        u32 save=field<be<u32>>(0x101F84DC);
        return field<be<u8>>(save+0x5D)==0x77;
    }
    return false;
}
VERIFY(0x0205A608,Auction_itemObtained);

BOOL Auction_hasAvailableItem() {
    WWHD_FUNC(0x0205CD90,BOOL);
    for(u32 i=0;i<5;++i)
        if(!gabi::call<BOOL>(0x0205A608,gabi::at<AuctionItem>(0x1019016C+i*8))) return true;
    return false;
}
VERIFY(0x0205CD90,Auction_hasAvailableItem);

void Auction_eventEndInit(daAuction_c* actor) {
    WWHD_FUNC(0x02059C44,void,actor);
    u8 flags=actor->mFlags;
    f32 blend=constant(0x10007820);
    actor->mFlags=flags&0xFB;
    actor->mBlend=blend;
    gabi::call(0x0255BA9C,&actor->mLight);
}
VERIFY(0x02059C44,Auction_eventEndInit);

BOOL Auction_eventMesSet(daAuction_c* actor) {
    WWHD_FUNC(0x02059C64,BOOL,actor);
    gabi::call(0x025A11EC,actor,0);
    return actor->mCurrMsgBsPcId!=0xFFFFFFFF;
}
VERIFY(0x02059C64,Auction_eventMesSet);

BOOL Auction_eventMesEnd(daAuction_c* actor) {
    WWHD_FUNC(0x02059CA8,BOOL,actor);
    return gabi::call<u32>(0x025A11EC,actor,0)==0x12;
}
VERIFY(0x02059CA8,Auction_eventMesEnd);

BOOL Auction_eventStart(daAuction_c* actor) {
    WWHD_FUNC(0x02059CD8,BOOL,actor);
    if(!actor->mGaugeID) {
        u32 gauge=gabi::call<u32>(0x025DE50C,(u32)actor->mTimerID);
        actor->mGaugeID=gauge;
        if(gauge) gabi::call(0x025C5E54,gabi::at<void>(gauge),0);
    }
    u32 play=gabi::call<u32>(0x025200D4);
    field<be<u8>>(play+0x5BBA)=0x3E;
    play=gabi::call<u32>(0x025200D4);
    field<be<u8>>(play+0x5BB9)=0x3E;
    return actor->mGaugeID!=0;
}
VERIFY(0x02059CD8,Auction_eventStart);

void Auction_executeWait(daAuction_c* actor) {
    WWHD_FUNC(0x0205AD44,void,actor);
    for(u32 i=0;i<8;++i) if(actor->mNpcIDs[i]==0xFFFFFFFF) return;
    actor->mMoveState=1;
}
VERIFY(0x0205AD44,Auction_executeWait);

void Auction_setMessage2(daAuction_c* actor,u32 message) {
    WWHD_FUNC(0x0205AD6C,void,actor,message);
    u32 receiver=gabi::call<u32>(0x020594AC,actor,message);
    field<be<u8>>(receiver+0x937)=2;
    gabi::call(0x0261BC80,gabi::at<void>(receiver));
}
VERIFY(0x0205AD6C,Auction_setMessage2);

void Auction_eventMainMsgSet(daAuction_c* actor) {
    WWHD_FUNC(0x0205BFD8,void,actor);
    if(gabi::call<BOOL>(0x02059C64,actor)) actor->m937=3;
}
VERIFY(0x0205BFD8,Auction_eventMainMsgSet);

void Auction_SafeString_deletingDestructor(void* object,s32 flags) {
    WWHD_FUNC(0x0205CFB4,void,object,flags);
    if(object && (flags&1)) gabi::call(0x0273AF40,object);
}
VERIFY(0x0205CFB4,Auction_SafeString_deletingDestructor);
BOOL Auction_isDelete(void* actor) { WWHD_FUNC(0x0205CFC8,BOOL,actor); return true; }
VERIFY(0x0205CFC8,Auction_isDelete);
void Auction_emptyCFD0(void* object,void* context) { WWHD_FUNC(0x0205CFD0,void,object,context); }
VERIFY(0x0205CFD0,Auction_emptyCFD0);
void Auction_emptyCFD4(void* object,void* context) { WWHD_FUNC(0x0205CFD4,void,object,context); }
VERIFY(0x0205CFD4,Auction_emptyCFD4);
void Auction_SafeString_assureTerminated(void* object) { WWHD_FUNC(0x0205D074,void,object); }
VERIFY(0x0205D074,Auction_SafeString_assureTerminated);

void Auction_deletingDestructor(daAuction_c* actor,s32 flags) {
    WWHD_FUNC(0x0205CFD8,void,actor,flags);
    if(!actor) return;
    gabi::call(0x02515A70,&actor->mCyl,2);
    gabi::call(0x02515860,&actor->mStts,2);
    gabi::call(0x02018034,gabi::at<void>(gabi::ea(actor)+0x628),2);
    field<be<u32>>(gabi::ea(actor)+0x470)=0x10007764;
    field<be<u32>>(gabi::ea(actor)+0x464)=0x10007774;
    gabi::call(0x024EFD9C,&actor->mObjAcch,0);
    gabi::call(0x025D50BC,actor,0);
    if(flags&1) gabi::call(0x0273AF40,actor);
}
VERIFY(0x0205CFD8,Auction_deletingDestructor);

BOOL Auction_createHeap(daAuction_c* actor) {
    WWHD_FUNC(0x02058DF0,BOOL,actor);
    struct Name { be<u32> text, vtable; };
    gabi::Local<Name> archive;
    archive->vtable=0x1000774C;
    u32 manager=field<be<u32>>(0x101F4F28);
    archive->text=0x10007784;
    u32 resource=gabi::call<u32>(0x026067F4,gabi::at<void>(manager),archive.get(),0);
    if(!resource) return false;
    u32 model=gabi::call<u32>(0x025E38E0,gabi::at<void>(resource),0,0x11020203);
    actor->mpModel=model;
    return model!=0;
}
VERIFY(0x02058DF0,Auction_createHeap);
BOOL Auction_heapCallback(daAuction_c* actor) { WWHD_FUNC(0x02058E84,BOOL,actor); return gabi::call<BOOL>(0x02058DF0,actor); }
VERIFY(0x02058E84,Auction_heapCallback);

s32 Auction_getRand(daAuction_c* actor,s32 range) {
    WWHD_FUNC(0x02058E88,s32,actor,range);
    f32 random=gabi::call<f32>(0x020198D8,(f32)range);
    s32 result=gabi::ftoi(random);
    return (u32)result==(u32)range?0:result;
}
VERIFY(0x02058E88,Auction_getRand);

daAuction_c* Auction_constructor(daAuction_c* actor) {
    WWHD_FUNC(0x02058EF4,daAuction_c*,actor);
    if(!actor) {
        actor=gabi::call<daAuction_c*>(0x0273AD10,0x954);
        if(!actor) return actor;
    }
    gabi::call(0x025A1458,actor);
    field<be<u32>>(gabi::ea(actor)+0xB4)=0x10007B98;
    gabi::call(0x0259F740,&actor->mNpcEvtInfo);
    f32 light=constant(0x10007798);
    actor->mLight.m20=light;
    for(u32 i=0;i<8;++i) {
        actor->mNpcIDs[i]=0xFFFFFFFF;
        actor->mNpcOrder[i]=i;
        actor->mNpcKinds[i]=0xFF;
    }
    for(u32 i=0;i<100;++i) {
        u32 first=(u32)gabi::call<s32>(0x02058E88,actor,6)+1;
        u32 second=(u32)gabi::call<s32>(0x02058E88,actor,6)+1;
        u32 order=gabi::ea(actor)+0x924;
        u8 a=field<be<u8>>(order+second),b=field<be<u8>>(order+first);
        field<be<u8>>(order+first)=a;
        field<be<u8>>(order+second)=b;
    }
    u32 x=field<be<u32>>(gabi::ea(actor)+0x314);
    u32 z=field<be<u32>>(gabi::ea(actor)+0x31C);
    actor->m934=0xFF;
    actor->m935=0xFF;
    u32 y=field<be<u32>>(gabi::ea(actor)+0x318);
    actor->m94A=0;
    field<be<u32>>(gabi::ea(actor)+0x380)=y;
    actor->mBidStep=0;
    actor->mBidLimit=0;
    field<be<u32>>(gabi::ea(actor)+0x37C)=x;
    field<be<u32>>(gabi::ea(actor)+0x384)=z;
    actor->m939=0;
    actor->m938=1;
    actor->mpEmitter=0;
    actor->mMoveState=0;
    actor->m94E=0;
    return actor;
}
VERIFY(0x02058EF4,Auction_constructor);

s32 Auction_create(daAuction_c* actor) {
    WWHD_FUNC(0x02059240,s32,actor);
    u32 condition=field<be<u32>>(gabi::ea(actor)+0x2E4);
    if(!(condition&8)) {
        if(actor) {
            gabi::call(0x02058EF4,actor);
            condition=field<be<u32>>(gabi::ea(actor)+0x2E4);
        }
        field<be<u32>>(gabi::ea(actor)+0x2E4)=condition|8;
    }
    s32 phase=gabi::call<s32>(0x02520460,actor->mPhs,STR(0x100077F8));
    if(phase==4) {
        if(!gabi::call<s32>(0x025D63E8,actor,gabi::at<void>(0x02058E84),0x2400)) return 5;
        return gabi::call<s32>(0x020590FC,actor);
    }
    return phase;
}
VERIFY(0x02059240,Auction_create);
s32 Auction_createWrapper(daAuction_c* actor) { WWHD_FUNC(0x020592E8,s32,actor); return gabi::call<s32>(0x02059240,actor); }
VERIFY(0x020592E8,Auction_createWrapper);

BOOL Auction_delete(daAuction_c* actor) {
    WWHD_FUNC(0x020592EC,BOOL,actor);
    if(gabi::call<s32>(0x025986B0,actor)) {
        u32 global=field<be<u32>>(0x101F8344);
        u32 controller=field<be<u32>>(global+0x1F4);
        gabi::call(0x020063C0,gabi::at<void>(controller+0x18),gabi::at<void>(0x1048E0DC));
    }
    gabi::call(0x025204C8,actor->mPhs,STR(0x10007800));
    if(field<be<u32>>(gabi::ea(actor)+0xF4)) {
        u32 emitter=actor->mpEmitter;
        if(emitter) {
            u32 flags=field<be<u32>>(emitter+0x254);
            field<be<u32>>(emitter+0x5C)=0xFFFFFFFF;
            field<be<u32>>(emitter+0x254)=flags|1;
        }
    }
    return true;
}
VERIFY(0x020592EC,Auction_delete);
BOOL Auction_deleteWrapper(daAuction_c* actor) { WWHD_FUNC(0x0205937C,BOOL,actor); return gabi::call<BOOL>(0x020592EC,actor); }
VERIFY(0x0205937C,Auction_deleteWrapper);

BOOL Auction_draw(daAuction_c* actor) {
    WWHD_FUNC(0x0205ACD8,BOOL,actor);
    u32 env=gabi::call<u32>(0x02555D0C);
    gabi::call(0x025626A4,gabi::at<void>(env),3,gabi::at<void>(gabi::ea(actor)+0x314),gabi::at<void>(gabi::ea(actor)+0x110));
    env=gabi::call<u32>(0x02555D0C);
    u32 model=actor->mpModel;
    gabi::call(0x02562F5C,gabi::at<void>(env),gabi::at<void>(model),gabi::at<void>(gabi::ea(actor)+0x110));
    if(actor->mFlags&4) {
        model=actor->mpModel;
        gabi::call(0x025E2DE0,gabi::at<void>(model),0);
    }
    return true;
}
VERIFY(0x0205ACD8,Auction_draw);
BOOL Auction_drawWrapper(daAuction_c* actor) { WWHD_FUNC(0x0205AD40,BOOL,actor); return gabi::call<BOOL>(0x0205ACD8,actor); }
VERIFY(0x0205AD40,Auction_drawWrapper);
BOOL Auction_executeWrapper(daAuction_c* actor) { WWHD_FUNC(0x0205ACD4,BOOL,actor); return gabi::call<BOOL>(0x0205AAC8,actor); }
VERIFY(0x0205ACD4,Auction_executeWrapper);

void Auction_setMtx(daAuction_c* actor) {
    WWHD_FUNC(0x02059030,void,actor);
    u32 a=gabi::ea(actor);
    f32 sx=field<be<f32>>(a+0x330),sy=field<be<f32>>(a+0x334);
    u32 model=actor->mpModel;
    f32 sz=field<be<f32>>(a+0x338);
    field<be<f32>>(model+0xBC)=sx;
    field<be<f32>>(model+0xC0)=sy;
    field<be<f32>>(model+0xC4)=sz;
    f32 x=field<be<f32>>(a+0x314),y=field<be<f32>>(a+0x318),z=field<be<f32>>(a+0x31C);
    gabi::call(0x028E93CC,gabi::at<void>(0x1048D0CC),x,y,z);
    f32 matrix[12];
    for(u32 i=0;i<12;++i) matrix[i]=field<be<f32>>(0x1048D0CC+i*4);
    model=actor->mpModel;
    for(u32 i=0;i<12;++i) field<be<f32>>(model+0xC8+i*4)=matrix[i];
}
VERIFY(0x02059030,Auction_setMtx);

fopAc_ac_c* Auction_getNpcActor(daAuction_c* actor,s32 index) {
    WWHD_FUNC(0x02059854,fopAc_ac_c*,actor,index);
    if(!index) {
        u32 play=gabi::call<u32>(0x025200D4);
        return gabi::at<fopAc_ac_c>(field<be<u32>>(play+0x5B2C));
    }
    u32 id=field<be<u32>>(gabi::ea(actor)+0x850+(u32)index*4);
    gabi::Local<be<u32>> query;
    query.get()->set(id);
    if(id==0xFFFFFFFF) return nullptr;
    return gabi::call<fopAc_ac_c*>(0x025D5218,gabi::at<void>(0x025E1234),query.get());
}
VERIFY(0x02059854,Auction_getNpcActor);

void Auction_setLinkAnm(daAuction_c* actor,u8 animation) {
    WWHD_FUNC(0x02059D50,void,actor,animation);
    u32 selected=animation;
    if(selected==1 && !actor->m93E) selected=0x1D;
    u32 play=gabi::call<u32>(0x025200D4);
    u32 link=field<be<u32>>(play+0x5B34);
    field<be<u32>>(link+0x430)=selected;
    field<be<u32>>(link+0x428)=3;
    actor->mCurLinkAnm=selected;
}
VERIFY(0x02059D50,Auction_setLinkAnm);

BOOL Auction_eventCameraOffNpc(daAuction_c* actor) {
    WWHD_FUNC(0x0205A010,BOOL,actor);
    if(!gabi::call<BOOL>(0x02059CA8,actor)) return false;
    u8 flags=actor->mCameraFlags;
    actor->m94C=0;
    actor->mCameraFlags=(flags|2)&0xFE;
    return true;
}
VERIFY(0x0205A010,Auction_eventCameraOffNpc);

BOOL Auction_eventEnd(daAuction_c* actor) {
    WWHD_FUNC(0x0205A078,BOOL,actor);
    f32 blend=actor->mBlend;
    f32 step=constant(0x10007928);
    f32 next=gabi::fadds_ppc(blend,step);
    f32 limit=constant(0x10007798);
    if(next>limit) next=limit;
    actor->mBlend=next;
    gabi::call(0x0255FDA4,4,0,next);
    return actor->mBlend==limit;
}
VERIFY(0x0205A078,Auction_eventEnd);

u8 Auction_getItemNo(daAuction_c* actor) {
    WWHD_FUNC(0x0205A67C,u8,actor);
    u32 available=0;
    for(u32 i=0;i<5;++i) if(!gabi::call<BOOL>(0x0205A608,gabi::at<AuctionItem>(0x1019016C+i*8))) ++available;
    u32 choice=gabi::call<u32>(0x02058E88,actor,available);
    u32 result=0;
    for(u32 i=0;i<4;++i) {
        if(!gabi::call<BOOL>(0x0205A608,gabi::at<AuctionItem>(0x1019016C+i*8))) {
            if(!choice) break;
            --choice;
        }
        ++result;
    }
    return result;
}
VERIFY(0x0205A67C,Auction_getItemNo);

s32 Auction_createInit(daAuction_c* actor) {
    WWHD_FUNC(0x020590FC,s32,actor);
    const u32 names[5]={0x100077B4,0x100077C4,0x100077A4,0x100077D8,0x100077E8};
    const u32 destinations[5]={0x90C,0x90E,0x910,0x912,0x914};
    for(u32 i=0;i<5;++i) {
        u32 play=gabi::call<u32>(0x025200D4);
        s32 event=gabi::call<s32>(0x02543F10,gabi::at<void>(play+0x52C4),STR(names[i]),0xFF);
        field<be<s16>>(gabi::ea(actor)+destinations[i])=event;
    }
    gabi::call(0x0259F7D4,&actor->mNpcEvtInfo,STR(0x1000779C),actor);
    u8 kind=actor->mNpcKinds[0];
    actor->m93C=0;
    actor->m93E=0xFF;
    s32 message=kind<12?(s16)field<be<s16>>(0x10190304+(u32)kind*10):0;
    u32 play=gabi::call<u32>(0x025200D4);
    field<be<u32>>(play+0x5B54)=message;
    actor->mCurrAuctionItemIndex=0;
    s16 startingBid=field<be<s16>>(0x10190170);
    s16 name=field<be<s16>>(0x1019016E);
    actor->mCurrBid=startingBid;
    play=gabi::call<u32>(0x025200D4);
    field<be<u32>>(play+0x5B58)=(s32)name;
    s16 bid=actor->mCurrBid;
    play=gabi::call<u32>(0x025200D4);
    field<be<s16>>(play+0x5BA0)=bid;
    gabi::call(0x02059030,actor);
    return 4;
}
VERIFY(0x020590FC,Auction_createInit);

void Auction_staticInit() {
    WWHD_FUNC(0x0205CE14,void);
    field<be<u32>>(0x10461540)=0;
    field<be<u32>>(0x1046153C)=0;
    field<be<u32>>(0x10461538)=0;
    field<be<u32>>(0x10461534)=0;
    gabi::call(0x028F026C,gabi::at<void>(0x101903FC));
    f32 light=constant(0x10007B40),blend=constant(0x10007B44);
    field<be<f32>>(0x10461518)=light;
    field<be<f32>>(0x1046151C)=blend;
    gabi::call(0x028ED6F8,gabi::at<void>(0x10461530));
    gabi::call(0x028F026C,gabi::at<void>(0x10190408));
    gabi::call(0x028EAB2C,gabi::at<void>(0x10461531));
    gabi::call(0x028F026C,gabi::at<void>(0x10190414));
    f32 speed=constant(0x10007B48),first=constant(0x10007B50),gauge=constant(0x10007B4C);
    f32 second=constant(0x10007B54),third=constant(0x10007B58);
    field<be<f32>>(0x10461520)=speed;
    field<be<f32>>(0x1046152C)=gauge;
    field<be<f32>>(0x10461544)=first;
    field<be<f32>>(0x10461548)=second;
    f32 fourth=constant(0x10007B5C);
    field<be<f32>>(0x1046154C)=third;
    f32 fifth=constant(0x10007B60);
    field<be<f32>>(0x10461550)=fourth;
    field<be<f32>>(0x10461554)=fifth;
    f32 sixth=constant(0x10007B64),seventh=constant(0x10007B68);
    field<be<f32>>(0x10461558)=sixth;
    f32 eighth=constant(0x10007B6C);
    field<be<f32>>(0x1046155C)=seventh;
    f32 ninth=constant(0x10007B70);
    field<be<f32>>(0x10461560)=eighth;
    f32 tenth=constant(0x10007B74),eleventh=constant(0x10007B78),thirteenth=constant(0x10007B80);
    f32 twelfth=constant(0x10007B7C);
    field<be<f32>>(0x10461564)=ninth;
    field<be<f32>>(0x10461568)=tenth;
    f32 fourteenth=constant(0x10007B84);
    field<be<f32>>(0x1046156C)=eleventh;
    f32 fifteenth=constant(0x10007B88);
    field<be<f32>>(0x10461570)=twelfth;
    f32 sixteenth=constant(0x10007B8C),seventeenth=constant(0x10007B90);
    field<be<f32>>(0x10461574)=thirteenth;
    field<be<f32>>(0x10461528)=gauge;
    f32 eighteenth=constant(0x10007B94);
    field<be<f32>>(0x10461578)=fourteenth;
    field<be<f32>>(0x10461524)=speed;
    field<be<f32>>(0x1046157C)=fifteenth;
    field<be<f32>>(0x10461580)=sixteenth;
    field<be<f32>>(0x10461584)=seventeenth;
    field<be<f32>>(0x10461588)=eighteenth;
}
VERIFY(0x0205CE14,Auction_staticInit);

void Auction_eventTalkInit(daAuction_c* actor,s32 staff) {
    WWHD_FUNC(0x020594BC,void,actor,staff);
    u32 play=gabi::call<u32>(0x025200D4);
    u32 data=gabi::call<u32>(0x0254487C,gabi::at<void>(play+0x52C4),staff,STR(0x10007808),3);
    u32 message=0;
    if(data) {
        message=field<be<u32>>(data);
        if(message==0x1CF4) {
            gabi::call(0x025E1988,0x8DC);
            message=field<be<u32>>(data);
        }
    }
    gabi::call(0x020594AC,actor,message);
}
VERIFY(0x020594BC,Auction_eventTalkInit);

void Auction_eventGetItemNpcInit(daAuction_c* actor,s32 staff) {
    WWHD_FUNC(0x02059B54,void,actor,staff);
    u8 npc=actor->m93C;
    gabi::call(0x020598BC,actor,npc,0);
    u32 play=gabi::call<u32>(0x025200D4);
    u32 data=gabi::call<u32>(0x0254487C,gabi::at<void>(play+0x52C4),staff,STR(0x10007880),3);
    s16 timer=0;
    npc=actor->m93C;
    if(data) timer=field<be<s16>>(data+2);
    actor->m941=npc;
    actor->mTimer=timer;
    actor->m94C=0x10;
}
VERIFY(0x02059B54,Auction_eventGetItemNpcInit);

void Auction_eventGetItemMesInit(daAuction_c* actor) {
    WWHD_FUNC(0x02059BD4,void,actor);
    u8 npc=actor->m93C;
    u32 kind=field<be<u8>>(gabi::ea(actor)+0x92C+npc);
    if(kind>=12) {
        gabi::call(0x0273AA24,STR(0x10007888),0xBF6,STR(0x10007898));
        kind=0;
    }
    s16 message=field<be<s16>>(0x1019030C+kind*10);
    gabi::call(0x020594AC,actor,(s32)message);
}
VERIFY(0x02059BD4,Auction_eventGetItemMesInit);

void Auction_eventMainInit(daAuction_c* actor) {
    WWHD_FUNC(0x020596F0,void,actor);
    f32 zero=constant(0x10007820);
    actor->m91C=0xE10;
    for(u32 i=0;i<7;++i) actor->mNpcWait[i]=zero;
    u8 type=actor->m93A;
    actor->m942=0;
    actor->m943=0;
    actor->m94C=0;
    actor->m946=0;
    actor->m937=type==2;
    f32 one=constant(0x10007798);
    actor->mCameraFlags=0;
    actor->mBidSpeed=zero;
    actor->mGaugeValue=one;
    u32 play=gabi::call<u32>(0x025200D4);
    if(field<be<u32>>(play+0x5CFC)==4) {
        play=gabi::call<u32>(0x025200D4);
        u32 timer=field<be<u32>>(play+0x5CF0);
        if(timer) gabi::call(0x025C5864,gabi::at<void>(timer));
    }
    if(actor->m93A==1) gabi::call(0x0261BC68);
    actor->m94E=actor->m94E|1;
    play=gabi::call<u32>(0x025200D4);
    u32 link=field<be<u32>>(play+0x5B34);
    field<be<s16>>(link+0x420)=3;
    field<be<u32>>(link+0x428)=0;
    actor->mCurLinkAnm=1;
    play=gabi::call<u32>(0x025200D4);
    u32 eventManager=play+0x51D0;
    u32 index=gabi::call<u32>(0x0253F124,gabi::at<void>(eventManager),actor);
    field<be<u32>>(eventManager+0xCC)=index;
    actor->m947=0;
    for(u32 i=0;i<6;++i) field<be<u32>>(gabi::ea(actor)+0x8A4+i*4)=field<be<u32>>(0x10461544+i*4);
    gabi::call(0x0261BC20);
}
VERIFY(0x020596F0,Auction_eventMainInit);

void Auction_eventGetItemInit(daAuction_c* actor) {
    WWHD_FUNC(0x02059A44,void,actor);
    u8 type=actor->m93A,index=actor->mCurrAuctionItemIndex;
    u32 itemAddress=type==2?0x10190168+(u32)index*2:0x1019016C+(u32)index*8;
    u8 item=field<be<u8>>(itemAddress+1);
    s32 id=gabi::call<s32>(0x025D7DEC,gabi::at<void>(gabi::ea(actor)+0x314),item,0,-1,-1,0,0);
    if(id!=-1) {
        u32 play=gabi::call<u32>(0x025200D4);
        field<be<u32>>(play+0x52A0)=id;
    }
    index=actor->mCurrAuctionItemIndex;
    itemAddress=(type==2?0x10190194:0x1019016C)+(u32)index*8;
    gabi::call(0x02059838,gabi::at<AuctionItem>(itemAddress));
    gabi::call(0x020598BC,actor,0,0);
}
VERIFY(0x02059A44,Auction_eventGetItemInit);

void Auction_checkOrder(daAuction_c* actor) {
    WWHD_FUNC(0x02059380,void,actor);
    if(field<be<u16>>(gabi::ea(actor)+0xF8)!=2) return;
    const u32 events[5]={0x90C,0x90E,0x910,0x912,0x914};
    for(u32 i=0;i<3;++i) {
        s16 event=field<be<s16>>(gabi::ea(actor)+events[i]);
        u32 play=gabi::call<u32>(0x025200D4);
        if(gabi::call<s32>(0x0254407C,gabi::at<void>(play+0x52C4),(s32)event) && actor->m950==(s8)(i+3)) {
            actor->m950=0;
            break;
        }
    }
    for(u32 i=3;i<5;++i) {
        s16 event=field<be<s16>>(gabi::ea(actor)+events[i]);
        u32 play=gabi::call<u32>(0x025200D4);
        if(gabi::call<s32>(0x0254407C,gabi::at<void>(play+0x52C4),(s32)event) && actor->m950==(s8)(i+3)) {
            actor->m950=0;
            break;
        }
    }
}
VERIFY(0x02059380,Auction_checkOrder);

void Auction_eventStartInit(daAuction_c* actor) {
    WWHD_FUNC(0x02059544,void,actor);
    u8 type=actor->m93A;
    f32 y=constant(0x10007814),x=constant(0x10007810),w=constant(0x10007818);
    actor->m938=0xFE;
    f32 h=constant(0x1000781C);
    s32 timer=gabi::call<s32>(0x025C60F4,4,type==2?0x1E:0x3C,1,0,x,y,w,h);
    actor->mTimerID=timer;
    if(timer==-1) gabi::call(0x0273AA24,STR(0x10007830),0x87B,STR(0x10007840));
    u32 global=field<be<u32>>(0x101F8344),controller=field<be<u32>>(global+0x1F4);
    gabi::call(0x020063C0,gabi::at<void>(controller+0x18),gabi::at<void>(0x1048E0AC));
    actor->mGaugeID=0;
    actor->m948=1;
    u32 play=gabi::call<u32>(0x025200D4);
    field<be<s16>>(play+0x5CEC)=0;
    play=gabi::call<u32>(0x025200D4);
    field<be<s16>>(play+0x5BA6)=0;
    gabi::call(0x0261BC50);
    gabi::call(0x0261BC80);
    gabi::call(0x0261BC20);
    u8 index=actor->mCurrAuctionItemIndex;
    s16 item=field<be<s16>>(0x1019016C+(u32)index*8);
    u32 itemID=gabi::call<u32>(0x025D5834,0x103,(s32)item,gabi::at<void>(gabi::ea(actor)+0x314),-1,0,0,-1,0);
    f32 one=constant(0x10007798);
    actor->mCurrAuctionItemPID=itemID;
    gabi::call(0x0255FDA4,0,4,one);
    f32 z=constant(0x10007828),power=constant(0x1000782C);
    actor->mLight.position.z=z;
    actor->mLight.power=power;
    f32 lightY=constant(0x10007824),zero=constant(0x10007820);
    actor->mLight.position.y=lightY;
    actor->mLight.position.x=zero;
    actor->mLight.blue=0x3C;
    actor->mLight.fluctuation=zero;
    actor->mLight.red=0x96;
    actor->mLight.green=0x64;
    gabi::call(0x0255B9C8,&actor->mLight);
    actor->mFlags=actor->mFlags|4;
    gabi::call(0x0261BC20);
}
VERIFY(0x02059544,Auction_eventStartInit);

void Auction_setCameraNpc(daAuction_c* actor,s32 index,s16 angle) {
    WWHD_FUNC(0x020598BC,void,actor,index,angle);
    u32 npc=gabi::call<u32>(0x02059854,actor,index);
    if(!npc) {
        gabi::call(0x0273AA24,STR(0x10007870),0xDA1,STR(0x10007868));
        return;
    }
    f32 x=field<be<f32>>(npc+0x314);
    actor->mParticlePos.x=x;
    f32 y=field<be<f32>>(npc+0x318);
    actor->mParticlePos.y=y;
    f32 z=field<be<f32>>(npc+0x31C);
    u32 data=0x1019037C+(u32)index*12;
    actor->mParticlePos.z=z;
    f32 height=constant(data+4);
    actor->mParticlePos.y=gabi::fadds_ppc(y,height);
    u16 facing=field<be<u16>>(npc+0x322);
    f32 radius=constant(data);
    f32 sine=constant(0x104A44F8+(u32)(facing>>3)*8);
    f32 offsetX=gabi::fmuls_ppc(sine,radius);
    f32 zero=constant(0x10007820);
    actor->mParticleScale.x=offsetX;
    actor->mParticleScale.y=zero;
    facing=field<be<u16>>(npc+0x322);
    f32 cosine=constant(0x104A44FC+(u32)(facing>>3)*8);
    radius=constant(data);
    actor->mParticleScale.z=gabi::fmuls_ppc(cosine,radius);
    s16 baseAngle=field<be<s16>>(data+0xA);
    s32 extra=angle;
    if(!extra) extra=(s16)gabi::ftoi(gabi::call<f32>(0x02019918,constant(0x10007864)));
    gabi::call(0x025F1884,gabi::at<void>(0x1048D0CC),(s16)((u32)(s32)baseAngle+(u32)extra));
    s16 tilt=field<be<s16>>(data+8);
    gabi::call(0x025F1BF4,gabi::at<void>(0x1048D0CC),tilt);
    gabi::call(0x028E8F64,gabi::at<void>(0x1048D0CC),&actor->mParticleScale,&actor->mParticleScale);
    gabi::call(0x028E8D88,&actor->mParticleScale,&actor->mParticlePos,&actor->mParticleScale);
    actor->mCameraFlags=(actor->mCameraFlags|9)&0xFD;
}
VERIFY(0x020598BC,Auction_setCameraNpc);

namespace {
template<class R=void> R auctionMember(daAuction_c* actor,u32 descriptor) {
    s16 index=field<be<s16>>(descriptor+2);
    s16 adjustment=field<be<s16>>(descriptor);
    u32 receiver=gabi::ea(actor)+(u32)(s32)adjustment;
    u32 target;
    if(index<0) target=field<be<u32>>(descriptor+4);
    else {
        s16 offset=field<be<s16>>(descriptor+6);
        u32 table=field<be<u32>>(receiver+(u32)(s32)offset);
        target=field<be<u32>>(table+(u32)(s32)index*8+4);
    }
    return gabi::call<R>(target,gabi::at<void>(receiver));
}
bool auctionEventEnded(daAuction_c* actor,u32 eventOffset) {
    s16 event=field<be<s16>>(gabi::ea(actor)+eventOffset);
    u32 play=gabi::call<u32>(0x025200D4);
    return gabi::call<s32>(0x025440C8,gabi::at<void>(play+0x52C4),(s32)event)!=0;
}
void auctionEventFlag() {
    u32 play=gabi::call<u32>(0x025200D4);
    field<be<u16>>(play+0x52B8)=field<be<u16>>(play+0x52B8)|8;
}
}
void Auction_eventMove(daAuction_c* actor) {
    WWHD_FUNC(0x0205A428,void,actor);
    if(auctionEventEnded(actor,0x90C)) {
        auctionEventFlag();
        actor->m950=actor->m93C?5:4;
        return;
    }
    bool ended=auctionEventEnded(actor,0x90E);
    if(!ended) ended=auctionEventEnded(actor,0x910);
    if(!ended) ended=auctionEventEnded(actor,0x914);
    if(ended) {
        auctionEventFlag();
        u32 play=gabi::call<u32>(0x025200D4);
        u16 flags=field<be<u16>>(play+0x5CE8);
        field<be<u8>>(play+0x5CEE)=0;
        field<be<u8>>(play+0x5CEA)=0;
        field<be<u16>>(play+0x5CE8)=flags^0x10;
        gabi::call(0x025D57E0,(u32)actor->mGaugeID);
        u32 global=field<be<u32>>(0x101F8344),controller=field<be<u32>>(global+0x1F4);
        gabi::call(0x020063C0,gabi::at<void>(controller+0x18),gabi::at<void>(0x1048E0DC));
        return;
    }
    if(auctionEventEnded(actor,0x912)) {
        auctionEventFlag();
        actor->m950=7;
        return;
    }
    u8 attention=actor->mNpcEvtInfo.mbAttention;
    if(gabi::call<s32>(0x0259F858,&actor->mNpcEvtInfo)) {
        if(!actor->mNpcEvtInfo.mbAttention) actor->mNpcEvtInfo.mbAttention=attention;
    } else gabi::call(0x0205A104,actor);
}
VERIFY(0x0205A428,Auction_eventMove);

BOOL Auction_execute(daAuction_c* actor) {
    WWHD_FUNC(0x0205AAC8,BOOL,actor);
    gabi::call(0x02059380,actor);
    u32 play=gabi::call<u32>(0x025200D4);
    if(!field<be<u8>>(play+0x5292) || field<be<u16>>(gabi::ea(actor)+0xF8)==1)
        auctionMember(actor,0x10190214+(u32)(u8)actor->mMoveState*8);
    else gabi::call(0x0205A428,actor);
    gabi::call(0x0205A724,actor);
    u32 emitter=actor->mpEmitter;
    if(emitter) {
        u8 flags=actor->m94E;
        f32 step=constant(0x100079AC);
        if(flags&1) {
            s16 delay=actor->m922;
            if(delay) actor->m922=(s16)((u32)(s32)delay-1);
            else {
                f32 alpha=actor->mAlpha;
                f32 next=gabi::fadds_ppc(alpha,step);
                f32 limit=constant(0x100079B0);
                emitter=actor->mpEmitter;
                if(next>limit) {
                    actor->mAlpha=limit;
                    actor->m94E=actor->m94E&0xFE;
                    next=limit;
                } else actor->mAlpha=next;
                field<be<u8>>(emitter+0x247)=(u8)gabi::ftoi(next);
            }
        } else if(flags&2) {
            f32 next=gabi::fsubs_ppc((f32)actor->mAlpha,step);
            f32 zero=constant(0x10007820);
            actor->mAlpha=next;
            emitter=actor->mpEmitter;
            if(next<zero) {
                u32 status=field<be<u32>>(emitter+0x254);
                field<be<u32>>(emitter+0x5C)=0xFFFFFFFF;
                field<be<u32>>(emitter+0x254)=status|1;
                actor->mpEmitter=0;
            } else field<be<u8>>(emitter+0x247)=(u8)gabi::ftoi(next);
        }
    }
    gabi::call(0x02059030,actor);
    return true;
}
VERIFY(0x0205AAC8,Auction_execute);

void Auction_nextBet(daAuction_c* actor) {
    WWHD_FUNC(0x0205AD98,void,actor);
    u8 delay=actor->m949;
    if(delay) { actor->m949=(u8)(delay-1); return; }
    f32 threshold=constant(0x100079B4),maximum=constant(0x10007824),scale=constant(0x10007798);
    for(u32 i=1;i<=6;++i) {
        u32 play=gabi::call<u32>(0x025200D4);
        u32 end=field<be<u32>>(play+0x5CF8);
        play=gabi::call<u32>(0x025200D4);
        u32 start=field<be<u32>>(play+0x5CF4);
        s32 milliseconds=(s32)((end-start)*1000u);
        s32 seconds=milliseconds/30/1000;
        s32 random=gabi::call<s32>(0x02058E88,actor,(s32)(60u-(u32)seconds));
        f32 addition=gabi::fmuls_ppc((f32)random,scale);
        f32 wait=actor->mNpcWait[i];
        if(addition>maximum) addition=maximum;
        f32 next=gabi::fadds_ppc(wait,addition);
        actor->mNpcWait[i]=next;
        if(!(next<threshold)) actor->m949=(u8)((u8)actor->m949+1);
    }
    actor->m949=(u8)((u8)actor->m949+2);
}
VERIFY(0x0205AD98,Auction_nextBet);

void Auction_privateCut(daAuction_c* actor) {
    WWHD_FUNC(0x0205A104,void,actor);
    u32 play=gabi::call<u32>(0x025200D4);
    s32 staff=gabi::call<s32>(0x02542D88,gabi::at<void>(play+0x52C4),STR(0x1000792C),0,0);
    if(staff==-1) return;
    play=gabi::call<u32>(0x025200D4);
    s8 action=(s8)gabi::call<s32>(0x02542EDC,gabi::at<void>(play+0x52C4),staff,gabi::at<void>(0x101903D0),11,1,0);
    actor->mAction=action;
    play=gabi::call<u32>(0x025200D4);
    if(action==-1) {
        gabi::call(0x02543280,gabi::at<void>(play+0x52C4),staff);
        return;
    }
    if(gabi::call<s32>(0x025447C8,gabi::at<void>(play+0x52C4),staff)) {
        action=actor->mAction;
        bool initialized=true;
        switch(action) {
        case 0:gabi::call(0x020594BC,actor,staff);break;
        case 2:gabi::call(0x02059544,actor);break;
        case 3:gabi::call(0x020596F0,actor);break;
        case 4:gabi::call(0x02059A44,actor);break;
        case 5:gabi::call(0x02059B40,actor);break;
        case 6:gabi::call(0x02059B54,actor,staff);break;
        case 7:gabi::call(0x02059BD4,actor);break;
        case 9:gabi::call(0x02059C44,actor);break;
        default:initialized=false;break;
        }
        if(initialized) action=actor->mAction;
    } else action=actor->mAction;
    bool done=true;
    switch(action) {
    case 0:case 7:done=gabi::call<BOOL>(0x02059C64,actor);break;
    case 1:done=gabi::call<BOOL>(0x02059CA8,actor);break;
    case 2:done=gabi::call<BOOL>(0x02059CD8,actor);break;
    case 3:done=gabi::call<BOOL>(0x02059DB4,actor);break;
    case 8:done=gabi::call<BOOL>(0x0205A010,actor);break;
    case 9:done=gabi::call<BOOL>(0x0205A078,actor);break;
    }
    if(done) {
        play=gabi::call<u32>(0x025200D4);
        gabi::call(0x02543280,gabi::at<void>(play+0x52C4),staff);
    }
    u32 camera=gabi::call<u32>(0x024F8044);
    u8 flags=actor->mCameraFlags;
    if(flags&1) {
        gabi::call(0x02514F2C,gabi::at<void>(camera));
        flags=((u8)actor->mCameraFlags&0xFE)|4;
        actor->mCameraFlags=flags;
    }
    if(flags&2) {
        gabi::call(0x02514F38,gabi::at<void>(camera));
        flags=(u8)actor->mCameraFlags&0xF1;
        actor->mCameraFlags=flags;
    }
    if((flags&4) && (flags&8)) {
        gabi::Local<cXyz> eye,center;
        center->z=actor->mParticleScale.z;
        eye->x=actor->mParticlePos.x;
        eye->y=actor->mParticlePos.y;
        center->x=actor->mParticleScale.x;
        eye->z=actor->mParticlePos.z;
        center->y=actor->mParticleScale.y;
        gabi::call(0x02514F50,gabi::at<void>(camera),eye.get(),center.get());
    }
}
VERIFY(0x0205A104,Auction_privateCut);

bool Auction_eventMain(daAuction_c* actor) {
    WWHD_FUNC(0x02059DB4,bool,actor);
    u32 play=gabi::call<u32>(0x025200D4);
    u32 link=field<be<u32>>(play+0x5B34);
    u32 vt=field<be<u32>>(link+0xB4);
    f32 rate=gabi::call<f32>(field<be<u32>>(vt+0x9C),gabi::at<void>(link));
    if(rate==constant(0x10007820)) {
        u8 animation=actor->mCurLinkAnm;
        if(animation!=1 && animation!=29 && animation!=37)
            gabi::call(0x02059D50,actor,1);
    }
    actor->mFlags=(u8)actor->mFlags&4;
    play=gabi::call<u32>(0x025200D4);
    if(field<be<u32>>(play+0x5CFC)==4) {
        play=gabi::call<u32>(0x025200D4);
        u32 timer=field<be<u32>>(play+0x5CF0);
        if(timer) gabi::call(0x025C5794,gabi::at<void>(timer),2);
    }
    auctionMember(actor,0x1019022C+(u8)actor->m937*8u);
    if(actor->m94A) field<be<u32>>(link+0x3B4)=(u8)actor->mFace;
    if(actor->m93A==1) {
        s16 wait=(s16)gabi::ftoi(actor->mNpcWait[0]);
        play=gabi::call<u32>(0x025200D4);
        field<be<s16>>(play+0x5BA8)=wait;
    }
    if(actor->m937>1 || !actor->m943) return false;
    u8 kind=field<be<u8>>(gabi::ea(actor)+0x92C+(u8)actor->m93C);
    if(kind>=12) {
        gabi::call(0x0273AA24,gabi::at<void>(0x100078D8),0x95A,gabi::at<void>(0x100078E8));
        kind=0;
    }
    s32 name=field<be<s16>>(0x10190304+kind*10u);
    play=gabi::call<u32>(0x025200D4);
    field<be<s32>>(play+0x5B54)=name;
    play=gabi::call<u32>(0x025200D4);
    if(field<be<u32>>(play+0x5CFC)==4) {
        play=gabi::call<u32>(0x025200D4);
        u32 timer=field<be<u32>>(play+0x5CF0);
        if(timer) gabi::call(0x025C5794,gabi::at<void>(timer),2);
    }
    gabi::call(0x0261BC50);
    gabi::call(0x0261BC80);
    u8 flags=actor->m94E;
    actor->m943=1; actor->m94C=0; actor->m94E=flags|2;
    field<be<s16>>(link+0x420)=2;
    field<be<u32>>(link+0x430)=1;
    u8 camera=actor->mCameraFlags;
    if(camera&4) actor->mCameraFlags=(camera|2)&0xFE;
    gabi::call(0x025E1988,0x8DE);
    s16 bid=actor->mCurrBid;
    play=gabi::call<u32>(0x025200D4);
    field<be<s16>>(play+0x5BA0)=bid;
    return true;
}
VERIFY(0x02059DB4,Auction_eventMain);

void Auction_eventOrder(daAuction_c* actor) {
    WWHD_FUNC(0x0205A724,void,actor);
    if(actor->m939==1) {
        actor->m950=3; actor->mMoveState=2;
        u8 request=actor->m939;
        actor->m939=0; actor->m93A=request;
    } else if(actor->m939==2) {
        u8 request=actor->m939;
        actor->mMoveState=2; actor->m939=0;
        actor->m950=6; actor->m93A=request;
    }
    s8 order=actor->m950;
    if(order==1 || order==2) {
        s8 current=actor->m950;
        field<be<u16>>(gabi::ea(actor)+0xFA)=field<be<u16>>(gabi::ea(actor)+0xFA)|1;
        if(current==1) gabi::call(0x025D76A8,actor);
        return;
    }
    if(order==3) {
        u32 play=field<be<u32>>(0x101F84DC);
        if(gabi::call<s32>(0x025B8B94,gabi::at<void>(play+0x644),0x4008)) {
            play=field<be<u32>>(0x101F84DC);
            u32 data=gabi::call<u32>(0x02720118,gabi::at<void>(play+0x12C0));
            s16 bid=actor->mCurrBid;
            u8 index=field<be<u8>>(data+0x11);
            actor->mCurrBid=bid+10;
            actor->mCurrAuctionItemIndex=index;
            s32 message=field<be<s16>>(0x1019016C+index*8u+2);
            play=gabi::call<u32>(0x025200D4);
            field<be<s32>>(play+0x5B58)=message;
        } else {
            u32 index=gabi::call<u32>(0x0205A67C,actor);
            actor->mCurrAuctionItemIndex=index;
            s32 message=field<be<s16>>(0x1019016C+index*8u+2);
            play=gabi::call<u32>(0x025200D4);
            field<be<s32>>(play+0x5B58)=message;
        }
        s16 bid=field<be<s16>>(0x1019016C+(u8)actor->mCurrAuctionItemIndex*8u+4);
        actor->mCurrBid=bid;
        play=gabi::call<u32>(0x025200D4);
        field<be<s16>>(play+0x5BA0)=bid;
        play=gabi::call<u32>(0x025200D4);
        gabi::call(0x025D7970,gabi::at<void>(field<be<u32>>(play+0x5B2C)),actor,(s32)(s16)actor->mEvtStartIdx,0,0xFF7F);
        play=gabi::call<u32>(0x025200D4);
        u16 flags=field<be<u16>>(play+0x5CE8);
        field<be<u8>>(play+0x5CEA)=5;
        field<be<u16>>(play+0x5CE8)=flags|0x10;
        return;
    }
    if(order==4 || order==5 || order==7) {
        u32 offset=order==4?0x90E:order==5?0x910:0x914;
        s32 event=field<be<s16>>(gabi::ea(actor)+offset);
        gabi::call(0x025D7A58,actor,event,0xFF,0xFF7F,0,1);
        return;
    }
    if(order==6) {
        u8 index=(u8)actor->mCurrAuctionItemIndex&1;
        actor->mCurrAuctionItemIndex=index;
        s16 bid=field<be<s16>>(0x10190194+index*8u+4);
        s32 message=field<be<s16>>(0x10190194+index*8u+2);
        actor->mCurrBid=bid;
        u32 play=gabi::call<u32>(0x025200D4);
        field<be<s32>>(play+0x5B58)=message;
        bid=actor->mCurrBid;
        play=gabi::call<u32>(0x025200D4);
        field<be<s16>>(play+0x5BA0)=bid;
        play=gabi::call<u32>(0x025200D4);
        gabi::call(0x025D7970,gabi::at<void>(field<be<u32>>(play+0x5B2C)),actor,(s32)(s16)actor->mEvtStart2Idx,0,0xFF7F);
        play=gabi::call<u32>(0x025200D4);
        u16 flags=field<be<u16>>(play+0x5CE8);
        field<be<u8>>(play+0x5CEA)=5;
        field<be<u16>>(play+0x5CE8)=flags|0x10;
    }
}
VERIFY(0x0205A724,Auction_eventOrder);

// WWHD Auction event contribution.
namespace {
template<class T> T& eventField(u32 address) { return *gabi::at<T>(address); }
f32 eventConstant(u32 address) { return eventField<be<f32>>(address); }
}

f32 Auction_getPiconDispOfs(daAuction_c* actor,s32 index) {
    WWHD_FUNC(0x0205C1CC,f32,actor,index);
    if(!index) return eventConstant(0x101901A4);
    u32 npc=gabi::call<u32>(0x02059854,actor,index);
    if(npc) return eventField<be<f32>>(npc+0x894);
    gabi::call(0x0273AA24,STR(0x10007A7C),0xDDE,STR(0x10007A78));
    return eventConstant(0x10007820);
}
VERIFY(0x0205C1CC,Auction_getPiconDispOfs);

void Auction_eventMainMsgBikonW(daAuction_c* actor) {
    WWHD_FUNC(0x0205C4B4,void,actor);
    s16 timer=actor->mTimer;
    if(timer) actor->mTimer=(s16)((u16)timer-1);
    else {
        gabi::call(0x0205AD6C,actor,(s32)actor->mBidLimit);
        gabi::call(0x020598BC,actor,(u32)actor->m93F,0);
        gabi::call(0x0261BC08);
        if(actor->m93F) actor->m94C=actor->m94C|1;
    }
    u32 play=gabi::call<u32>(0x025200D4);
    eventField<be<u8>>(play+0x5BBA)=0;
    play=gabi::call<u32>(0x025200D4);
    eventField<be<u8>>(play+0x5BB9)=0x3E;
}
VERIFY(0x0205C4B4,Auction_eventMainMsgBikonW);

void Auction_eventMainMsgEnd(daAuction_c* actor) {
    WWHD_FUNC(0x0205C014,void,actor);
    if(gabi::call<s32>(0x02059CA8,actor)) {
        u8 animation=actor->mCurLinkAnm;
        if(animation!=1 && animation!=29) gabi::call(0x02059D50,actor,1);
        u8 flags=actor->m94C;
        if(flags&0x20) {
            actor->m937=4;
            actor->mBidSpeed=eventConstant(0x10007820);
        } else {
            // Integer loads preserve vector bits, including signaling NaNs.
            u32 y=eventField<be<u32>>(gabi::ea(actor)+0x318);
            u8 mode=actor->m93A;
            eventField<be<u32>>(gabi::ea(actor)+0x380)=y;
            u32 x=eventField<be<u32>>(gabi::ea(actor)+0x314);
            flags=actor->m94C;
            eventField<be<u32>>(gabi::ea(actor)+0x37C)=x;
            u32 z=eventField<be<u32>>(gabi::ea(actor)+0x31C);
            actor->m937=mode==2;
            eventField<be<u32>>(gabi::ea(actor)+0x384)=z;
            actor->m94A=0;
            actor->mBidSpeed=eventConstant(0x10007820);
        }
        if(!(flags&2)) {
            u32 camera=gabi::call<u32>(0x02058E88,actor,3)&0xFF;
            actor->m947=camera;
            u32 source=0x10461544+camera*24;
            for(u32 i=0;i<6;++i)
                eventField<be<u32>>(gabi::ea(actor)+0x8A4+i*4)=eventField<be<u32>>(source+i*4);
        }
        gabi::call(0x025C5E54,(u32)actor->mGaugeID,0);
        gabi::call(0x0261CF64);
        actor->m94C=actor->m94C&0xD6;
        gabi::call(0x0261BC20);
        gabi::call(0x0261BC68);
        return;
    }
    u32 play=gabi::call<u32>(0x025200D4);
    if(eventField<be<u8>>(play+0x5BD2)) {
        u32 message=actor->mBidStep;
        if(message-0x1D3Au<0x12 || message-0x339Du<0xC) {
            gabi::call(0x020598BC,actor,(u32)actor->m93D,0);
            u8 flags=actor->m94C;
            u8 camera=actor->m93D;
            actor->m94C=flags|8;
            actor->m941=camera;
        }
    }
}
VERIFY(0x0205C014,Auction_eventMainMsgEnd);

void Auction_eventMainMsgBikonC(daAuction_c* actor) {
    WWHD_FUNC(0x0205C248,void,actor);
    u32 self=gabi::ea(actor);
    u8 bidder=actor->m940;
    s16 bid=actor->m918;
    actor->m93F=bidder;
    actor->m941=bidder;
    u32 play=gabi::call<u32>(0x025200D4);
    eventField<be<s16>>(play+0x5BA0)=bid;
    u32 kind=eventField<be<u8>>(self+0x92C+(u8)actor->m93F);
    if(kind>=12) {
        gabi::call(0x0273AA24,STR(0x10007A9C),0xB5D,STR(0x10007AAC));
        kind=0;
    }
    s16 name=eventField<be<s16>>(0x10190304+kind*10);
    play=gabi::call<u32>(0x025200D4);
    eventField<be<s32>>(play+0x5B54)=name;
    u32 npc=gabi::call<u32>(0x02059854,actor,(u32)actor->m93F);
    gabi::Local<cXyz> position;
    position->x=eventField<be<f32>>(npc+0x314);
    f32 y=eventField<be<f32>>(npc+0x318);
    position->y=y;
    u8 index=actor->m93F;
    position->z=eventField<be<f32>>(npc+0x31C);
    f32 height=gabi::call<f32>(0x0205C1CC,actor,(u32)index);
    f32 offset=height+eventConstant(0x10007A8C);
    f32 scale=eventConstant(0x10007A90);
    f32 maximum=eventConstant(0x10007A94);
    f32 minimum=eventConstant(0x10007A98);
    position->y=y+offset;
    f32 distance=gabi::call<f32>(0x0200EE00,&actor->mParticlePos,position.get(),scale,maximum,minimum);
    if(distance==eventConstant(0x10007820)) {
        actor->m937=5;
        npc=gabi::call<u32>(0x02059854,actor,(u32)actor->m93F);
        for(u32 i=0;i<3;++i) eventField<be<u32>>(self+0x8C0+i*4)=eventField<be<u32>>(npc+0x314+i*4);
        height=gabi::call<f32>(0x0205C1CC,actor,(u32)actor->m93F);
        f32 itemY=actor->mItemPos.y;
        s8 room=eventField<be<s8>>(self+0x326);
        actor->mItemPos.y=itemY+height;
        play=gabi::call<u32>(0x025200D4);
        u32 particles=eventField<be<u32>>(play+0x5AB0);
        gabi::call(0x025A847C,particles,0,0x8153,&actor->mItemPos,0,0,0xFF,0,(s32)room,0,0,0);
        actor->mTimer=30;
        if(!actor->m93F) gabi::call(0x025E1988,0x8EB);
        else {
            gabi::call(0x025E1988,0x591F);
            actor->m94A=1;
            u32 face=gabi::call<u32>(0x02058E88,actor,8);
            u8 npcIndex=actor->m93F;
            actor->mFace=eventField<be<u32>>(0x101901F4+face*4);
            npc=gabi::call<u32>(0x02059854,actor,(u32)npcIndex);
            for(u32 i=0;i<3;++i) eventField<be<u32>>(self+0x37C+i*4)=eventField<be<u32>>(npc+0x37C+i*4);
            play=gabi::call<u32>(0x025200D4);
            eventField<be<u8>>(play+0x5BBA)=0;
            play=gabi::call<u32>(0x025200D4);
            eventField<be<u8>>(play+0x5BB9)=0x3E;
            return;
        }
        npc=gabi::call<u32>(0x02059854,actor,(u32)actor->m93F);
        for(u32 i=0;i<3;++i) eventField<be<u32>>(self+0x37C+i*4)=eventField<be<u32>>(npc+0x37C+i*4);
    }
    play=gabi::call<u32>(0x025200D4);
    eventField<be<u8>>(play+0x5BBA)=0;
    play=gabi::call<u32>(0x025200D4);
    eventField<be<u8>>(play+0x5BB9)=0x3E;
}
VERIFY(0x0205C248,Auction_eventMainMsgBikonC);

namespace {
s32 eventTimeRemaining() {
    u32 play=gabi::call<u32>(0x025200D4);
    u32 end=eventField<be<u32>>(play+0x5CF8);
    play=gabi::call<u32>(0x025200D4);
    u32 start=eventField<be<u32>>(play+0x5CF4);
    return (s32)((end-start)*1000u)/30;
}
void eventRestartTimer() {
    u32 play=gabi::call<u32>(0x025200D4);
    if(eventField<be<s32>>(play+0x5CFC)==4) {
        play=gabi::call<u32>(0x025200D4);
        u32 timer=eventField<be<u32>>(play+0x5CF0);
        if(timer) gabi::call(0x025C57D4,timer,2);
    }
}
}
void Auction_eventMainUri(daAuction_c* actor) {
    WWHD_FUNC(0x0205B9C0,void,actor);
    eventRestartTimer();
    if(eventTimeRemaining()<=0) actor->m943=1;
    else if(actor->m946 && gabi::call<s32>(0x02007898,0))
        gabi::call(0x0205AD6C,actor,0x1D1A);
    else {
        f32 doubleRate=eventConstant(0x100079C4);
        s16 wait=actor->m91E;
        if(wait) actor->m91E=(s16)((u16)wait-1);
        else for(u32 i=1;i<7;++i) {
            u32 row=0x10190294+i*16;
            f32 low=eventField<be<f32>>(row);
            f32 high=eventField<be<f32>>(row+4);
            f32 random=gabi::call<f32>(0x020198D8,high-low);
            low=eventField<be<f32>>(row);
            f32 speed=random+low;
            f32 value=actor->mNpcWait[i];
            actor->mNpcWait[i]=value+(speed+speed);
        }
        f32 limit=eventConstant(0x100079B4);
        f32 zero=eventConstant(0x10007820);
        u32 bidder=1;
        for(;bidder<7;++bidder) {
            if(!(actor->mNpcWait[bidder]<limit)) {
                actor->mNpcWait[bidder]=zero;
                if((s16)actor->mCurrBid<eventField<be<s16>>(0x10190294+bidder*16+12)) break;
            }
        }
        if(bidder<7) {
            actor->m940=bidder;
            f32 unit=eventConstant(0x10007798);
            f32 multiplier=unit;
            if(eventTimeRemaining()<30000) multiplier=doubleRate;
            else if(eventTimeRemaining()<60000) multiplier=eventConstant(0x100079C8);
            u32 row=0x10190294+bidder*16;
            s16 low=eventField<be<s16>>(row+8);
            s16 high=eventField<be<s16>>(row+10);
            f32 random=gabi::call<f32>(0x020198D8,(f32)((s32)high-low));
            low=eventField<be<s16>>(row+8);
            s16 base=(s16)gabi::ftoi(random+(f32)low);
            f32 amount=(f32)((s32)base*2)*multiplier;
            actor->m944=4;
            actor->m946=1;
            actor->mBidLimit=0x1CF9;
            s16 old=actor->mCurrBid;
            s16 increment=(s16)gabi::ftoi(amount);
            s16 total=(s16)((u16)old+(u16)increment);
            actor->mCurrBid=total;
            actor->m918=total;
            random=gabi::call<f32>(0x020198D8,unit);
            f32 duration=random*eventConstant(0x100079CC);
            actor->m937=4;
            u8 shown=actor->m948;
            actor->m91E=(s16)gabi::ftoi(duration);
            if(shown) actor->m948=0;
            else if(eventTimeRemaining()>eventField<be<s16>>(0x10190150)
                    && !gabi::call<s32>(0x02058E88,actor,3)) {
                s16 bid=actor->mCurrBid;
                u32 message=bid<=100?0x1CFD:bid<=150?0x1CFE:bid<=200?0x1CFF:0x1D00;
                u32 kind=eventField<be<u8>>(gabi::ea(actor)+0x92C+(u8)actor->m93C);
                if(kind>=12) {
                    gabi::call(0x0273AA24,STR(0x10007A28),0xAB3,STR(0x10007A38));kind=0;
                }
                s16 name=eventField<be<s16>>(0x10190304+kind*10);
                u32 play=gabi::call<u32>(0x025200D4);
                eventField<be<s32>>(play+0x5B54)=name;
                gabi::call(0x0205AD6C,actor,message);
                gabi::call(0x020598BC,actor,(u32)actor->m93C,0);
                if(!actor->m93C) gabi::call(0x02059D50,actor,20);
                u8 flags=actor->m94C;
                u8 camera=actor->m93F;
                actor->m94C=flags|0x20;
                actor->m941=camera;
                actor->m948=1;
            }
            if(actor->m937==4) {
                u8 selected=actor->m940;
                s16 bid=actor->m918;
                actor->m941=selected;actor->m93F=selected;
                u32 play=gabi::call<u32>(0x025200D4);
                eventField<be<s16>>(play+0x5BA0)=bid;
                u32 kind=eventField<be<u8>>(gabi::ea(actor)+0x92C+(u8)actor->m93F);
                if(kind>=12) {
                    gabi::call(0x0273AA24,STR(0x10007A28),0xACC,STR(0x10007A38));kind=0;
                }
                s16 name=eventField<be<s16>>(0x10190304+kind*10);
                play=gabi::call<u32>(0x025200D4);
                eventField<be<s32>>(play+0x5B54)=name;
            }
            actor->mCameraFlags=(actor->mCameraFlags|9)&0xFD;
        } else {
            s32 remaining=eventTimeRemaining();
            u8 next=actor->m942;
            if(remaining<eventField<be<s16>>(0x10190158+next*2)) {
                gabi::call(0x0205AD6C,actor,(u32)eventField<be<u32>>(0x1019027C+next*4));
                next=actor->m942;
                gabi::call(0x025E1988,(u32)eventField<be<u32>>(0x10190288+next*4));
                actor->m942=(u8)actor->m942+1;
            }
        }
    }
    if(actor->m946) {
        u32 play=gabi::call<u32>(0x025200D4);
        eventField<be<u8>>(play+0x5BBA)=0x25;
    }
}
VERIFY(0x0205B9C0,Auction_eventMainUri);

u16 Auction_nextMsgStatus(daAuction_c* actor,be<u32>* message) {
    WWHD_FUNC(0x0205C55C,u16,actor,message);
    f32 zero=eventConstant(0x10007820);
    u32 number=*message;
    // HD loads the message singleton before dispatch, even on unrelated cases.
    u32 manager=eventField<be<u32>>(0x101F4B5C);
    u16 status=15;
    switch(number) {
    case 0x1CF2: *message=0x1CF3;break;
    case 0x1CF4:
        if(actor->m93A==2) *message=0x1D1B;
        else {
            u32 save=eventField<be<u32>>(0x101F84DC);
            if(gabi::call<s32>(0x025B8B94,save+0x644,0x4008)) {
                save=eventField<be<u32>>(0x101F84DC);
                u32 kind=gabi::call<u32>(0x025B8BB0,save+0x644,0x790F)&0xFF;
                if(kind>=12) {gabi::call(0x0273AA24,STR(0x10007AF0),0xC7E,STR(0x10007B00));kind=0;}
                s16 name=eventField<be<s16>>(0x10190304+kind*10);
                u32 play=gabi::call<u32>(0x025200D4);
                eventField<be<s32>>(play+0x5B54)=name;
                *message=0x1CF6;
            } else *message=0x1CF5;
        } break;
    case 0x1CF7: case 0x1CF9: {
        s16 bid=actor->mCurrBid;
        u32 play=gabi::call<u32>(0x025200D4);
        eventField<be<s16>>(play+0x5CEC)=bid;
        bid=actor->mCurrBid;
        play=gabi::call<u32>(0x025200D4);
        eventField<be<s16>>(play+0x5BA6)=bid;
        if(number==0x1CF7) {
            gabi::call(0x0261BC38);*message=0x1CF8;break;
        }
        u32 after=actor->m944;
        if(after<4) *message=eventField<be<u32>>(0x101901D4+after*4);
        else {
            u32 kind=eventField<be<u8>>(gabi::ea(actor)+0x92C+(u8)actor->m93C);
            if(kind>=12) {gabi::call(0x0273AA24,STR(0x10007AF0),0xC9B,STR(0x10007B00));kind=0;}
            if(!actor->m93F && eventField<be<s16>>(0x10190304+kind*10+2)) {
                u32 row=0x10190304+kind*10;
                // HD threshold is 10000ms, unlike the GC source's 60000ms.
                u32 offset=eventTimeRemaining()<10000?4:2;
                *message=(s32)eventField<be<s16>>(row+offset);
            } else status=16;
        }
        u8 previous=actor->m93C;
        u8 selected=actor->m93F;
        actor->m93D=previous;actor->m93E=selected;actor->m93C=selected;
        if(selected) gabi::call(0x02059D50,actor,1);
        break;
    }
    case 0x1CFA: {
        u32 play=gabi::call<u32>(0x025200D4);
        u32 save=eventField<be<u32>>(0x101F84DC);
        s16 bid=eventField<be<s16>>(play+0x5BA2);
        u16 rupees=eventField<be<u16>>(save+0x24);
        f32 half=eventConstant(0x10007824);
        if((s32)rupees<bid || bid<=(s16)actor->mCurrBid) {
            bool insufficient=(s32)rupees<bid;
            gabi::call(0x025E1988,0x8E5);
            gabi::call(0x02059D50,actor,74);
            *message=insufficient?0x1CFB:0x1D1E;
            actor->mNpcWait[0]=half;
        } else {
            gabi::call(0x025E1988,0x8E3);
            actor->mNpcWait[0]=zero;
            if(bid==999) {
                *message=0x1D24;
                u8 previous=actor->m93C;
                u8 selected=actor->m93F;
                actor->m93D=previous;actor->m93E=selected;
                actor->mCurrBid=bid;actor->m93C=selected;
            } else {
                f32 frames=eventConstant(0x100079CC);
                *message=0x1CF9;actor->m944=0;
                for(u32 i=0;i<4;++i) {
                    f32 old=(f32)(s16)actor->mCurrBid;
                    f32 rate=eventField<be<f32>>(0x101901C4+i*4);
                    s16 minimum=(s16)gabi::ftoi(old*rate);
                    if(bid>=minimum && (s16)actor->mCurrBid>=eventField<be<s16>>(0x10190160+i*2)) {
                        u32 row=0x101901E4+i*4;
                        s16 low=eventField<be<s16>>(row);
                        s16 high=eventField<be<s16>>(row+2);
                        f32 random=gabi::call<f32>(0x020198D8,(f32)((s32)high-low));
                        u32 selected=actor->m944;
                        low=eventField<be<s16>>(0x101901E4+selected*4);
                        actor->m91E=(s16)gabi::ftoi((random+(f32)low)*frames);
                        break;
                    }
                    actor->m944=i+1;
                }
                actor->mCurrBid=bid;
            }
            play=gabi::call<u32>(0x025200D4);
            eventField<be<s16>>(play+0x5BA0)=bid;
        }break;
    }
    case 0x1CFC:
        if(!eventField<be<u32>>(manager+0x948)) *message=0x1D1F;
        else status=16;break;
    case 0x1D1A:
        if(!eventField<be<u32>>(manager+0x948)) actor->m943=1;
        status=16;break;
    case 0x1D1F:
        gabi::call(0x0252012C,STR(0x10007AEC),3,11,-1,0,1,0,zero);
        [[fallthrough]];
    case 0x1D24: {
        s16 bid=actor->mCurrBid;
        u32 play=gabi::call<u32>(0x025200D4);
        eventField<be<s16>>(play+0x5CEC)=bid;
        bid=actor->mCurrBid;
        play=gabi::call<u32>(0x025200D4);
        eventField<be<s16>>(play+0x5BA6)=bid;
        actor->m943=1;status=16;break;
    }
    case 0x1D20:case 0x1D21:case 0x1D22:case 0x1D23: {
        gabi::call(0x02059D50,actor,20);
        u32 selected=gabi::call<u32>(0x02058E88,actor,6)+1;
        u32 kind=eventField<be<u8>>(gabi::ea(actor)+0x92C+(selected&0xFF));
        if(kind>=12) {gabi::call(0x0273AA24,STR(0x10007AF0),0xCFF,STR(0x10007B00));kind=0;}
        *message=(s32)eventField<be<s16>>(0x10190304+kind*10+6);
        actor->m93D=selected;break;
    }
    case 0x1D48:case 0x33A2:case 0x33A8:case 0x33A5:case 0x339F:
    case 0x1D4B:case 0x1D3F:case 0x1D45:case 0x1D42:case 0x1D3C:
        actor->m920=0;actor->m94C=actor->m94C|2;status=16;break;
    case 0x1D05:
        if(actor->m93A==2) *message=0x1D1C;
        else {
            u32 save=eventField<be<u32>>(0x101F84DC);
            if(actor->m93C) {
                gabi::call(0x025B8B68,save+0x644,0x4008);
                u8 selected=actor->m93C;
                save=eventField<be<u32>>(0x101F84DC);
                u8 kind=eventField<be<u8>>(gabi::ea(actor)+0x92C+selected);
                gabi::call(0x025B8AF4,save+0x644,0x790F,(u32)kind);
                save=eventField<be<u32>>(0x101F84DC);
                u32 item=gabi::call<u32>(0x02720118,save+0x12C0);
                eventField<be<u8>>(item+0x11)=actor->mCurrAuctionItemIndex;
            } else gabi::call(0x025B8B7C,save+0x644,0x4008);
            gabi::call(0x025D57E4,(u32)actor->mCurrAuctionItemPID);status=16;
        } break;
    case 0x1D1C: case 0x1D07: {
        s16 bid=actor->mCurrBid;
        u32 play=gabi::call<u32>(0x025200D4);
        u32 old=eventField<be<u32>>(play+0x5B48);
        eventField<be<u32>>(play+0x5B48)=number==0x1D1C?old+(s32)bid:old-(s32)bid;
        if(number==0x1D1C)status=16;else *message=0x1D08;
        break;
    }
    case 0x1D1D:case 0x1D08:*message=0x1D09;break;
    default:status=16;break;
    }
    actor->mBidStep=status==15?(u32)*message:0;
    return status;
}
VERIFY(0x0205C55C,Auction_nextMsgStatus);

void Auction_eventMainKai(daAuction_c* actor) {
    WWHD_FUNC(0x0205AF48,void,actor);
    eventRestartTimer();
    if(eventTimeRemaining()<=0) actor->m943=1;
    else {
        s32 cancel=gabi::call<s32>(0x020078BC,0);
        f32 zero=eventConstant(0x10007820);
        f32 limit=eventConstant(0x100079B4);
        if(cancel) {
            gabi::call(0x0205AD6C,actor,0x1CFC);
            gabi::call(0x025E1988,0x8E6);
        } else {
            f32 speed=actor->mBidSpeed;
            f32 increment=actor->mGaugeValue;
            f32 next=speed+increment;
            f32 one=eventConstant(0x10007798);
            f32 negative=eventConstant(0x100079B8);
            if(!(next<one)) {actor->mBidSpeed=one;actor->mGaugeValue=negative;}
            else if(next>negative) actor->mBidSpeed=next;
            else {actor->mBidSpeed=negative;actor->mGaugeValue=one;}
            f32 gauge=actor->mNpcWait[0];
            if(gauge>zero && gauge<limit) actor->mNpcWait[0]=gauge+eventConstant(0x100079BC);
            if(gabi::call<s32>(0x02007898,0)) {
                f32 low=eventField<be<f32>>(0x10190294);
                f32 high=eventField<be<f32>>(0x10190298);
                f32 random=gabi::call<f32>(0x020198D8,high-low);
                low=eventField<be<f32>>(0x10190294);
                gauge=actor->mNpcWait[0];
                gauge=gauge+(random+low);
                f32 unsignedBoundary=eventConstant(0x100079C0);
                actor->mNpcWait[0]=gauge;
                u32 soundParameter=gauge<unsignedBoundary?(u32)gabi::ftoi(gauge):
                    (u32)gabi::ftoi(gauge-eventConstant(0x100079C0))+0x80000000;
                gabi::call(0x025E1A04,0x8E4,0,soundParameter);
            }
            if(actor->mNpcWait[0]>limit) actor->mNpcWait[0]=limit;
            s16 wait=actor->m91E;
            if(wait) {
                s16 nextWait=(s16)((u16)wait-1);actor->m91E=nextWait;
                if(!nextWait) {
                    u8 flags=actor->m94C;
                    if(flags&2) {
                        u32 camera=gabi::call<u32>(0x02058E88,actor,3)&0xFF;
                        actor->m947=camera;
                        u32 source=0x10461544+camera*24;
                        for(u32 i=0;i<3;++i) eventField<be<u32>>(gabi::ea(actor)+0x8A4+i*4)=eventField<be<u32>>(source+i*4);
                        flags=actor->m94C;
                        for(u32 i=3;i<6;++i) eventField<be<u32>>(gabi::ea(actor)+0x8A4+i*4)=eventField<be<u32>>(source+i*4);
                    }
                    actor->m94C=flags&0xF9;
                }
            } else for(u32 i=1;i<7;++i) {
                u32 row=0x10190294+i*16;
                f32 low=eventField<be<f32>>(row);
                f32 high=eventField<be<f32>>(row+4);
                f32 random=gabi::call<f32>(0x020198D8,high-low);
                low=eventField<be<f32>>(row);
                f32 addition=random+low;
                actor->mNpcWait[i]=(f32)actor->mNpcWait[i]+addition;
            }
            u32 end=(actor->m94C&2)?1:7;
            u32 bidder=0;
            for(;bidder<end;++bidder)
                if(!(actor->mNpcWait[bidder]<limit) && (s16)actor->mCurrBid<eventField<be<s16>>(0x10190294+bidder*16+12))break;
            if(bidder<end) {
                s16 previous=actor->mCurrBid;
                actor->m940=bidder;
                u32 message;
                if(bidder) {
                    f32 multiplier=one;
                    if(eventTimeRemaining()<30000) multiplier=eventConstant(0x100079C4);
                    else if(eventTimeRemaining()<60000) multiplier=eventConstant(0x100079C8);
                    u32 row=0x10190294+bidder*16;
                    s16 low=eventField<be<s16>>(row+8);
                    s16 high=eventField<be<s16>>(row+10);
                    f32 random=gabi::call<f32>(0x020198D8,(f32)((s32)high-low));
                    low=eventField<be<s16>>(row+8);
                    s16 amount=(s16)gabi::ftoi(random+(f32)low);
                    s16 increase=(s16)gabi::ftoi((f32)amount*multiplier);
                    s16 current=actor->mCurrBid;
                    actor->m944=4;
                    s16 total=(s16)((u16)current+(u16)increase);
                    actor->mCurrBid=total;actor->m918=total;
                    actor->mNpcWait[bidder]=zero;
                    gabi::call(0x0205AD98,actor);
                    message=0x1CF9;
                } else {
                    s16 current=actor->mCurrBid;
                    actor->m918=(s16)((u16)previous+1);
                    u32 play=gabi::call<u32>(0x025200D4);
                    eventField<be<s16>>(play+0x5BA2)=(s16)((u16)current+1);
                    gabi::call(0x02059D50,actor,72);
                    gabi::call(0x025C5E54,(u32)actor->mGaugeID,1);
                    gabi::call(0x0261CF60);
                    play=gabi::call<u32>(0x025200D4);
                    gabi::Local<cXyz> direction;
                    direction->x=zero;direction->y=one;direction->z=zero;
                    gabi::call(0x025CB374,play+0x599C,5,1,direction.get());
                    play=gabi::call<u32>(0x025200D4);
                    direction->x=zero;direction->y=one;direction->z=zero;
                    gabi::call(0x025CB374,play+0x599C,4,0x3E,direction.get());
                    message=0x1CFA;
                }
                f32 random=gabi::call<f32>(0x020198D8,zero);
                f32 frames=(random+one)*eventConstant(0x100079CC);
                u8 shown=actor->m948;
                actor->m937=4;actor->mBidLimit=message;
                actor->m91E=(s16)gabi::ftoi(frames);
                if(shown) actor->m948=0;
                else if(actor->m93E!=0xFF && bidder && eventTimeRemaining()>eventField<be<s16>>(0x10190150)
                         && !gabi::call<s32>(0x02058E88,actor,3)) {
                    s16 bid=actor->mCurrBid;
                    message=bid<=100?0x1CFD:bid<=150?0x1CFE:bid<=200?0x1CFF:0x1D00;
                    u32 play=gabi::call<u32>(0x025200D4);
                    eventField<be<s16>>(play+0x5BA0)=previous;
                    u32 kind=eventField<be<u8>>(gabi::ea(actor)+0x92C+(u8)actor->m93C);
                    if(kind>=12){gabi::call(0x0273AA24,STR(0x100079D8),0x9FA,STR(0x100079E8));kind=0;}
                    s16 name=eventField<be<s16>>(0x10190304+kind*10);
                    play=gabi::call<u32>(0x025200D4);
                    eventField<be<s32>>(play+0x5B54)=name;
                    gabi::call(0x0205AD6C,actor,message);
                    gabi::call(0x020598BC,actor,(u32)actor->m93C,0);
                    if(!actor->m93C && actor->mCurLinkAnm!=29) gabi::call(0x02059D50,actor,20);
                    actor->m94C=actor->m94C|0x20;
                    actor->m941=(u8)actor->m93F;actor->m948=1;
                }
                if(actor->m937==4) {
                    u8 selected=actor->m940;
                    s16 bid=actor->m918;
                    actor->m941=selected;actor->m93F=selected;
                    u32 play=gabi::call<u32>(0x025200D4);
                    eventField<be<s16>>(play+0x5BA0)=bid;
                    u32 kind=eventField<be<u8>>(gabi::ea(actor)+0x92C+(u8)actor->m93F);
                    if(kind>=12){gabi::call(0x0273AA24,STR(0x100079D8),0xA13,STR(0x100079E8));kind=0;}
                    s16 name=eventField<be<s16>>(0x10190304+kind*10);
                    play=gabi::call<u32>(0x025200D4);
                    eventField<be<s32>>(play+0x5B54)=name;
                }
                actor->mCameraFlags=(actor->mCameraFlags|9)&0xFD;
            } else {
                s32 remaining=eventTimeRemaining();u8 next=actor->m942;
                if(remaining<eventField<be<s16>>(0x10190150+next*2)) {
                    gabi::call(0x0205AD6C,actor,(u32)eventField<be<u32>>(0x1019027C+next*4));
                    next=actor->m942;
                    gabi::call(0x025E1988,(u32)eventField<be<u32>>(0x10190288+next*4));
                    actor->m942=(u8)actor->m942+1;
                }
            }
        }
        if(actor->m94C&2) {
            actor->mParticleScale.x=zero;
            actor->mParticleScale.y=eventConstant(0x10007824);
            actor->mParticleScale.z=eventConstant(0x100079D0);
            actor->m94C=actor->m94C|4;
            u32 npc=gabi::call<u32>(0x02059854,actor,0);
            for(u32 i=0;i<3;++i)eventField<be<u32>>(gabi::ea(actor)+0x8A4+i*4)=eventField<be<u32>>(npc+0x314+i*4);
            // Native sine-table indexing and remaining effect reviewed before baseline.
            s16 angle=actor->m920;
            u32 index=((u16)angle>>3)*8;
            f32 sine=eventField<be<f32>>(0x104A44F8+index);
            actor->mParticlePos.x=gabi::fmadds(sine,eventConstant(0x100079D4),(f32)actor->mParticlePos.x);
            actor->mParticlePos.y=(f32)actor->mParticlePos.y+limit;
            actor->m920=(s16)((u16)actor->m920+200);
            gabi::call(0x025E1988,0x5120);
        }
    }
    u32 play=gabi::call<u32>(0x025200D4);
    eventField<be<u8>>(play+0x5BBA)=0x25;
    play=gabi::call<u32>(0x025200D4);
    eventField<be<u8>>(play+0x5BB9)=0x27;
}
VERIFY(0x0205AF48,Auction_eventMainKai);
