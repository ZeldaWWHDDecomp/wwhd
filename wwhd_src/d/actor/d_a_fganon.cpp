// Phantom Ganon, reconstructed locally from WWHD.
#include "d/actor/d_a_fganon.h"
using namespace fganon;
void pos_move(fganon_class*,u8);
void fly_se_set(fganon_class*);

void anm_init(fganon_class* actor, s32 index, f32 blend, u8 loop, f32 speed, s32 soundIndex) {
    WWHD_FUNC(0x02134108, void, actor,index,blend,loop,speed,soundIndex);
    void* animation=dComIfG_getObjectRes(STR(0x1000EDD4),index,0x1000EBFC);
    void* sound=nullptr;
    if(soundIndex>=0) sound=dComIfG_getObjectRes(STR(0x1000EDD4),soundIndex,0x1000EBFC);
    gabi::call(0x025E4A98,read<u32>(actor,0x3DC),animation,loop,blend,speed,0.0f,-1.0f,sound);
}
VERIFY(0x02134108,anm_init);

static u32 jointMatrix(u32 model,u32 index) {
    u32 buffer=gabi::load<u32>(model+0x2C);
    u16 flags=gabi::load<u16>(buffer+4);
    u32 matrices=gabi::load<u32>(buffer+0x10);
    gabi::store<u16>(buffer+4,flags|0x10);
    return matrices+index*0x30;
}
static void copyJointMatrix(u32 model,u32 index) {
    u32 buffer=gabi::load<u32>(model+0x2C);
    u16 flags=gabi::load<u16>(buffer+4);
    u32 source=gabi::load<u32>(0x1018C7B0);
    gabi::store<u16>(buffer+4,flags|0x10);
    u32 matrices=gabi::load<u32>(buffer+0x10);
    f32 values[12];
    for(u32 i=0;i<12;++i)values[i]=gabi::load<f32>(source+i*4);
    for(u32 i=0;i<12;++i)gabi::store<f32>(matrices+index*0x30+i*4,values[i]);
}
BOOL nodeCallback(void* node,s32 timing) {
    WWHD_FUNC(0x02134230,BOOL,node,timing);
    if(timing==0) {
        void* joint=gabi::call<void*>(0x027F7878,node);
        u32 model=gabi::load<u32>(0x104B462C);
        u32 actor=gabi::load<u32>(model+0xB8);
        u32 index=gabi::load<u16>(gabi::ea(joint)+4);
        if(actor) {
            u32 matrix=jointMatrix(model,index);
            gabi::call(0x028E90D4,matrix,gabi::load<u32>(0x1018C7B0));
            gabi::call(0x025F1C28,gabi::load<u32>(0x1018C7B0),gabi::load<s16>(actor+0xDCC));
            gabi::call(0x025F1C5C,gabi::load<u32>(0x1018C7B0),gabi::load<s16>(actor+0xDCE));
            copyJointMatrix(model,index);
            gabi::call(0x028E90D4,gabi::load<u32>(0x1018C7B0),0x104B4868);
            if(index==(u32)(s32)gabi::load<s16>(0x1047B688)) {
                matrix=jointMatrix(model,index);
                gabi::call(0x028E90D4,matrix,gabi::load<u32>(0x1018C7B0));
                gabi::call(0x025F1C28,gabi::load<u32>(0x1018C7B0),gabi::load<s16>(0x1047B68A));
                gabi::call(0x025F1BF4,gabi::load<u32>(0x1018C7B0),gabi::load<s16>(0x1047B68C));
                gabi::call(0x025F1C5C,gabi::load<u32>(0x1018C7B0),gabi::load<s16>(0x1047B68E));
                copyJointMatrix(model,index);
                gabi::call(0x028E90D4,gabi::load<u32>(0x1018C7B0),0x104B4868);
            }
        }
    }
    return TRUE;
}
VERIFY(0x02134230,nodeCallback);

static void brkEntry(fganon_class* actor,u32 offset,u32 model,u32 target) {
    u32 animation=read<u32>(actor,offset);
    u32 data=gabi::load<u32>(model+0xAC);
    f32 frame=gabi::load<f32>(animation+4);
    gabi::call(target,animation,data,frame);
}
BOOL daFganon_Draw(fganon_class* actor) {
    WWHD_FUNC(0x02134428,BOOL,actor);
    gabi::call(0x025BED80,0xC2,actor,1.0f,1.0f,1.0f);
    u32 morf=read<u32>(actor,0x3DC);
    s8 visible=read<s8>(actor,0x8C1);
    u32 model=gabi::load<u32>(morf+0x90);
    u32 flags=gabi::load<u32>(model+0x74);
    flags=visible?flags|1:flags&~1u;
    gabi::store<u32>(model+0x74,flags);
    gabi::call(0x027F596C,model,flags);
    u32 sword=read<u32>(actor,0x3E4);
    flags=gabi::load<u32>(sword+0x74);
    flags=visible?flags|1:flags&~1u;
    gabi::store<u32>(sword+0x74,flags);
    gabi::call(0x027F596C,sword,flags);
    u32 env=gabi::call<u32>(0x02555D0C);
    gabi::call(0x02562F5C,env,model,member<void>(actor,0x110));
    brkEntry(actor,0x3E0,model,0x025E83FC);
    gabi::call(0x025E5590,read<u32>(actor,0x3DC));
    if(read<s8>(actor,0x3EC)!=2) {
        model=read<u32>(actor,0x3E4);
        env=gabi::call<u32>(0x02555D0C);
        gabi::call(0x025626A4,env,0,member<cXyz>(actor,0x314),member<void>(actor,0x3F0));
        env=gabi::call<u32>(0x02555D0C);
        gabi::call(0x02562F5C,env,model,member<void>(actor,0x3F0));
        brkEntry(actor,0x3E8,model,0x025E83FC);
        gabi::call(0x025E2DE0,model,0);
    }
    if(read<s8>(actor,0x8A5)) {
        model=read<u32>(actor,0x8A8);
        brkEntry(actor,0x8B0,model,0x025E83FC);
        brkEntry(actor,0x8AC,model,0x025E7FC4);
        gabi::call(0x025E2DE0,model,0);
    }
    return TRUE;
}
VERIFY(0x02134428,daFganon_Draw);

static void initBrk(fganon_class* actor,u32 model,u32 resource,s32 index,u32 animationOffset) {
    void* animation=dComIfG_getObjectRes(STR(resource),index,0x1000EBFC);
    u32 brk=read<u32>(actor,animationOffset);
    u32 data=gabi::load<u32>(model+0xAC);
    gabi::call(0x025E8154,brk,data,animation,1,0,1.0f,0,-1,1,0);
}

void deru_brk(fganon_class* actor) {
    WWHD_FUNC(0x021345C8,void,actor);
    initBrk(actor,morfModel(actor),0x1000EDE0,0x21,0x3E0);
    initBrk(actor,read<u32>(actor,0x3E4),0x1000EDE0,0x1F,0x3E8);
    sound(actor,0x594E);
    write<s8>(actor,0x8C1,1);
}
VERIFY(0x021345C8,deru_brk);

void kieru_brk(fganon_class* actor,u8 which) {
    WWHD_FUNC(0x021346F0,void,actor,which);
    if(which==0||which==1) {
        initBrk(actor,morfModel(actor),0x1000EDE8,0x24,0x3E0);
        sound(actor,0x594F);
    }
    if(which==0||which==2) initBrk(actor,read<u32>(actor,0x3E4),0x1000EDF0,7,0x3E8);
    write<s8>(actor,0x8C1,0);
}
VERIFY(0x021346F0,kieru_brk);

static void monsterSound(fganon_class* actor,u32 id) {
    u32 p=gabi::ea(actor);
    if(p+0x37C) {
        s8 room=read<s8>(actor,0x326);
        u32 process=p?read<u32>(actor,4):0xFFFFFFFF;
        s32 reverb=gabi::call<s32>(0x02520540,room);
        gabi::call(0x025E1AA4,id,member<cXyz>(actor,0x37C),process,0,reverb);
    }
}
void start(fganon_class* actor) {
    WWHD_FUNC(0x02139AA0,void,actor);
    u32 play=gabi::call<u32>(0x025200D4);
    u32 player=gabi::load<u32>(play+0x5B2C);
    s16 mode=read<s16>(actor,0x5BC);
    write<u32>(actor,0x39C,0);write<s16>(actor,0x5E2,3);
    switch(mode) {
    case 0: {
        gabi::Local<cXyz> offset;
        gabi::call(0x0201ADE0,gabi::at<cXyz>(player+0x314),offset.get(),member<cXyz>(actor,0x2EC));
        f32 x=offset->x,z=offset->z;
        f32 distance=gabi::call<f32>(0x028F4384,gabi::fmadds(x,x,z*z));
        f32 radius=(f32)read<u8>(actor,0x3D9)*10.0f;
        if(distance<radius) {
            write<s16>(actor,0x5BC,1);write<s16>(actor,0xD88,1);
            anm_init(actor,0x16,0,2,1,-1);
            gabi::call(0x025E1960,30);
        }
        break;
    }
    case 1: {
        play=gabi::call<u32>(0x025200D4);
        u32 target=gabi::load<u32>(play+0x5B2C);
        s16 angle=gabi::call<s16>(0x025D6894,actor,target);
        write<s16>(actor,0x32A,angle);break;
    }
    case 2:
        anm_init(actor,0x17,5,2,1,-1);monsterSound(actor,0x4939);
        write<s16>(actor,0x5BC,3);break;
    case 4:
        write<s16>(actor,0x5BC,5);
        write<s16>(actor,0x5D8,(s16)(gabi::load<s16>(0x1047B688)+0x50));
        anm_init(actor,0x10,10,2,1,-1);
        [[fallthrough]];
    case 5: {
        s16 trigger=(s16)(gabi::load<s16>(0x1047B688)+0x4B);
        if(read<s16>(actor,0x5D8)==trigger) {
            write<s8>(actor,0x63C,1);gabi::call(0x025E1918,0x80000041u);
            trigger=(s16)(gabi::load<s16>(0x1047B688)+0x4B);
        }
        if(read<s16>(actor,0x5D8)<=trigger)sound(actor,0x5152);
        gabi::Local<cXyz> offset;
        offset->x=0;offset->y=read<f32>(actor,0x370)*0.5f;offset->z=-read<f32>(actor,0x370);
        gabi::call(0x025F1884,gabi::load<u32>(0x1018C7B0),read<s16>(actor,0x32A));
        gabi::call(0x0200FCD8,offset.get(),member<cXyz>(actor,0x33C));
        gabi::call(0x028E8D88,member<cXyz>(actor,0x314),member<cXyz>(actor,0x33C),member<cXyz>(actor,0x314));
        f32 target=read<s16>(actor,0x5D8)==0?0:gabi::load<f32>(0x1047B62C)+20;
        f32 max=gabi::load<f32>(0x1047B624)+0.5f;
        gabi::call(0x0200ED84,member<f32>(actor,0x370),target,1.0f,max);break;
    }
    }
}
VERIFY(0x02139AA0,start);

void end(fganon_class* actor) {
    WWHD_FUNC(0x02139E80,void,actor);
    gabi::call(0x025200D4);
    write<u32>(actor,0x39C,0);
    u32 morf=read<u32>(actor,0x3DC);
    write<s16>(actor,0x5E2,3);
    s32 frame=gabi::ftoi(gabi::load<f32>(morf+0x9C));
    switch(read<s16>(actor,0x5BC)) {
    case 0:
        write<s16>(actor,0x5BC,1);anm_init(actor,0xB,2,0,1,-1);
        write<s16>(actor,0xD88,50);
        gabi::call(0x025B8B68,gabi::load<u32>(0x101F84DC)+0x644,0x3F20);break;
    case 1:if(frame==104)kieru_brk(actor,0);break;
    case 2:
        deru_brk(actor);anm_init(actor,0x17,1,2,1,-1);
        monsterSound(actor,0x4939);write<s16>(actor,0x5BC,3);break;
    case 4:
        deru_brk(actor);anm_init(actor,0xF,1,0,1,-1);
        write<s16>(actor,0x5BC,5);write<f32>(actor,0x340,0);
        sound(actor,0x5956);
        [[fallthrough]];
    case 5: {
        write<f32>(actor,0x318,read<f32>(actor,0x318)+read<f32>(actor,0x340));
        f32 acceleration=gabi::load<f32>(0x1047BAB0)+0.2f;
        f32 speed=read<f32>(actor,0x340)+acceleration;
        write<f32>(actor,0x340,speed);
        f32 limit=gabi::load<f32>(0x1047BAB4)+20;
        if(speed>limit)write<f32>(actor,0x340,limit);
        if(frame==28) {
            initBrk(actor,morfModel(actor),0x1000EBF4,0x25,0x3E0);
            initBrk(actor,read<u32>(actor,0x3E4),0x1000EBF4,0x23,0x3E8);
            sound(actor,0x594F);write<s8>(actor,0x8C1,0);
        } else if(frame==48)write<s16>(actor,0x5BC,6);
        break;
    }
    }
}
VERIFY(0x02139E80,end);

static bool animationStopped(fganon_class* actor) {
    u32 morf=read<u32>(actor,0x3DC);
    return (gabi::load<u8>(morf+0xA7)&1)||gabi::load<f32>(morf+0x98)==0;
}
void down(fganon_class* actor) {
    WWHD_FUNC(0x02139680,void,actor);
    gabi::call(0x025200D4);
    s16 mode=read<s16>(actor,0x5BC);
    switch(mode) {
    case 0:
        anm_init(actor,6,2,0,1,-1);
        mode=(s16)(read<s16>(actor,0x5BC)+1);
        write<f32>(actor,0x340,0);write<s16>(actor,0x5BC,mode);
        [[fallthrough]];
    case 1:
        {
        u32 ground=read<u32>(actor,0x954);
        write<s16>(actor,0x5E2,5);
        if((ground&0x20)&&animationStopped(actor)) {
            write<s16>(actor,0x5BC,(s16)(read<s16>(actor,0x5BC)+1));
            anm_init(actor,0x15,2,0,1,-1);
        }
        write<s16>(actor,0x5D8,gabi::load<s16>(0x10463D94));
        break;
        }
    case 2:
        if(animationStopped(actor)) {
            write<s16>(actor,0x5BC,(s16)(mode+1));
            anm_init(actor,8,2,2,1,-1);
        }
        break;
    case 3:
        if(gabi::ftoi(gabi::load<f32>(read<u32>(actor,0x3DC)+0x9C))==2)monsterSound(actor,0x4940);
        break;
    case 10:
        anm_init(actor,7,2,0,1,-1);write<s16>(actor,0x5BC,2);break;
    }
    f32 speed=read<f32>(actor,0x340);
    f32 y=read<f32>(actor,0x318);
    f32 nextSpeed=speed-0.5f;
    write<f32>(actor,0x318,y+speed);
    if(nextSpeed<-15)nextSpeed=-15;
    write<f32>(actor,0x340,nextSpeed);
    gabi::Local<dBgS_GndChk> ground;
    const dBgS_GndChk_vt vt={0x1000ECC4,0x1000ECD4,0x1000ECF4,0x1000ECE4};
    dBgS_GndChk_ct(ground.get(),vt,true);
    u32 g=gabi::ea(ground.get());
    gabi::store<u32>(g+0x50,0xE);
    y=read<f32>(actor,0x318);
    f32 x=read<f32>(actor,0x314);
    f32 z=read<f32>(actor,0x31C);
    gabi::store<f32>(g+0x24,x);gabi::store<f32>(g+0x28,y+300);gabi::store<f32>(g+0x2C,z);
    u32 play=gabi::call<u32>(0x025200D4);
    f32 height=gabi::call<f32>(0x02008974,play+0x12A0,ground.get());
    bool below=height!=-1000000000.0f && !(read<f32>(actor,0x318)>height);
    if(below||read<s16>(actor,0x5D8)==0) {
        u8 variant=read<u8>(actor,0x3D8);
        write<s16>(actor,0x5BA,2);write<s16>(actor,0x5BC,0);
        if(variant)write<s8>(actor,0x3A1,100);
    }
    gabi::store<u32>(g+0x20,0x1000EC54);gabi::store<u32>(g+0x40,0x1000EC74);gabi::store<u32>(g+0x4C,0x1000EC34);
    gabi::call(0x02008DAC,ground.get(),0);
}
VERIFY(0x02139680,down);

void spinattack2(fganon_class* actor) {
    WWHD_FUNC(0x02139194,void,actor);
    u32 play=gabi::call<u32>(0x025200D4);
    u32 player=gabi::load<u32>(play+0x5B2C);
    gabi::Local<dBgS_LinChk> line;
    const dBgS_LinChk_vt vt={0x1000ED34,0x1000ED44,0x1000ED64,0x1000ED54};
    dBgS_LinChk_ct(line.get(),vt,false);
    u32 morf=read<u32>(actor,0x3DC);
    s16 mode=read<s16>(actor,0x5BC);
    s32 frame=gabi::ftoi(gabi::load<f32>(morf+0x9C));
    switch(mode) {
    case 0: {
        u8 copy=read<u8>(actor,0x8C3);
        s16 playerYaw=gabi::load<s16>(player+0x32A);
        write<s16>(actor,0x32A,(s16)(playerYaw+copy*0x3333));
        write<s16>(actor,0x5BC,1);
        if(copy==0)for(u32 i=1;i<5;++i)
            gabi::call(0x025D5834,0xF1,(i*16)|3,member<cXyz>(actor,0x314),read<s8>(actor,0x326),0,0,-1,0);
        [[fallthrough]];
    }
    case 1: {
        gabi::call(0x025F1884,gabi::load<u32>(0x1018C7B0),read<s16>(actor,0x32A));
        gabi::Local<cXyz> offset,transformed,result;
        offset->x=0;offset->y=gabi::load<f32>(0x10463D78);offset->z=gabi::load<f32>(0x10463D80);
        gabi::call(0x0200FCD8,offset.get(),transformed.get());
        gabi::call(0x0201AD78,gabi::at<cXyz>(player+0x314),result.get(),transformed.get());
        member<cXyz>(actor,0x314)->copy(*result.get());
        anm_init(actor,0xA,1,2,1,-1);write<s16>(actor,0x5BC,2);deru_brk(actor);
        member<cXyz>(actor,0x5C0)->copy(*gabi::at<cXyz>(player+0x314));
        [[fallthrough]];
    }
    case 2: {
        write<s16>(actor,0x322,read<s16>(actor,0x32A));
        f32 speed=gabi::load<f32>(0x1047BAA0)+5;
        write<s16>(actor,0x320,0);write<f32>(actor,0x370,speed);
        pos_move(actor,1);
        gabi::Local<cXyz> result,offset;
        gabi::call(0x0201ADE0,member<cXyz>(actor,0x5C0),result.get(),member<cXyz>(actor,0x314));
        offset->copy(*result.get());
        f32 magnitude=gabi::call<f32>(0x028E8DD0,offset.get());
        f32 distance=gabi::call<f32>(0x028F4384,magnitude);
        if(distance<gabi::load<f32>(0x10463D84)) {
            anm_init(actor,9,2,0,1,-1);
            u8 copy=read<u8>(actor,0x8C3);
            write<s16>(actor,0x5BC,3);write<f32>(actor,0x370,0);
            if(copy==0)monsterSound(actor,0x493C);
        }
        break;
    }
    case 3: {
        if(frame==14 && read<u8>(actor,0x8C3)==0)sound(actor,0x5951);
        write<u8>(actor,0x8B9,1);write<u8>(actor,0xC70,8);
        s16 trigger=gabi::load<s16>(0x10463D8A);
        morf=read<u32>(actor,0x3DC);
        if((u32)frame==(u32)(s32)trigger)
            write<s8>(actor,0x8E0,(s8)(gabi::load<s16>(0x10463D8C)-trigger));
        if((gabi::load<u8>(morf+0xA7)&1)||gabi::load<f32>(morf+0x98)==0) {
            if(read<u8>(actor,0x8C3)==0) {
                write<s16>(actor,0x5BC,4);write<s16>(actor,0x5D8,40);
                anm_init(actor,0x16,10,2,1,-1);
            } else {write<s16>(actor,0x5BA,2);write<s16>(actor,0x5BC,0);}
        }
        break;
    }
    case 4:
        if(read<s16>(actor,0x5D8)==0) {write<s16>(actor,0x5BA,2);write<s16>(actor,0x5BC,0);}break;
    }
    u32 l=gabi::ea(line.get());
    gabi::store<u32>(l+0x58,0x1000ED64);gabi::store<u32>(l+0x64,0x1000EC34);gabi::store<u32>(l+0x20,0x1000EC24);
    gabi::call(0x02008B4C,line.get(),0);
}
VERIFY(0x02139194,spinattack2);

void last_end(fganon_class* actor) {
    WWHD_FUNC(0x0213A228,void,actor);
    gabi::call(0x025200D4);
    s16 mode=read<s16>(actor,0x5BC);
    write<s16>(actor,0x5E2,3);write<u32>(actor,0x39C,0);
    switch(mode) {
    case 0:
        write<s16>(actor,0x5BC,1);anm_init(actor,0xC,2,0,1,-1);
        write<s16>(actor,0xD88,100);write<f32>(actor,0x5F4,10000);
        write<s16>(actor,0x5DA,150);
        write<s16>(actor,0x5D8,(s16)(gabi::load<s16>(0x1047B690)+25));
        break;
    case 1:
        if(read<s16>(actor,0x5DA)==0) {
            s16 counter=read<s16>(actor,0xD88);
            write<s8>(actor,0x3EC,1);write<s16>(actor,0x5BC,2);
            write<s16>(actor,0xD88,(s16)(counter+1));write<s16>(actor,0x5DA,20);
        }
        break;
    case 2:
        gabi::call(0x0200ED84,member<f32>(actor,0x330),0.1f,1.0f,0.05f);
        gabi::call(0x0200ED84,member<f32>(actor,0x338),0.1f,1.0f,0.05f);
        gabi::call(0x0200ED84,member<f32>(actor,0x334),0.1f,1.0f,0.05f);
        if(read<s16>(actor,0x5DA)==0) {
            write<s16>(actor,0x5BC,3);write<s16>(actor,0x5DA,30);
            sound(actor,0x5905);
        }
        break;
    case 3:
        gabi::call(0x0200EDC8,member<f32>(actor,0x330),0.1f,0.05f);
        gabi::call(0x0200EDC8,member<f32>(actor,0x338),0.1f,0.05f);
        gabi::call(0x0200ED84,member<f32>(actor,0x334),10.0f,0.1f,1.0f);
        if(read<s16>(actor,0x5DA)==0) {
            write<s16>(actor,0x5BC,4);write<f32>(actor,0x330,0);
            write<s16>(actor,0x5DA,30);write<f32>(actor,0x334,0);write<f32>(actor,0x338,0);
        }
        break;
    case 4: {
        s16 timer=read<s16>(actor,0x5DA);
        if(timer<=4) {
            u32 id;
            if(timer==4) {
                id=gabi::call<u32>(0x025D5834,0x1CF,5,member<cXyz>(actor,0x314),read<s8>(actor,0x326),0,0,-1,0);
                write<u32>(actor,0x8E4,id);
            } else id=read<u32>(actor,0x8E4);
            gabi::Local<be<u32>> local;*local.get()=id;
            u32 boko=0;
            if(id!=0xFFFFFFFF)boko=gabi::call<u32>(0x025D5218,0x025E1234,local.get());
            if(boko) {
                write<s8>(actor,0x3EC,2);
                if(!(gabi::load<u32>(boko+0x2E0)&0x2000))gabi::call(0x025D9D0C,boko,0);
                u32 source=read<u32>(actor,0x3E4);
                u32 destination=gabi::load<u32>(boko+0x3B4);
                if(source)source+=0xC8;
                if(destination) {
                    f32 values[12];for(u32 i=0;i<12;++i)values[i]=gabi::load<f32>(source+i*4);
                    for(u32 i=0;i<12;++i)gabi::store<f32>(destination+0xC8+i*4,values[i]);
                }
                if(read<s16>(actor,0x5DA)==1) {
                    gabi::call(0x025D9D24,boko);
                    s16 counter=read<s16>(actor,0xD88);
                    write<s16>(actor,0xD8A,0);write<s16>(actor,0x5BC,5);write<s16>(actor,0xD88,(s16)(counter+1));
                }
            }
        }
        break;
    }
    }
    f32 fogEnd=read<f32>(actor,0x5F4);
    s16 timer=read<s16>(actor,0x5D8);
    write<u16>(actor,0x1B4,0xFF);write<u16>(actor,0x1B2,0xFF);write<u16>(actor,0x1B0,0xFF);
    f32 fogStart=gabi::load<f32>(0x1047BAB8);
    write<f32>(actor,0x1BC,fogEnd);write<f32>(actor,0x1B8,fogStart);
    if(timer<=1) {
        if(timer==1) {write<s8>(actor,0x8C2,1);write<f32>(actor,0x5F4,5100);}
        f32 max=gabi::load<f32>(0x1047B620)+63.5f;
        gabi::call(0x0200ED84,member<f32>(actor,0x5F4),100.0f,1.0f,max);
    }
    gabi::call(0x0200F428,member<s16>(actor,0x320),0,4,0x800);
}
VERIFY(0x0213A228,last_end);

static void invalidateEmitter(fganon_class* actor,u32 slot) {
    u32 emitter=read<u32>(actor,slot);
    if(emitter) {
        u32 flags=gabi::load<u32>(emitter+0x254);
        gabi::store<s32>(emitter+0x5C,-1);gabi::store<u32>(emitter+0x254,flags|1);
        write<u32>(actor,slot,0);
    }
}
static u32 createParticle(u32 id,cXyz* position,csXyz* angle=nullptr) {
    u32 play=gabi::call<u32>(0x025200D4);
    u32 particles=gabi::load<u32>(play+0x5AB0);
    return gabi::call<u32>(0x025A847C,particles,0,id,position,angle,0,0xFF,0,-1,0,0,0);
}
static void emitterJointMatrix(fganon_class* actor,u32 emitter,u32 index) {
    u32 model=morfModel(actor);
    u32 buffer=gabi::load<u32>(model+0x2C);
    u16 flags=gabi::load<u16>(buffer+4);
    u32 matrices=gabi::load<u32>(buffer+0x10);
    gabi::store<u16>(buffer+4,flags|0x10);
    gabi::call(0x028249B0,matrices+index*0x30,emitter+0x1F0,emitter+0x22C);
}
static void energyBallAppearance(fganon_class* actor) {
    if(read<s8>(actor,0x8A5)==0)return;
    gabi::call(0x025F1884,gabi::load<u32>(0x1018C7B0),read<s16>(actor,0x32A));
    gabi::Local<cXyz> offset,transformed,position;
    f32 distance=read<f32>(actor,0x8B4);
    offset->x=0;offset->z=distance;offset->y=-distance;
    gabi::call(0x0200FCD8,offset.get(),transformed.get());
    f32 x=read<f32>(actor,0x314)+transformed->x;
    f32 y=read<f32>(actor,0x318)+400;
    y=y+gabi::load<f32>(0x1047BAA0);
    y=y+transformed->y;
    f32 z=read<f32>(actor,0x31C)+transformed->z;
    s8 state=read<s8>(actor,0x8A5);
    position->x=x;position->y=y;position->z=z;
    write<f32>(actor,0x898,x);write<f32>(actor,0x89C,y);write<f32>(actor,0x8A0,z);
    switch(state) {
    case 1: {
        invalidateEmitter(actor,0x608);invalidateEmitter(actor,0x60C);
        u32 emitter=createParticle(0x821A,position.get());write<u32>(actor,0x608,emitter);
        emitter=createParticle(0x821B,position.get());write<u32>(actor,0x60C,emitter);
        u32 brk=read<u32>(actor,0x8B0);
        write<u8>(actor,0x8A6,250);write<s8>(actor,0x8A5,2);
        gabi::store<f32>(brk,1);break;
    }
    case 3:
        gabi::store<f32>(read<u32>(actor,0x8B0),-1);
        write<s8>(actor,0x8A5,4);
        createParticle(0x8242,position.get(),member<csXyz>(actor,0x328));
        createParticle(0x8243,position.get(),member<csXyz>(actor,0x328));break;
    case 4: {
        u8 alpha=(u8)(read<u8>(actor,0x8A6)-50);write<u8>(actor,0x8A6,alpha);
        for(u32 slot=0x608;slot<=0x60C;slot+=4) {
            u32 emitter=read<u32>(actor,slot);
            if(emitter) {gabi::store<u8>(emitter+0x247,alpha);alpha=read<u8>(actor,0x8A6);}
        }
        if(alpha==0)write<s8>(actor,0x8A5,5);break;
    }
    case 5:case 6:case 7:case 8:write<s8>(actor,0x8A5,(s8)(state+1));break;
    case 9:write<s8>(actor,0x8A5,0);break;
    }
    for(u32 slot=0x608;slot<=0x60C;slot+=4) {
        u32 emitter=read<u32>(actor,slot);
        if(emitter) {
            if(read<u8>(actor,0x8A6)) {
                f32 emitterY=position->y;
                if(gabi::load<u8>(emitter+0x262)>=7)emitterY=-emitterY;
                gabi::store<f32>(emitter+0x22C,position->x);gabi::store<f32>(emitter+0x230,emitterY);gabi::store<f32>(emitter+0x234,position->z);
            } else invalidateEmitter(actor,slot);
        }
    }
    gabi::call(0x028E93CC,0x1048D0CC,(f32)position->x,(f32)position->y,(f32)position->z);
    f32 matrix[12];for(u32 i=0;i<12;++i)matrix[i]=gabi::load<f32>(0x1048D0CC+i*4);
    u32 model=read<u32>(actor,0x8A8);
    for(u32 i=0;i<12;++i)gabi::store<f32>(model+0xC8+i*4,matrix[i]);
    gabi::call(0x025E742C,read<u32>(actor,0x8B0));
    gabi::call(0x025E742C,read<u32>(actor,0x8AC));
}
void shot2(fganon_class* actor) {
    WWHD_FUNC(0x02138670,void,actor);
    gabi::call(0x025200D4);
    u32 play=gabi::call<u32>(0x025200D4);
    s16 angle=gabi::call<s16>(0x025D6894,actor,gabi::load<u32>(play+0x5B2C));
    gabi::call(0x0200F428,member<s16>(actor,0x32A),angle,10,0x400);
    s16 mode=read<s16>(actor,0x5BC);
    s32 frame=gabi::ftoi(gabi::load<f32>(read<u32>(actor,0x3DC)+0x9C));
    switch(mode) {
    case 0: {
        anm_init(actor,0x11,5,0,1,-1);
        invalidateEmitter(actor,0x600);invalidateEmitter(actor,0x604);
        u32 emitter=createParticle(0x8218,member<cXyz>(actor,0x314));write<u32>(actor,0x600,emitter);
        emitter=createParticle(0x8219,member<cXyz>(actor,0x314));write<u32>(actor,0x604,emitter);
        monsterSound(actor,0x493B);
        write<s16>(actor,0x5BC,(s16)(read<s16>(actor,0x5BC)+1));
        [[fallthrough]];
    }
    case 1:
        sound(actor,0x5153);
        for(u32 slot=0x600;slot<=0x604;slot+=4) {
            u32 emitter=read<u32>(actor,slot);if(emitter)emitterJointMatrix(actor,emitter,26);
        }
        if(animationStopped(actor)) {
            anm_init(actor,0x12,1,2,1,-1);
            s16 modeNow=read<s16>(actor,0x5BC);
            write<s16>(actor,0x5D8,gabi::load<s16>(0x10463D88));write<s16>(actor,0x5BC,(s16)(modeNow+1));
            invalidateEmitter(actor,0x600);invalidateEmitter(actor,0x604);
        }
        if(frame==28) {write<f32>(actor,0x8B4,0);write<s8>(actor,0x8A5,1);}
        break;
    case 2:
        sound(actor,0x5153);
        if(read<s16>(actor,0x5D8)!=0)break;
        anm_init(actor,0xE,3,0,1,-1);
        write<s16>(actor,0x5BC,(s16)(read<s16>(actor,0x5BC)+1));
        {
        u32 emitter=createParticle(0x821C,member<cXyz>(actor,0x314));write<u32>(actor,0x600,emitter);
        emitter=createParticle(0x821D,member<cXyz>(actor,0x314));write<u32>(actor,0x604,emitter);
        }
        [[fallthrough]];
    case 3: {
        s32 trigger=gabi::load<s16>(0x1047BB12)+15;
        if(frame==trigger) {monsterSound(actor,0x493D);sound(actor,0x5955);trigger=gabi::load<s16>(0x1047BB12)+15;}
        if(frame>=trigger) {
            f32 target=gabi::load<f32>(0x1047BAA8)+250;
            f32 max=gabi::load<f32>(0x1047BAAC)+50;
            gabi::call(0x0200ED84,member<f32>(actor,0x8B4),target,1.0f,max);
            if((u32)frame==(u32)(s32)(s16)(gabi::load<s16>(0x1047BB14)+16))write<s8>(actor,0x8A5,3);
            if((u32)frame==(u32)(s32)(s16)(gabi::load<s16>(0x1047BB16)+16)) {
                for(u32 i=0;i<8;++i)gabi::call(0x025D5834,0xF2,i,member<cXyz>(actor,0x898),read<s8>(actor,0x326),0,0,-1,0);
                write<s8>(actor,0x8A4,0);
            }
        }
        for(u32 i=0;i<2;++i) {u32 emitter=read<u32>(actor,0x600+i*4);if(emitter)emitterJointMatrix(actor,emitter,14+i*9);}
        if(animationStopped(actor)) {
            anm_init(actor,0x16,10,2,1,-1);
            write<s16>(actor,0x5D8,60);write<s16>(actor,0x5BC,4);
            invalidateEmitter(actor,0x600);invalidateEmitter(actor,0x604);
        }
        break;
    }
    case 4:
        if(read<s8>(actor,0x8BE)==1)anm_init(actor,0x16,10,2,1,-1);
        if(read<s16>(actor,0x5D8)==0) {write<s16>(actor,0x5BA,9);write<s16>(actor,0x5BC,0);}break;
    }
    f32 max=gabi::load<f32>(0x1047B648)+1;
    gabi::call(0x0200EDC8,member<f32>(actor,0x370),1.0f,max);
    pos_move(actor,0);fly_se_set(actor);energyBallAppearance(actor);
}
VERIFY(0x02138670,shot2);

static void hitFlash(fganon_class* actor,cXyz* position,f32 size,bool critical,bool swordFlash) {
    if(critical)createParticle(0x10,member<cXyz>(actor,0x37C));
    gabi::Local<cXyz> scale;
    scale->x=size;scale->y=size;scale->z=size;
    gabi::Local<csXyz> angle;angle->z=0;angle->x=0;
    u32 play=gabi::call<u32>(0x025200D4);
    angle->y=gabi::call<s16>(0x025D6894,actor,gabi::load<u32>(play+0x5B2C));
    play=gabi::call<u32>(0x025200D4);
    gabi::call(0x025A847C,gabi::load<u32>(play+0x5AB0),0,0xD,position,angle.get(),scale.get(),0xFF,0,-1,0,0,0);
    if(swordFlash) {
        gabi::Local<cXyz> point;
        point->x=read<f32>(actor,0x37C);point->y=read<f32>(actor,0x380);point->z=read<f32>(actor,0x384);
        gabi::call(0x0255F554,point.get(),1);
    }
}
static void knockDown(fganon_class* actor,u32 player) {
    write<s16>(actor,0x5BC,0);write<s16>(actor,0x5BA,8);
    write<f32>(actor,0x5E4,gabi::load<f32>(0x1047BABC)+80);
    write<s16>(actor,0x5EC,(s16)(gabi::load<s16>(0x1047BB0C)+7));
    gabi::Local<cXyz> delta;
    gabi::call(0x0201ADE0,member<cXyz>(actor,0x314),delta.get(),gabi::at<cXyz>(player+0x314));
    s16 yaw=gabi::call<s16>(0x020195B0,(f32)delta->x,(f32)delta->z);
    f32 x=delta->x,z=delta->z;
    write<s16>(actor,0x5E8,yaw);
    f32 distance=gabi::call<f32>(0x028F4384,gabi::fmadds(x,x,z*z));
    s16 pitch=gabi::call<s16>(0x020195B0,(f32)delta->y,distance);
    write<f32>(actor,0x370,0);write<s16>(actor,0x5EA,(s16)-pitch);
}
void damage_check(fganon_class* actor) {
    WWHD_FUNC(0x0213A6FC,void,actor);
    u32 play=gabi::call<u32>(0x025200D4);
    u32 player=gabi::load<u32>(play+0x5B2C);
    gabi::call(0x02515E50,member<void>(actor,0xB0C));
    if(read<s16>(actor,0x5E2)!=0)return;
    struct AttackInfo {u8 bytes[0x1C];}; /* CcAtInfo: cc_at_check 025192A8 writes +0x18 (stack guard sweep 2026-10-05) */
    gabi::Local<AttackInfo> info;
    u32 a=gabi::ea(info.get());gabi::store<u32>(a+0x14,0);
    if(gabi::call<BOOL>(0x025162A4,member<void>(actor,0xB2C))) {
        u32 hit=gabi::call<u32>(0x02516300,member<void>(actor,0xB2C));gabi::store<u32>(a,hit);
        if(hit) {
            gabi::store<u32>(a+0x14,gabi::ea(actor)+0xBF8);
            u32 status=gabi::load<u32>(hit+0x44);
            u8 variant=read<u8>(actor,0x3D8);
            u32 source=status?gabi::load<u32>(status+0xC):0;
            gabi::store<u32>(a+4,source);
            if(variant==2) {
                if(gabi::load<u32>(hit+0x10)&0x100000) {
                    monsterSound(actor,0x4945);sound(actor,0x5957);
                    hitFlash(actor,member<cXyz>(actor,0x37C),2,true,true);
                    write<s16>(actor,0x5BC,0);write<s16>(actor,0x5E2,1000);write<s16>(actor,0x5BA,22);
                    gabi::call(0x025E1928);
                    if(read<s8>(actor,0x63C)!=0)write<s8>(actor,0x63C,35);
                    sound(actor,0x5904);
                    u32 emitter=createParticle(0x8405,member<cXyz>(actor,0x314));if(emitter)emitterJointMatrix(actor,emitter,10);
                    emitter=createParticle(0x8406,member<cXyz>(actor,0x314));if(emitter)emitterJointMatrix(actor,emitter,10);
                    write<f32>(actor,0x370,0);
                } else {
                    s8 soundState=read<s8>(actor,0x63C);
                    write<s16>(actor,0x5BC,0);write<s16>(actor,0x5BA,2);
                    if(soundState==1||soundState==2)write<s8>(actor,0x63C,35);
                }
                write<s8>(actor,0x8A5,0);invalidateEmitter(actor,0x608);invalidateEmitter(actor,0x60C);return;
            }
            if(source&&gabi::load<s16>(source+8)==0xF2) {
                write<s8>(actor,0x8A4,(s8)(read<u8>(actor,0x8A4)+1));anm_init(actor,6,2,0,1,-1);
                write<s16>(actor,0x5D8,60);monsterSound(actor,0x493F);
                if(read<s8>(actor,0x8A4)<5)return;
                hitFlash(actor,member<cXyz>(actor,0x37C),2,true,true);knockDown(actor,player);return;
            }
        }
    }
    if(read<u8>(actor,0x8C3)) {
        if(gabi::call<BOOL>(0x025162A4,member<void>(actor,0xB2C))||gabi::load<s16>(gabi::load<u32>(0x10463D30)+0x5BA)==8) {
            u32 emitter=createParticle(0x826B,member<cXyz>(actor,0x314));if(emitter)emitterJointMatrix(actor,emitter,0);
            gabi::call(0x025D57E0,actor);
        }
        return;
    }
    if(!gabi::call<BOOL>(0x025162A4,member<void>(actor,0xB2C)) && read<s8>(actor,0x8C0)==0)return;
    write<s16>(actor,0x5E2,6);
    if(gabi::call<BOOL>(0x025162A4,member<void>(actor,0xB2C))) {
        s16 action=read<s16>(actor,0x5BA);
        if(action!=8&&action!=7&&action!=10)return;
        u32 hit=gabi::call<u32>(0x02516300,member<void>(actor,0xB2C));
        gabi::store<u32>(a,hit);gabi::store<u32>(a+0x14,gabi::ea(actor)+0xBF8);
        u32 source=gabi::call<u32>(0x02518DB0,info.get());gabi::store<u32>(a+4,source);
        hit=gabi::load<u32>(a);
        if(!hit||!(gabi::load<u32>(hit+0x10)&2))return;
        u8 sword=gabi::load<u8>(gabi::load<u32>(0x101F84DC)+0x2E);
        if(sword!=0x39&&sword!=0x3E&&sword!=0x3A)return;
        source=gabi::call<u32>(0x025192A8,actor,info.get());gabi::store<u32>(a+4,source);
        if(gabi::load<u8>(a+0xA)==1&&gabi::load<u8>(player+0x69E8)!=0)write<s16>(actor,0x5E2,2);
        bool dead=gabi::load<u8>(a+9)!=0;
        if(dead)createParticle(0x10,member<cXyz>(actor,0x37C));
        gabi::Local<cXyz> scale;scale->x=dead?2:1;scale->y=dead?2:1;scale->z=dead?2:1;
        monsterSound(actor,read<s8>(actor,0x3A1)<=0?0x4943:0x4942);
        gabi::Local<csXyz> angle;angle->z=0;angle->x=0;
        play=gabi::call<u32>(0x025200D4);
        angle->y=gabi::call<s16>(0x025D6894,actor,gabi::load<u32>(play+0x5B2C));
        play=gabi::call<u32>(0x025200D4);
        gabi::call(0x025A847C,gabi::load<u32>(play+0x5AB0),0,0xD,member<cXyz>(actor,0xBF8),angle.get(),scale.get(),0xFF,0,-1,0,0,0);
        action=read<s16>(actor,0x5BA);
        if(action==7||action==10) {write<s16>(actor,0x5BC,0);write<s16>(actor,0x5BA,8);return;}
        u8 variant=read<u8>(actor,0x3D8);
        if(variant==0) {
            if(read<s8>(actor,0x3A1)<=0) {
                write<s16>(actor,0x5BC,0);write<s16>(actor,0x5E2,1000);write<s16>(actor,0x5BA,21);gabi::call(0x025E1928);
            } else {
                s8 remaining=(s8)(read<u8>(actor,0x8C4)-gabi::load<u8>(a+8));
                if(remaining<=0) {write<s16>(actor,0x5BC,0);write<s16>(actor,0x5BA,2);write<s8>(actor,0x8C4,10);}
                else {write<s8>(actor,0x8C4,remaining);write<s16>(actor,0x5BC,10);}
            }
        } else if(variant==1) {
            write<s8>(actor,0x3A1,0);write<s16>(actor,0x5BC,0);write<s16>(actor,0x5BA,3);gabi::call(0x025E1928);
        }
    } else {
        monsterSound(actor,0x493F);hitFlash(actor,member<cXyz>(actor,0x37C),2,true,true);knockDown(actor,player);
    }
}
VERIFY(0x0213A6FC,damage_check);

// These movement actions are inlined into move in the HD executable.
static f32 hioFloat(u32 gcOffset) { return gabi::load<f32>(0x10463D50+gcOffset-4); }
static s16 hioShort(u32 gcOffset) { return gabi::load<s16>(0x10463D50+gcOffset-4); }
static void destroyMovementLine(dBgS_LinChk* line) {
    u32 p=gabi::ea(line);
    gabi::store<u32>(p+0x58,0x1000ED64);
    gabi::store<u32>(p+0x64,0x1000EC34);
    gabi::store<u32>(p+0x20,0x1000EC24);
    gabi::call(0x02008B4C,line,0);
}
static bool wallCheck(fganon_class* actor) {
    u32 player=gabi::load<u32>(gabi::call<u32>(0x025200D4)+0x5B2C);
    gabi::Local<dBgS_LinChk> line;
    const dBgS_LinChk_vt vt={0x1000ED34,0x1000ED44,0x1000ED64,0x1000ED54};
    dBgS_LinChk_ct(line.get(),vt,false);
    gabi::Local<cXyz> offset,transformed,temporary,end;
    offset->x=0; offset->y=0; offset->z=hioFloat(0x34)+100.0f;
    bool hit=false;
    for(int i=0;i<8;++i) {
        gabi::call(0x025F1884,gabi::load<u32>(0x1018C7B0),(s16)(i*0x2000));
        gabi::call(0x0200FCD8,offset.get(),transformed.get());
        gabi::call(0x0201AD78,player+0x37C,temporary.get(),transformed.get());
        *end.get()=*temporary.get();
        gabi::call(0x024F1AFC,line.get(),player+0x37C,end.get(),actor);
        u32 play=gabi::call<u32>(0x025200D4);
        if(gabi::call<s32>(0x02008860,play+0x12A0,line.get())) { hit=true;break; }
    }
    destroyMovementLine(line.get());
    return hit;
}
static void appearAction(fganon_class* actor) {
    // This accessor call is retained even when the selected mode does no work.
    gabi::call(0x025200D4);
    actor->m3AE=3;
    if(actor->mMode!=0)return;
    u8 choice=gabi::load<u8>(0x10463D52);
    if(choice==0) {
        if(actor->m2BC==0)choice=gabi::call<f32>(0x020198D8,1.0f)<0.5f?1:2;
        else if(gabi::call<f32>(0x020198D8,1.0f)<0.4f&&!wallCheck(actor))choice=4;
        else choice=gabi::call<f32>(0x020198D8,1.0f)<0.5f?3:1;
    }
    switch(choice) {
    case 1: actor->mAction=5;actor->mMode=-10;actor->m3A4[2]=hioShort(0xA);break;
    case 2: actor->mAction=7;actor->mMode=0;break;
    case 3: actor->mAction=9;actor->mMode=-10;actor->m3A4[2]=hioShort(0xA);break;
    case 4: actor->mAction=10;actor->mMode=0;break;
    }
}
static void disappearAction(fganon_class* actor) {
    gabi::call(0x025200D4);
    actor->m3AE=3;
    switch((s16)actor->mMode) {
    case 0:
        if(actor->m408==1||actor->m408==2)actor->m408=35;
        kieru_brk(actor,0);
        if(actor->m68F) {actor->mMode=1;actor->m3A4[0]=30;}
        else {actor->mAction=0;actor->mMode=0;}
        break;
    case 1: if(actor->m3A4[0]==0)fopAcM_delete(actor);break;
    }
}
static void standbyAction(fganon_class* actor) {
    u32 player=gabi::load<u32>(gabi::call<u32>(0x025200D4)+0x5B2C);
    actor->m3AE=3;
    switch((s16)actor->mMode) {
    case -1: {
        if((actor->m2BC==2&&!gabi::call<s32>(0x02520C0C,0x36))||
           (actor->m2BC==1&&gabi::call<s32>(0x02520C0C,0x36))) {
            write<f32>(actor,0x318,-20000.0f);break;
        }
        f32 z=gabi::load<f32>(player+0x31C)-read<f32>(actor,0x2F4);
        f32 x=gabi::load<f32>(player+0x314)-read<f32>(actor,0x2EC);
        f32 distance=gabi::call<f32>(0x028F4384,gabi::fmadds(x,x,z*z));
        if(distance<(f32)(u8)actor->m2BD*10.0f) {
            actor->m3A4[0]=hioShort(8);actor->mB89=22;actor->mMode=1;
            write<u32>(actor,0x2E0,read<u32>(actor,0x2E0)|0x20);
        }
        break;
    }
    case 0: actor->m3A4[0]=hioShort(8);actor->mMode=(s16)actor->mMode+1;[[fallthrough]];
    case 1:
        if(actor->m3A4[0]==0) {
            actor->m69C=0;actor->m698=0;actor->mAction=1;actor->m694=0;actor->mMode=0;
        }
        break;
    }
}
static void defeatedAction(fganon_class* actor) {
    gabi::call(0x025200D4);
    actor->m3AE=3;
    switch((s16)actor->mMode) {
    case 0:
        kieru_brk(actor,1);actor->mMode=1;anm_init(actor,7,2.0f,0,1.0f,-1);break;
    case 1: {
        if(!animationStopped(actor))break;
        actor->mMode=2;actor->m3A4[0]=30;
        gabi::Local<dBgS_LinChk> line;
        const dBgS_LinChk_vt vt={0x1000ED34,0x1000ED44,0x1000ED64,0x1000ED54};
        dBgS_LinChk_ct(line.get(),vt,false);
        gabi::call(0x025F1884,gabi::load<u32>(0x1018C7B0),read<s16>(actor,0x2FA));
        gabi::Local<cXyz> offset,transformed;
        offset->x=0;offset->y=0;offset->z=10000;
        gabi::call(0x0200FCD8,offset.get(),transformed.get());
        offset->x=read<f32>(actor,0x2EC);offset->y=read<f32>(actor,0x2F0)+100;offset->z=read<f32>(actor,0x2F4);
        gabi::call(0x028E8D88,transformed.get(),offset.get(),transformed.get());
        gabi::call(0x024F1AFC,line.get(),offset.get(),transformed.get(),actor);
        gabi::call(0x028E90D4,jointMatrix(morfModel(actor),24),gabi::load<u32>(0x1018C7B0));
        offset->x=0;offset->y=0;offset->z=0;
        gabi::call(0x0200FCD8,offset.get(),transformed.get());
        u32 play=gabi::call<u32>(0x025200D4);
        if(gabi::call<s32>(0x02008860,play+0x12A0,line.get())) {
            u32 p=gabi::ea(line.get());offset->x=gabi::load<f32>(p+0x30);offset->y=gabi::load<f32>(p+0x34);offset->z=gabi::load<f32>(p+0x38);
        }
        s16 angle=gabi::call<s16>(0x020195B0,(f32)transformed->x-(f32)offset->x,(f32)transformed->z-(f32)offset->z);
        actor->m6A0=read<s16>(actor,0x32A)-angle+0x7058+gabi::load<s16>(0x1047B698);
        actor->m6A8=100;actor->m6A4=gabi::load<s16>(0x1047B692)+0x80;
        destroyMovementLine(line.get());break;
    }
    case 2:
        if(actor->m3A4[0]==0) {
            s16 step=(s16)((s16)gabi::ftoi(actor->m69C)+(s16)actor->m6A4);
            gabi::call(0x0200F428,member<s16>(actor,0x8D6),(s16)actor->m6A0,1,step);
            actor->m694=(f32)actor->m694+(f32)actor->m698;
            actor->m698=(f32)actor->m698+(f32)actor->m69C;
            actor->m69C=(f32)actor->m69C+(gabi::load<f32>(0x1047BCF8)+3.0f);
            f32 limit=gabi::load<f32>(0x1047BCDC)+1000.0f;
            if(actor->m69C>limit)actor->m69C=limit;
            if(!(actor->m694<15884.0f)) {
                actor->m694=15884.0f;actor->m6A4=gabi::load<s16>(0x1047B694)+0x800;
                if(actor->m698>gabi::load<f32>(0x1047BCD4)+700.0f) {
                    actor->m698=-((f32)actor->m698*(gabi::load<f32>(0x1047BCD8)+0.4f));
                    sound(actor,0x6998,actor->m6A8);
                    if(actor->m6A8>=30)actor->m6A8=(u32)actor->m6A8-20;
                    if(actor->m6A6==0)actor->m6A6=gabi::load<s16>(0x1047B68A)+30;
                } else {actor->m698=0;actor->m3A4[0]=0;actor->mMode=3;}
            }
        }
        break;
    case 3:
        if(actor->m3A4[0]==0) {
            kieru_brk(actor,2);actor->m3A4[0]=10;actor->mMode=4;
            actor->mBokoID=gabi::call<u32>(0x025D5834,0x1CF,5,member<cXyz>(actor,0x314),read<s8>(actor,0x326),0,0,-1,0);
        }
        break;
    case 4: {
        gabi::Local<be<u32>> id;*id.get()=actor->mBokoID;
        u32 boko=actor->mBokoID==0xFFFFFFFFu?0:gabi::call<u32>(0x025D5218,0x025E1234,id.get());
        if(!boko)break;
        if(!(gabi::load<u32>(boko+0x2E0)&0x2000))gabi::call(0x025D9D0C,boko,0);
        u32 model=read<u32>(actor,0x3E4),destination=gabi::load<u32>(boko+0x3B4);
        u32 source=model?model+0xC8:0;
        if(destination) {
            f32 matrix[12];for(u32 i=0;i<12;++i)matrix[i]=gabi::load<f32>(source+i*4);
            for(u32 i=0;i<12;++i)gabi::store<f32>(destination+0xC8+i*4,matrix[i]);
        }
        if(actor->m3A4[0]==0) {
            fopAcM_delete(actor);gabi::store<u8>(boko+0x434,1);gabi::call(0x025D9D24,boko);
        }
        break;
    }
    }
}
static void flyingAction(fganon_class* actor,bool second) {
    u32 player=gabi::load<u32>(gabi::call<u32>(0x025200D4)+0x5B2C);
    gabi::Local<dBgS_LinChk> line;
    const dBgS_LinChk_vt vt={0x1000ED34,0x1000ED44,0x1000ED64,0x1000ED54};
    dBgS_LinChk_ct(line.get(),vt,false);
    if(((actor->m384&15)==0&&gabi::call<f32>(0x020198D8,1.0f)<0.5f)||actor->mMode==-10)
        actor->m398=gabi::load<s16>(player+0x32A);
    switch((s16)actor->mMode) {
    case -10: {
        deru_brk(actor);
        for(u32 i=0;i<3;++i)write<u32>(actor,0x314+4*i,read<u32>(actor,0x2EC+4*i));
        write<f32>(actor,0x318,gabi::load<f32>(player+0x318)+hioFloat(second?0x18:0x10));
        u32 play=gabi::call<u32>(0x025200D4);
        write<s16>(actor,0x32A,gabi::call<s16>(0x025D6894,actor,gabi::load<u32>(play+0x5B2C)));
        actor->mMode=1;anm_init(actor,0x16,20.0f,2,1.0f,-1);actor->m3A4[1]=second?40:60;
        break;
    }
    case 0:
        anm_init(actor,0x16,20.0f,2,1.0f,-1);actor->mMode=(s16)actor->mMode+1;actor->m3A0=0;
        actor->m3A4[1]=(s16)gabi::ftoi(gabi::call<f32>(0x020198D8,50.0f)+50.0f);
        [[fallthrough]];
    case 1: {
        if(actor->m68A==1)anm_init(actor,0x16,10.0f,2,1.0f,-1);
        gabi::call(0x025F1884,gabi::load<u32>(0x1018C7B0),(s16)actor->m398);
        gabi::Local<cXyz> offset,transformed,temporary;
        offset->x=0;offset->z=hioFloat(second?0x14:0xC);offset->y=hioFloat(second?0x18:0x10);
        gabi::call(0x0200FCD8,offset.get(),transformed.get());
        gabi::call(0x0201AD78,player+0x314,temporary.get(),transformed.get());
        actor->m38C=*temporary.get();
        gabi::call(0x0201ADE0,member<cXyz>(actor,0x5C0),temporary.get(),member<cXyz>(actor,0x314));
        *offset.get()=*temporary.get();
        f32 length=gabi::call<f32>(0x028F4384,gabi::call<f32>(0x028E8DD0,offset.get()));
        if(length>gabi::load<f32>(0x1047B640)+300.0f)
            gabi::call(0x0200ED84,member<f32>(actor,0x370),gabi::load<f32>(0x1047B64C)+30.0f,1.0f,gabi::load<f32>(0x1047B644)+2.0f);
        else gabi::call(0x0200EDC8,member<f32>(actor,0x370),1.0f,gabi::load<f32>(0x1047B648)+1.0f);
        actor->m39C=gabi::load<f32>(0x1047B624)+1500.0f;
        u32 play=gabi::call<u32>(0x025200D4);
        s16 yaw=gabi::call<s16>(0x025D6894,actor,gabi::load<u32>(play+0x5B2C));
        gabi::call(0x0200F428,member<s16>(actor,0x32A),yaw,10,0x400);
        gabi::call(0x0200ED84,member<f32>(actor,0x5F0),gabi::load<f32>(0x1047B63C)+50.0f,1.0f,2.0f);
        break;
    }
    }
    pos_move(actor,0);fly_se_set(actor);
    if(actor->m3A4[1]==0) {actor->mAction=second?11:6;actor->mMode=0;}
    if(actor->m3A4[2]==0) {actor->mAction=2;actor->mMode=0;}
    destroyMovementLine(line.get());
}
static void shotAction(fganon_class* actor) {
    gabi::call(0x025200D4);
    u32 play=gabi::call<u32>(0x025200D4);
    s16 yaw=gabi::call<s16>(0x025D6894,actor,gabi::load<u32>(play+0x5B2C));
    gabi::call(0x0200F428,member<s16>(actor,0x32A),yaw,10,0x400);
    switch((s16)actor->mMode) {
    case 0:
        if(actor->m408!=0)break;
        anm_init(actor,0x10,10.0f,2,1.0f,-1);actor->mMode=(s16)actor->mMode+1;actor->m3A4[0]=40;
        monsterSound(actor,0x493A);[[fallthrough]];
    case 1:
        if(actor->m3A4[0]==30)actor->m408=1;
        if(actor->m3A4[0]<30)sound(actor,0x5152);
        if(actor->m3A4[0]==0) {
            anm_init(actor,0xD,3.0f,0,1.0f,-1);actor->mMode=(s16)actor->mMode+1;
            monsterSound(actor,0x493C);sound(actor,0x5954);
        }
        break;
    case 2:
        if(gabi::ftoi(gabi::load<f32>(read<u32>(actor,0x3DC)+0x9C))==(s32)gabi::load<s16>(0x1047B688)+14) {
            actor->m688=0;actor->m687=0;actor->m689=0;actor->m409=1;
        }
        if(animationStopped(actor)) {
            anm_init(actor,0x16,3.0f,2,1.0f,-1);actor->mMode=(s16)actor->mMode+1;
        }
        [[fallthrough]];
    case 3:
        if(actor->m408==5) {
            gabi::Local<cXyz> temporary,offset;
            gabi::call(0x0201ADE0,member<cXyz>(actor,0x614),temporary.get(),member<cXyz>(actor,0x37C));
            *offset.get()=*temporary.get();
            f32 length=gabi::call<f32>(0x028F4384,gabi::call<f32>(0x028E8DD0,offset.get()));
            f32 limit=gabi::fmadds((f32)actor->m404,gabi::load<f32>(0x1047B610)+10.0f,400.0f)+gabi::load<f32>(0x1047B614);
            if(!(length<limit))break;
            if(actor->m689==0) {
                s32 animation=gabi::call<f32>(0x020198D8,1.0f)<0.5f?0x13:0x14;
                anm_init(actor,animation,0.0f,0,1.0f,-1);actor->mMode=(s16)actor->mMode+1;
                monsterSound(actor,0x493E);sound(actor,0x5950);
                actor->m687=(s8)((s8)actor->m687+1);
                if(actor->m688>=7||(actor->m687>=4&&gabi::call<f32>(0x020198D8,1.0f)<0.3f))actor->m689=1;
            } else if(actor->m2BC==2) {actor->mAction=2;actor->mMode=0;return;}
        }
        break;
    case 4: {
        u32 morf=read<u32>(actor,0x3DC);
        s32 frame=gabi::ftoi(gabi::load<f32>(morf+0x9C));
        if(frame>=5&&frame<=15)actor->m686=1;
        if(animationStopped(actor)||(actor->m2BC!=0&&!(gabi::load<f32>(morf+0x9C)<20.0f))) {
            anm_init(actor,0x16,10.0f,2,1.0f,-1);actor->mMode=3;
        }
        break;
    }
    }
    gabi::call(0x0200EDC8,member<f32>(actor,0x370),1.0f,gabi::load<f32>(0x1047B648)+1.0f);
    pos_move(actor,0);fly_se_set(actor);
    if(actor->mMode>=2&&actor->m408==0) {actor->mAction=5;actor->mMode=0;}
    else if(actor->mMode==3||actor->mMode==4) {
        s32 reverb=gabi::call<s32>(0x02520540,read<s8>(actor,0x326));
        gabi::call(0x025E1A7C,0x6237,member<cXyz>(actor,0x614),100,reverb);
    }
}
static void spinAction(fganon_class* actor) {
    u32 player=gabi::load<u32>(gabi::call<u32>(0x025200D4)+0x5B2C);
    gabi::Local<dBgS_LinChk> line;
    const dBgS_LinChk_vt vt={0x1000ED34,0x1000ED44,0x1000ED64,0x1000ED54};
    dBgS_LinChk_ct(line.get(),vt,false);
    s32 frame=gabi::ftoi(gabi::load<f32>(read<u32>(actor,0x3DC)+0x9C));
    switch((s16)actor->mMode) {
    case 0: actor->mMode=1;write<s16>(actor,0x32A,gabi::load<s16>(player+0x32A));[[fallthrough]];
    case 1: {
        gabi::call(0x025F1884,gabi::load<u32>(0x1018C7B0),read<s16>(actor,0x32A));
        gabi::Local<cXyz> offset,transformed,temporary;
        offset->x=0;offset->y=hioFloat(0x2C)+100.0f;offset->z=hioFloat(0x30);
        gabi::call(0x0200FCD8,offset.get(),transformed.get());
        gabi::call(0x0201AD78,player+0x314,temporary.get(),transformed.get());
        *member<cXyz>(actor,0x314)=*temporary.get();
        gabi::call(0x024F1AFC,line.get(),player+0x37C,member<cXyz>(actor,0x314),actor);
        u32 play=gabi::call<u32>(0x025200D4);
        if(gabi::call<s32>(0x02008860,play+0x12A0,line.get())) {
            write<s16>(actor,0x32A,(s16)gabi::ftoi(gabi::call<f32>(0x02019918,32768.0f)));break;
        }
        write<f32>(actor,0x318,read<f32>(actor,0x318)-100.0f);
        anm_init(actor,0x16,1.0f,2,1.0f,-1);actor->mMode=2;actor->m3A4[0]=30;deru_brk(actor);
        [[fallthrough]];
    }
    case 2:
        if(actor->m3A4[0]==0) {
            anm_init(actor,9,2.0f,0,1.0f,-1);write<f32>(actor,0x370,0);actor->mMode=3;
            monsterSound(actor,0x493C);
        }
        break;
    case 3:
        if(frame==14)sound(actor,0x5951);
        actor->m685=1;write<u8>(actor,0xC70,8);
        if(frame==hioShort(0x3E))actor->m6AC=hioShort(0x40)-hioShort(0x3E);
        gabi::call(0x0200ED84,member<f32>(actor,0x370),(u32)(frame-13)<12?20.0f:0.0f,1.0f,4.0f);
        write<s16>(actor,0x322,read<s16>(actor,0x32A));write<s16>(actor,0x320,0);pos_move(actor,1);
        if(animationStopped(actor)) {actor->mAction=2;actor->mMode=0;}
        break;
    }
    destroyMovementLine(line.get());
}
s32 move(fganon_class* actor) {
    WWHD_FUNC(0x0213B3AC,s32,actor);
    u32 play=gabi::call<u32>(0x025200D4);
    if(!(gabi::load<u32>(play+0x5CD8)&0x10000)) {
        play=gabi::call<u32>(0x025200D4);
        if(!(gabi::load<u32>(play+0x5CD8)&0x100000)&&actor->mAction!=0) {
            u32 type=actor->mAction==10?0x1000ED94:(actor->m2BC==1?0x1000EDAC:0x1000EDA0);
            u32 camera=gabi::call<u32>(0x024F8044);
            gabi::call(0x02514EE4,camera,type,0);
        }
    }
    if(actor->m2BC==0&&actor->mAction<20) {
        play=gabi::call<u32>(0x025200D4);
        u32 player=gabi::load<u32>(play+0x5B2C);
        if(actor->mAction==0&&gabi::load<f32>(player+0x318)<710.0f)return 0;
        if(actor->mAction!=0&&actor->mAction!=2&&gabi::load<f32>(player+0x318)<710.0f) {actor->mAction=2;actor->mMode=0;}
    }
    s32 result=0;
    switch((s16)actor->mAction) {
    case 0:standbyAction(actor);break;
    case 1:appearAction(actor);break;
    case 2:disappearAction(actor);break;
    case 3:defeatedAction(actor);result=1;break;
    case 5:flyingAction(actor,false);result=1;break;
    case 6:shotAction(actor);result=1;break;
    case 7:spinAction(actor);result=1;break;
    case 8:down(actor);result=1;break;
    case 9:flyingAction(actor,true);result=1;break;
    case 10:spinattack2(actor);break;
    case 11:shot2(actor);result=1;break;
    case 20:start(actor);break;
    case 21:end(actor);break;
    case 22:last_end(actor);break;
    }
    damage_check(actor);actor->m68C=0;
    if(gabi::ea(actor)+0xAF0) {
        for(u32 i=0;i<3;++i)write<f32>(actor,0x314+i*4,read<f32>(actor,0x314+i*4)+read<f32>(actor,0xAF0+i*4));
    }
    if(actor->m3B0>0.01f) {
        gabi::Local<cXyz> offset,transformed;
        offset->x=0;offset->y=0;offset->z=actor->m3B0;
        gabi::call(0x025F1884,gabi::load<u32>(0x1018C7B0),(s16)actor->m3B4);
        gabi::call(0x025F1BF4,gabi::load<u32>(0x1018C7B0),(s16)actor->m3B6);
        gabi::call(0x0200FCD8,offset.get(),transformed.get());
        gabi::call(0x028E8D88,member<cXyz>(actor,0x314),transformed.get(),member<cXyz>(actor,0x314));
        gabi::call(0x0200EDC8,member<f32>(actor,0x5E4),1.0f,7.0f);
    }
    return result;
}
VERIFY(0x0213B3AC,move);

static f32 regFloat(u32 index) {return gabi::load<f32>(0x1047B610+4*index);}
static s16 regShort(u32 index) {return gabi::load<s16>(0x1047B688+2*index);}
static f32 angleSine(s32 angle) {return gabi::load<f32>(0x104A44F8+((u16)angle>>3)*8);}
static f32 angleCosine(s32 angle) {return gabi::load<f32>(0x104A44FC+((u16)angle>>3)*8);}
static void copyMatrixStorage(u32 source,u32 destination) {
    f32 matrix[12];for(u32 i=0;i<12;++i)matrix[i]=gabi::load<f32>(source+4*i);
    for(u32 i=0;i<12;++i)gabi::store<f32>(destination+4*i,matrix[i]);
}
static void executeAnimation(fganon_class* actor) {
    gabi::call(0x025200D4);
    gabi::Local<cXyz> zero,offset,temporary,delta,relative;
    zero->x=0;zero->y=0;zero->z=0;
    if(actor->mB89>=2) {
        actor->mB89=(s8)((s8)actor->mB89-1);
        if(actor->mB89==1)gabi::call(0x025E1918,0x80000047u);
    }
    gabi::call(0x025DE508,0x02134A90,actor);
    u32 env=gabi::call<u32>(0x02555D0C);
    gabi::call(0x025626A4,env,0,member<cXyz>(actor,0x314),member<void>(actor,0x110));
    s16 action=actor->mAction;
    if((action==5||action==7||(action>=9&&action<=11))&&actor->m68B!=0&&actor->m68A==0) {
        actor->m68B=0;anm_init(actor,0x17,6.0f,2,1.0f,-1);actor->m68A=60;monsterSound(actor,0x4941);
    }
    actor->m384=(s16)((s16)actor->m384+1);
    for(u32 i=0;i<5;++i)if(actor->m3A4[i]!=0)actor->m3A4[i]=(s16)((s16)actor->m3A4[i]-1);
    if(actor->m68A!=0)actor->m68A=(s8)((s8)actor->m68A-1);
    if(actor->m3AE!=0)actor->m3AE=(s16)((s16)actor->m3AE-1);
    if(actor->m3B8!=0)actor->m3B8=(s16)((s16)actor->m3B8-1);
    if(gabi::load<u8>(0x10463D51)==0) {
        write<u32>(actor,0x39C,actor->mbIsMaterialized?4:0);
        if(move(actor)!=0) {
            u32 play=gabi::call<u32>(0x025200D4);
            gabi::call(0x024F08A8,member<void>(actor,0x92C),play+0x12A0);
        }
        gabi::call(0x025E535C,read<u32>(actor,0x3DC),member<cXyz>(actor,0x37C),0,0);
        gabi::call(0x025E742C,read<u32>(actor,0x3E0));
        gabi::call(0x025E742C,read<u32>(actor,0x3E8));
    }
    s16 phase=actor->m384;
    s16 adjustment=regShort(6);
    f32 amplitude=actor->m3BC;
    offset->x=angleSine(phase*(adjustment+700))*amplitude;
    offset->y=angleSine(phase*(adjustment+1000))*amplitude;
    offset->z=angleCosine(phase*(adjustment+800))*amplitude;
    gabi::call(0x0200EDC8,member<f32>(actor,0x5F0),1.0f,1.0f);
    gabi::call(0x0201AD78,member<cXyz>(actor,0x314),temporary.get(),offset.get());
    gabi::call(0x0201ADE0,member<cXyz>(actor,0xDC0),delta.get(),temporary.get());
    *zero.get()=*delta.get();
    gabi::call(0x025F1884,gabi::load<u32>(0x1018C7B0),(s16)-read<s16>(actor,0x32A));
    gabi::call(0x0200FCD8,zero.get(),relative.get());
    write<s16>(actor,0xDCC,(s16)gabi::ftoi((f32)relative->x*(gabi::load<f32>(0x1047C030)-60.0f)));
    write<s16>(actor,0xDCE,(s16)gabi::ftoi((f32)relative->z*(gabi::load<f32>(0x1047C034)-60.0f)));
    u32 model=morfModel(actor);
    f32 scaleX=read<f32>(actor,0x330),scaleY=read<f32>(actor,0x334),scaleZ=read<f32>(actor,0x338);
    gabi::store<f32>(model+0xC0,scaleY);gabi::store<f32>(model+0xC4,scaleZ);gabi::store<f32>(model+0xBC,scaleX);
    gabi::call(0x0201AD78,member<cXyz>(actor,0x314),temporary.get(),offset.get());
    *member<cXyz>(actor,0xDC0)=*temporary.get();
    gabi::call(0x028E93CC,0x1048D0CC,(f32)temporary->x,(f32)temporary->y,(f32)temporary->z);
    phase=actor->m384;
    f32 wobble=(f32)(s16)actor->m3B8*(regFloat(14)+500.0f);
    s16 yaw=(s16)(read<s16>(actor,0x32A)+(s16)gabi::ftoi(angleSine(phase*0x2100)*wobble));
    s16 pitchWobble=(s16)gabi::ftoi(angleCosine(phase*0x2300)*wobble);
    gabi::call(0x025F1C28,0x1048D0CC,yaw);
    gabi::call(0x025F1BF4,0x1048D0CC,(s16)(read<s16>(actor,0x328)+pitchWobble));
    gabi::call(0x025F1C5C,0x1048D0CC,read<s16>(actor,0x32C));
    copyMatrixStorage(0x1048D0CC,model+0xC8);
    gabi::call(0x025E55A0,read<u32>(actor,0x3DC));
    zero->x=0;zero->y=0;zero->z=0;
    gabi::call(0x028E90D4,jointMatrix(model,27),gabi::load<u32>(0x1018C7B0));
    gabi::call(0x0200FCD8,zero.get(),member<cXyz>(actor,0x37C));
    f32 eyeX=read<f32>(actor,0x37C),eyeY=read<f32>(actor,0x380),eyeZ=read<f32>(actor,0x384);
    write<f32>(actor,0x390,eyeX);write<f32>(actor,0x398,eyeZ);write<f32>(actor,0x394,eyeY+30.0f);
    if(actor->m2D0==0) {
        gabi::call(0x028E90D4,jointMatrix(morfModel(actor),24),gabi::load<u32>(0x1018C7B0));
        gabi::call(0x025F1C28,gabi::load<u32>(0x1018C7B0),(s16)(gabi::load<s16>(0x1047BB0A)-400));
        gabi::call(0x025F1BF4,gabi::load<u32>(0x1018C7B0),(s16)(gabi::load<s16>(0x1047BB0C)-400));
        gabi::call(0x025F1C5C,gabi::load<u32>(0x1018C7B0),(s16)(gabi::load<s16>(0x1047BB0E)+0x8000));
        gabi::call(0x0200FAD8,1,gabi::load<f32>(0x1047BAB4),gabi::load<f32>(0x1047BAB8),gabi::load<f32>(0x1047BABC)+85.0f);
        gabi::call(0x0200FAD8,1,regFloat(12),regFloat(13),regFloat(14)+90.0f);
        gabi::call(0x025F1C5C,gabi::load<u32>(0x1018C7B0),(s16)actor->m6A0);
        gabi::call(0x025F1BF4,gabi::load<u32>(0x1018C7B0),(s16)gabi::ftoi(actor->m694));
        gabi::call(0x025F1C5C,gabi::load<u32>(0x1018C7B0),(s16)-(s16)actor->m6A0);
        s16 rotation=0;
        if(actor->m6A6!=0) {
            s16 countdown=actor->m6A6;
            f32 displacement=angleSine(countdown*(regShort(2)+0x1800))*(f32)countdown;
            rotation=(s16)gabi::ftoi(displacement*(regFloat(14)+400.0f));
            actor->m6A6=(s16)(countdown-1);
        }
        gabi::call(0x025F1C5C,gabi::load<u32>(0x1018C7B0),(s16)((s16)actor->m6A2+rotation));
        gabi::call(0x0200FAD8,1,-regFloat(12),-regFloat(13),-(regFloat(14)+90.0f));
        copyMatrixStorage(gabi::load<u32>(0x1018C7B0),read<u32>(actor,0x3E4)+0xC8);
    }
}
static void emitterMatrix(u32 emitter,u32 matrix) {
    if(emitter)gabi::call(0x028249B0,matrix,emitter+0x1F0,emitter+0x22C);
}
static f32 vectorLength(cXyz* vector) {
    return gabi::call<f32>(0x028F4384,gabi::call<f32>(0x028E8DD0,vector));
}
static void aimEnergyBall(fganon_class* actor,cXyz* delta,cXyz* offset,bool initial=false) {
    f32 x=delta->x,z=delta->z;
    u32 matrix=gabi::load<u32>(0x1018C7B0);
    s16 yaw=gabi::call<s16>(0x020195B0,x,z);
    gabi::call(0x025F1884,matrix,yaw);
    matrix=gabi::load<u32>(0x1018C7B0);
    f32 horizontal=gabi::call<f32>(0x028F4384,gabi::fmadds((f32)delta->x,(f32)delta->x,(f32)delta->z*(f32)delta->z));
    s16 pitch=gabi::call<s16>(0x020195B0,(f32)delta->y,horizontal);
    gabi::call(0x025F1BF4,matrix,(s16)-pitch);
    offset->z=initial?hioFloat(actor->m2BC==0?0x1C:0x20):(f32)actor->m404;
    if(initial)actor->m404=offset->z;
    gabi::call(0x0200FCD8,offset,member<cXyz>(actor,0x62C));
}
static bool energyBallWallCheck(fganon_class* actor) {
    gabi::Local<dBgS_LinChk> line;
    const dBgS_LinChk_vt vt={0x1000ED34,0x1000ED44,0x1000ED64,0x1000ED54};
    dBgS_LinChk_ct(line.get(),vt,false);
    gabi::Local<cXyz> difference,scaled,temporary,end;
    gabi::call(0x0201ADE0,member<cXyz>(actor,0x614),difference.get(),member<cXyz>(actor,0x620));
    gabi::call(0x0201AE48,difference.get(),scaled.get(),1.05f);
    gabi::call(0x0201AD78,member<cXyz>(actor,0x614),temporary.get(),scaled.get());
    *end.get()=*temporary.get();
    gabi::call(0x024F1AFC,line.get(),member<cXyz>(actor,0x614),end.get(),actor);
    u32 play=gabi::call<u32>(0x025200D4);
    bool hit=gabi::call<s32>(0x02008860,play+0x12A0,line.get())!=0;
    if(hit)for(u32 i=0;i<3;++i)write<u32>(actor,0x614+i*4,gabi::load<u32>(gabi::ea(line.get())+0x30+i*4));
    destroyMovementLine(line.get());return hit;
}
static void energyBallAction(fganon_class* actor) {
    u32 player=gabi::load<u32>(gabi::call<u32>(0x025200D4)+0x5B2C);
    gabi::Local<cXyz> offset,delta,temporary,collisionPosition;
    offset->x=0;offset->y=0;offset->z=0;
    if(actor->m408==0)return;
    struct AttackInfo {u8 data[0x1C];}; /* CcAtInfo: cc_at_check 025192A8 writes +0x18 (stack guard sweep 2026-10-05) */
    gabi::Local<AttackInfo> info;u32 infoAddress=gabi::ea(info.get());gabi::store<u32>(infoAddress+0x14,0);
    if(actor->m408==35) {
        if(actor->m688!=0) {gabi::call(0x025E1E6C,8);actor->m688=0;}
        invalidateEmitter(actor,0x600);invalidateEmitter(actor,0x604);actor->m408=0;return;
    }
    if(actor->m408==1) {
        invalidateEmitter(actor,0x600);invalidateEmitter(actor,0x604);
        write<u32>(actor,0x600,createParticle(0x81CE,member<cXyz>(actor,0x314)));
        write<u32>(actor,0x604,createParticle(0x81CF,member<cXyz>(actor,0x314)));
        actor->m408=2;
    }
    if(actor->m408==2) {
        gabi::call(0x028E90D4,jointMatrix(morfModel(actor),14),gabi::load<u32>(0x1018C7B0));
        gabi::call(0x0200FAD8,1,gabi::load<f32>(0x1047BCD0)+30.0f,gabi::load<f32>(0x1047BCD4)+30.0f,gabi::load<f32>(0x1047BCD8));
        gabi::call(0x0200FCD8,offset.get(),member<cXyz>(actor,0x614));
        for(u32 i=0;i<2;++i)emitterMatrix(read<u32>(actor,0x600+4*i),gabi::load<u32>(0x1018C7B0));
        if(actor->m409!=0) {
            actor->m409=0;actor->m408=3;
            gabi::call(0x0201ADE0,player+0x37C,temporary.get(),member<cXyz>(actor,0x614));
            *delta.get()=*temporary.get();delta->y=(f32)delta->y-(regFloat(18)+50.0f);
            aimEnergyBall(actor,delta.get(),offset.get(),true);actor->m40A=5;
            gabi::call(0x02516094,member<void>(actor,0x76C));
        }
    }
    if(actor->m408>=3) {
        actor->m3EC=actor->m3E0;
        gabi::call(0x028E8D88,member<cXyz>(actor,0x614),member<cXyz>(actor,0x62C),member<cXyz>(actor,0x614));
        gabi::call(0x028E93CC,0x1048D0CC,read<f32>(actor,0x614),read<f32>(actor,0x618),read<f32>(actor,0x61C));
        for(u32 i=0;i<2;++i)emitterMatrix(read<u32>(actor,0x600+4*i),0x1048D0CC);
        bool sword=false,bottle=false,reflected=false;
        gabi::call(0x0201ADE0,member<cXyz>(actor,0x37C),temporary.get(),member<cXyz>(actor,0x614));
        *delta.get()=*temporary.get();delta->y=(f32)delta->y-50.0f;
        if(vectorLength(delta.get())<(f32)actor->m404+200.0f+regFloat(3)) {
            if(actor->m686!=0)sword=true;
            else if(actor->m408==5) {
                if(actor->m2BC!=2)actor->m68C=1;
                else {if(actor->m40A!=0)return;actor->m40A=50;actor->mAction=2;actor->mMode=0;return;}
            }
        }
        if(actor->m408==4) {
            u32 vt=gabi::load<u32>(player+0xB4);
            if(gabi::call_ptr<s32>(gabi::load<u32>(vt+0x64),player)) {
                gabi::call(0x0201ADE0,player+0x37C,temporary.get(),member<cXyz>(actor,0x614));
                *delta.get()=*temporary.get();delta->y=(f32)delta->y-30.0f;
                bottle=vectorLength(delta.get())<regFloat(2)+100.0f;
            }
        }
        if((gabi::call<s32>(0x025162A4,member<void>(actor,0x640))||sword||bottle)&&actor->m40A==0) {
            if(gabi::call<s32>(0x025162A4,member<void>(actor,0x640))) {
                gabi::store<u32>(infoAddress,gabi::call<u32>(0x02516300,member<void>(actor,0x640)));
                gabi::call(0x02518DB0,info.get());
            } else {gabi::store<u32>(infoAddress,0);gabi::store<u8>(infoAddress+0xA,0xFF);}
            u32 hit=gabi::load<u32>(infoAddress);bool masterSword=false;
            if(hit&&gabi::load<u8>(infoAddress+0xA)==1&&(gabi::load<u32>(hit+0x10)&2)) {
                u8 equip=gabi::load<u8>(gabi::load<u32>(0x101F84DC)+0x2E);
                masterSword=equip==0x39||equip==0x3E||equip==0x3A;
            }
            if(masterSword||bottle) {
                gabi::call(0x0201ADE0,member<cXyz>(actor,0x37C),temporary.get(),member<cXyz>(actor,0x614));
                *delta.get()=*temporary.get();delta->y=(f32)delta->y-(regFloat(17)+30.0f);
                aimEnergyBall(actor,delta.get(),offset.get());actor->m408=5;
                gabi::store<u8>(0x101EACB7,2);
                s32 status=(s8)actor->m688+2;if(status>7)status=7;gabi::call(0x025E1E6C,status);
                actor->m688=(s8)((s8)actor->m688+1);
                u32 soundPlayer=gabi::load<u32>(gabi::call<u32>(0x025200D4)+0x5B2C);
                if(soundPlayer&&soundPlayer+0x37C) {
                    s32 reverb=gabi::call<s32>(0x02520540,gabi::load<s8>(soundPlayer+0x326));
                    gabi::call(0x025E1A40,0x5954,soundPlayer+0x37C,0,reverb);
                }
                gabi::call(0x025B8B68,gabi::load<u32>(0x101F84DC)+0x644,0x3F20);reflected=true;
            } else if(sword) {
                gabi::call(0x0201ADE0,player+0x37C,temporary.get(),member<cXyz>(actor,0x614));
                *delta.get()=*temporary.get();delta->y=(f32)delta->y-(regFloat(18)+50.0f);
                aimEnergyBall(actor,delta.get(),offset.get());actor->m408=4;reflected=true;
            }
            if(reflected) {
                actor->m404=(f32)actor->m404+hioFloat(actor->m2BC==0?0x24:0x28);
                gabi::Local<csXyz> angles;gabi::call(0x0201A478,angles.get(),0,0,0);
                angles->y=gabi::call<s16>(0x020195B0,read<f32>(actor,0x62C),read<f32>(actor,0x634));
                createParticle(0x81F0,member<cXyz>(actor,0x614),angles.get());
                s32 reverb=gabi::call<s32>(0x02520540,read<s8>(actor,0x326));
                gabi::call(0x025E1A40,0x28A5,member<cXyz>(actor,0x614),0,reverb);
                gabi::call(0x028E8D88,member<cXyz>(actor,0x614),member<cXyz>(actor,0x62C),member<cXyz>(actor,0x614));actor->m40A=5;
            }
        }
        *collisionPosition.get()=actor->m3E0;
        if(actor->m40A!=0) {actor->m40A=(s8)((s8)actor->m40A-1);collisionPosition->x=-20000;collisionPosition->y=-20000;collisionPosition->z=20000;}
        if(actor->m408==3) {gabi::call(0x025167C0,member<void>(actor,0x76C),collisionPosition.get());actor->m408=4;}
        else gabi::call(0x025167E4,member<void>(actor,0x76C),collisionPosition.get());
        gabi::call(0x02018C8C,member<void>(actor,0x884),regFloat(6)+25.0f);
        gabi::call(0x02018D40,member<void>(actor,0x758),collisionPosition.get());
        gabi::call(0x02018C8C,member<void>(actor,0x758),regFloat(7)+100.0f);
        u32 play=gabi::call<u32>(0x025200D4);gabi::call(0x0200E240,play+0x26A4,member<void>(actor,0x640));
        play=gabi::call<u32>(0x025200D4);gabi::call(0x0200E240,play+0x26A4,member<void>(actor,0x76C));
        if(actor->m68C!=0||energyBallWallCheck(actor)||gabi::call<s32>(0x025160DC,member<void>(actor,0x76C))) {
            if(actor->m688!=0) {
                s32 status=actor->m68C!=0?10:(gabi::call<s32>(0x025160DC,member<void>(actor,0x76C))?9:8);
                gabi::call(0x025E1E6C,status);actor->m688=0;
            }
            actor->m408=0;
            u32 emitter=read<u32>(actor,0x600);if(emitter)gabi::store<u8>(emitter+0x247,0);
            invalidateEmitter(actor,0x604);
            gabi::Local<csXyz> angles;gabi::call(0x0201A478,angles.get(),0,0,0);
            createParticle(0x81EE,member<cXyz>(actor,0x614),angles.get());createParticle(0x81EF,member<cXyz>(actor,0x614),angles.get());
            s32 reverb=gabi::call<s32>(0x02520540,read<s8>(actor,0x326));gabi::call(0x025E1A40,0x6A35,member<cXyz>(actor,0x614),100,reverb);
            if(gabi::call<s32>(0x025160DC,member<void>(actor,0x76C))) {
                u32 hit=gabi::call<u32>(0x02516178,member<void>(actor,0x76C));
                u32 status=hit?gabi::load<u32>(hit+0x44):0;
                u32 source=status?gabi::load<u32>(status+0xC):0;gabi::store<u32>(infoAddress+4,source);
                if(source&&gabi::load<s16>(source+8)==0xA8&&actor->mAction!=22) {
                    actor->m68B=1;actor->mAction=5;actor->mMode=1;actor->m3A4[1]=(s16)gabi::ftoi(gabi::call<f32>(0x020198D8,30.0f)+70.0f);
                }
            }
        }
    }
    gabi::call(0x0201ADE0,member<cXyz>(actor,0x2EC),temporary.get(),member<cXyz>(actor,0x614));
    *delta.get()=*temporary.get();if(vectorLength(delta.get())>10000.0f)actor->m408=35;
}
static void executeCollision(fganon_class* actor) {
    gabi::Local<cXyz> zero,center,weaponPosition;
    zero->x=0;zero->y=0;zero->z=0;
    for(u32 i=0;i<2;++i) {
        u32 slot=0x5F8+4*i,emitter=read<u32>(actor,slot);
        if(actor->mbIsMaterialized!=0&&actor->mAction!=22) {
            if(emitter) {
                u32 index=gabi::load<u32>(0x101B4CBC+4*i);
                u32 matrix=jointMatrix(morfModel(actor),index);
                u32 rotation=gabi::call<u32>(0x0213866C,emitter+0x1F0);
                gabi::call(0x028249B0,matrix,rotation,emitter+0x22C);
            } else {
                u16 id=gabi::load<u16>(0x101B4CC4+2*i);
                write<u32>(actor,slot,createParticle(id,member<cXyz>(actor,0x314)));
            }
        } else if(emitter) {
            gabi::store<s32>(emitter+0x5C,-1);gabi::call(0x0213865C,emitter,1);write<u32>(actor,slot,0);
        }
    }
    if(actor->mbIsMaterialized!=0) {
        gabi::call(0x028E90D4,jointMatrix(morfModel(actor),10),gabi::load<u32>(0x1018C7B0));
        gabi::call(0x0200FCD8,zero.get(),center.get());center->y=(f32)center->y-(regFloat(0)+150.0f);
        u32 flags=read<u32>(actor,0xB58);write<u32>(actor,0xB58,actor->m68F?flags&~1u:flags|1);
    } else {
        u8 hidden=actor->m68F;u32 flags=read<u32>(actor,0xB58);
        center->x=20000;center->y=20000;write<u32>(actor,0xB58,flags&~1u);center->z=-1000.0f*(f32)hidden;
    }
    gabi::call(0x020182E0,member<void>(actor,0xC44),center.get());
    gabi::call(0x02018428,member<void>(actor,0xC44),gabi::load<f32>(0x1047BA9C)+250.0f);
    gabi::call(0x020184DC,member<void>(actor,0xC44),gabi::load<f32>(0x1047BAA0)+70.0f);
    u32 play=gabi::call<u32>(0x025200D4);gabi::call(0x0200E240,play+0x26A4,member<void>(actor,0xB2C));
    weaponPosition->x=-10000;weaponPosition->y=-10000;weaponPosition->z=-10000;
    if(actor->m685!=0) {
        gabi::call(0x02018C8C,member<void>(actor,0xD74),regFloat(13)+60.0f);
        gabi::call(0x028E90D4,jointMatrix(morfModel(actor),24),gabi::load<u32>(0x1018C7B0));
        zero->x=0;zero->y=0;zero->z=100;gabi::call(0x0200FCD8,zero.get(),weaponPosition.get());
        s8 active=actor->m684;actor->m685=0;
        if(active==0) {gabi::call(0x025167C0,member<void>(actor,0xC5C),weaponPosition.get());actor->m684=1;}
        else gabi::call(0x025167E4,member<void>(actor,0xC5C),weaponPosition.get());
    } else {actor->m684=0;gabi::call(0x02018D40,member<void>(actor,0xD74),weaponPosition.get());}
    play=gabi::call<u32>(0x025200D4);gabi::call(0x0200E240,play+0x26A4,member<void>(actor,0xC5C));
    u32 capeID=actor->mCapeID;actor->m686=0;
    u32 cape=gabi::call<u32>(0x0213860C,capeID);
    if(cape) {
        gabi::call(0x028E90D4,jointMatrix(morfModel(actor),(s32)regShort(5)+20),gabi::load<u32>(0x1018C7B0));
        zero->x=regFloat(0)+35.0f;zero->y=regFloat(1);zero->z=regFloat(2)-30.0f;
        gabi::call(0x0200FCD8,zero.get(),cape+0x362C);
        gabi::call(0x028E90D4,jointMatrix(morfModel(actor),(s32)regShort(6)+11),gabi::load<u32>(0x1018C7B0));
        zero->x=regFloat(3)+35.0f;zero->y=regFloat(4);zero->z=regFloat(5)+30.0f;
        gabi::call(0x0200FCD8,zero.get(),cape+0x3638);
        for(u32 i=0;i<3;++i)gabi::store<u32>(cape+0x314+4*i,read<u32>(actor,0x37C+4*i));
        for(u32 i=0;i<3;++i)gabi::store<u16>(cape+0x320+2*i,read<u16>(actor,0x320+2*i));
        f32 target=actor->mbIsMaterialized!=0&&actor->m68E==0?1.0f:0.0f;
        gabi::call(0x0200ED84,cape+0x334,target,1.0f,regFloat(8)+0.1f);
    }
    if(actor->m6AC!=0) {
        actor->m6AC=(s8)((s8)actor->m6AC-1);
        write<f32>(actor,0x3B8,0);write<f32>(actor,0x3BC,10);write<f32>(actor,0x3B4,10000);write<u8>(actor,0x3C4,1);
        write<f32>(actor,0x3B4,hioFloat(0x44));write<f32>(actor,0x3C0,5);
    } else write<f32>(actor,0x3C0,100);
}
static f32 reg8Float(u32 index) {return gabi::load<f32>(0x1047BA90+4*index);}
static s16 reg8Short(u32 index) {return gabi::load<s16>(0x1047BB08+2*index);}
static void setVector(cXyz* vector,f32 x,f32 y,f32 z) {vector->x=x;vector->y=y;vector->z=z;}
static void cameraSwitch(fganon_class* actor,u8 index,bool on) {
    u32 save=gabi::load<u32>(0x101F84DC);
    gabi::call(on?0x025B9E38:0x025B9F7C,save+0x20,index,read<s8>(actor,0x326));
}
static void cameraLaugh(fganon_class* actor) {
    s32 reverb=gabi::call<s32>(0x02520540,read<s8>(actor,0x326));gabi::call(0x025E1A40,0x4938,0,0,reverb);
}
static void playerPosition(u32 player,cXyz* position,s16 angle) {
    u32 vt=gabi::load<u32>(player+0xB4);
    gabi::call_ptr(gabi::load<u32>(vt+0x114),player,position,angle);
}
static void cameraReset(fganon_class* actor,u32 camera) {
    gabi::call(0x02515280,camera+0x248,0);gabi::call(0x02514F38,camera+0x248);gabi::call(0x0259169C);
    u32 play=gabi::call<u32>(0x025200D4);
    gabi::store<u16>(play+0x52B8,gabi::load<u16>(play+0x52B8)|8);actor->mB54=0;
}
static void demoCameraAction(fganon_class* actor) {
    u32 player=gabi::load<u32>(gabi::call<u32>(0x025200D4)+0x5B2C);
    s8 cameraId=gabi::load<s8>(gabi::call<u32>(0x025200D4)+0x5B30);
    u32 camera=gabi::load<u32>(gabi::call<u32>(0x025200D4)+(s32)cameraId*0x34+0x5AF8);
    gabi::Local<cXyz> offset,transformed,temporary;
    s16 mode=actor->mB54;
    switch(mode) {
    case 1:case 50:case 100:
        if(read<u16>(actor,0xF8)!=2) {
            gabi::call(0x025D7B24,actor,2,0xFFFF,0);
            write<u16>(actor,0xFA,read<u16>(actor,0xFA)|2);return;
        }
        actor->mB54=(s16)(mode+1);
        gabi::call(0x02514F2C,camera+0x248);gabi::call(0x02515280,camera+0x248,2);
        actor->mB56=0;actor->mB84=mode==50?55.0f:60.0f;actor->mB80=0;
        if(mode==1) {
            gabi::store<u32>(player+0x428,0);gabi::store<u32>(player+0x430,0x18);gabi::store<u16>(player+0x420,3);
        } else {
            gabi::store<u16>(player+0x420,3);gabi::store<u32>(player+0x428,0);
            if(mode==50)write<s16>(actor,0x32A,0);
            else {
                u32 mainCamera=gabi::load<u32>(gabi::call<u32>(0x025200D4)+0x5AF8);
                for(u32 i=0;i<3;++i)write<u32>(actor,0xD90+4*i,gabi::load<u32>(mainCamera+0xDC+4*i));
                for(u32 i=0;i<3;++i)write<u32>(actor,0xD9C+4*i,gabi::load<u32>(mainCamera+0xE8+4*i));
                u32 play=gabi::call<u32>(0x025200D4);
                write<s16>(actor,0x32A,gabi::call<s16>(0x025D6894,actor,gabi::load<u32>(play+0x5B2C)));
            }
        }
        mode=(s16)(mode+1);break;
    }
    switch(mode) {
    case 2:
        setVector(transformed.get(),-300306.0f,715.0f,-303407.0f);playerPosition(player,transformed.get(),-0x7BCD);
        setVector(member<cXyz>(actor,0xD9C),-300319.0f,812.0f,-303342.0f);
        setVector(member<cXyz>(actor,0xD90),-300440.0f,787.0f,-303137.0f);
        if(actor->mB56==2)cameraLaugh(actor);
        if(actor->mB56==30)gabi::store<u32>(player+0x430,0x35);
        if(actor->mB56==45) {
            initBrk(actor,morfModel(actor),0x1000EBEC,0x22,0x3E0);
            initBrk(actor,read<u32>(actor,0x3E4),0x1000EBEC,0x20,0x3E8);
            sound(actor,0x594E);actor->mbIsMaterialized=1;
        }
        if(actor->mB56!=47)break;
        actor->mB54=3;actor->mB56=0;
        setVector(member<cXyz>(actor,0x314),reg8Float(0)-300294.0f,reg8Float(1)+745.0f,reg8Float(2)-303109.0f);
        setVector(member<cXyz>(actor,0xD90),reg8Float(3)-300169.0f,reg8Float(4)+770.0f,reg8Float(5)-303635.0f);
        actor->mB68=*member<cXyz>(actor,0x314);actor->mB68.y=(f32)actor->mB68.y+(regFloat(0)+160.0f);
        [[fallthrough]];
    case 3:
        if(actor->mB56>60) {
            gabi::call(0x0200ED84,member<f32>(actor,0xD90),-300269.0f,0.1f,100.0f*(f32)actor->mB80);
            gabi::call(0x0200ED84,member<f32>(actor,0xD94),870.0f,0.1f,100.0f*(f32)actor->mB80);
            gabi::call(0x0200ED84,member<f32>(actor,0xD98),-303335.0f,0.1f,300.0f*(f32)actor->mB80);
            actor->mB68.x=read<f32>(actor,0x314);actor->mB68.z=read<f32>(actor,0x31C);
            gabi::call(0x0200ED84,member<f32>(actor,0xDA0),read<f32>(actor,0x318)+215.0f,0.1f,55.0f*(f32)actor->mB80);
            gabi::call(0x0200ED84,member<f32>(actor,0xDB4),0.1f,1.0f,reg8Float(7)+0.01f);
            if(actor->mB56==110)actor->mMode=2;
            if(actor->mB56==180)actor->mMode=4;
            if(actor->mB56==280) {
                actor->mAction=6;actor->mMode=1;actor->m3A4[0]=29;actor->mB54=150;
                if(actor->m2BF!=0xFF)cameraSwitch(actor,actor->m2BF,true);
            }
        }
        break;
    case 51:
        setVector(member<cXyz>(actor,0x314),-300202.0f,715.0f,-301859.0f);
        gabi::call(0x025F1884,gabi::load<u32>(0x1018C7B0),read<s16>(actor,0x32A));
        setVector(offset.get(),regFloat(0)-150.0f,regFloat(1)+20.0f,regFloat(2)+500.0f);
        gabi::call(0x0200FCD8,offset.get(),transformed.get());
        gabi::call(0x0201AD78,member<cXyz>(actor,0x314),temporary.get(),transformed.get());actor->mB5C=*temporary.get();
        setVector(offset.get(),regFloat(3),0,regFloat(5)+280.0f);
        gabi::call(0x0200FCD8,offset.get(),transformed.get());
        gabi::call(0x028E8D88,transformed.get(),member<cXyz>(actor,0x314),transformed.get());transformed->y=715;
        playerPosition(player,transformed.get(),-0x8000);
        actor->mB68=*member<cXyz>(actor,0x37C);actor->mB68.y=(f32)actor->mB68.y+(regFloat(6)-40.0f);
        if(actor->mB56>150) {
            setVector(member<cXyz>(actor,0xD9C),-299898.0f,1188.0f,-301158.0f);
            setVector(member<cXyz>(actor,0xD90),-299703.0f,921.0f,-300851.0f);
            actor->mB54=(s16)((s16)actor->mB54+1);actor->mB56=0;gabi::store<u32>(player+0x430,0x1A);
        }
        break;
    case 52:
        setVector(transformed.get(),-300202.0f,gabi::load<f32>(player+0x318),-301859.0f);playerPosition(player,transformed.get(),0);
        if(actor->mB56==10)actor->mMode=(s16)((s16)actor->mMode+1);
        write<f32>(actor,0x314,(f32)actor->mB68.x-50.0f+regFloat(4));
        write<f32>(actor,0x318,(f32)actor->mB68.y+regFloat(5));
        write<f32>(actor,0x31C,(f32)actor->mB68.z-100.0f+regFloat(6));write<s16>(actor,0x32A,regShort(0)+2000);
        if(actor->mB56==40) {actor->mB54=(s16)((s16)actor->mB54+1);actor->mB56=0;}
        break;
    case 53:
        setVector(member<cXyz>(actor,0xD9C),-300098.0f,580.0f,-301997.0f);
        setVector(member<cXyz>(actor,0xD90),-300274.0f,929.0f,-301770.0f);
        if(actor->mB56!=30)break;
        setVector(member<cXyz>(actor,0xD90),-299703.0f,921.0f,-300851.0f);
        actor->mB54=(s16)((s16)actor->mB54+1);actor->mB56=0;
        write<f32>(actor,0x340,0);actor->mB68=*member<cXyz>(actor,0x37C);
        actor->mB68.y=(f32)actor->mB68.y-(regFloat(11)+30.0f);actor->mMode=(s16)((s16)actor->mMode+1);
        [[fallthrough]];
    case 54:
        gabi::call(0x0200ED84,member<f32>(actor,0xDA0),read<f32>(actor,0x380)-30.0f+regFloat(11),0.1f,20.0f);
        if(actor->mB56!=100)break;
        actor->mB54=55;gabi::store<u32>(player+0x430,0xF);actor->mB56=0;[[fallthrough]];
    case 55:
        if(actor->mB56==20)cameraLaugh(actor);
        for(u32 i=0;i<3;++i)write<u32>(actor,0xD9C+4*i,gabi::load<u32>(player+0x314+4*i));
        actor->mB68.y=(f32)actor->mB68.y+reg8Float(3);
        setVector(offset.get(),0,reg8Float(4)+1500.0f,reg8Float(5)+2000.0f);
        gabi::call(0x025F1884,gabi::load<u32>(0x1018C7B0),(s16)(gabi::load<s16>(player+0x32A)+(s16)actor->mB76+reg8Short(5)));
        gabi::call(0x0200FCD8,offset.get(),transformed.get());actor->mB76=(s16)((s16)actor->mB76+reg8Short(6)+30);
        gabi::call(0x0201AD78,player+0x314,temporary.get(),transformed.get());actor->mB5C=*temporary.get();
        if(actor->mB56==130) {
            if(actor->mSwitchNo!=0xFF)cameraSwitch(actor,actor->mSwitchNo,true);
            if(actor->m2BF!=0xFF)cameraSwitch(actor,actor->m2BF,false);
            actor->mB54=150;fopAcM_delete(actor);cameraReset(actor,camera);return;
        }
        break;
    case 101: {
        gabi::call(0x0200ED84,member<f32>(actor,0xD9C),read<f32>(actor,0x314),0.1f,200.0f);
        gabi::call(0x0200ED84,member<f32>(actor,0xDA0),read<f32>(actor,0x380)-50.0f,0.1f,200.0f);
        gabi::call(0x0200ED84,member<f32>(actor,0xDA4),read<f32>(actor,0x31C),0.1f,200.0f);
        gabi::call(0x025F1884,gabi::load<u32>(0x1018C7B0),read<s16>(actor,0x32A));
        setVector(offset.get(),0,reg8Float(0)-100.0f,reg8Float(1)+400.0f);
        gabi::call(0x0200FCD8,offset.get(),transformed.get());gabi::call(0x028E8D88,transformed.get(),member<cXyz>(actor,0x314),transformed.get());
        gabi::Local<dBgS_LinChk> line;
        const dBgS_LinChk_vt vt={0x1000ED34,0x1000ED44,0x1000ED64,0x1000ED54};dBgS_LinChk_ct(line.get(),vt,false);
        gabi::call(0x024F1AFC,line.get(),member<cXyz>(actor,0xD9C),transformed.get(),actor);
        u32 play=gabi::call<u32>(0x025200D4);
        if(gabi::call<s32>(0x02008860,play+0x12A0,line.get())) {
            u32 p=gabi::ea(line.get());transformed->x=gabi::load<f32>(p+0x30);transformed->y=gabi::load<f32>(p+0x34)+(reg8Float(18)+20.0f);transformed->z=gabi::load<f32>(p+0x38);
        }
        destroyMovementLine(line.get());
        gabi::call(0x0200ED84,member<f32>(actor,0xD90),(f32)transformed->x,0.1f,50.0f*(f32)actor->mB80);
        gabi::call(0x0200ED84,member<f32>(actor,0xD94),(f32)transformed->y,0.1f,50.0f*(f32)actor->mB80);
        gabi::call(0x0200ED84,member<f32>(actor,0xD98),(f32)transformed->z,0.1f,50.0f*(f32)actor->mB80);
        gabi::call(0x0200ED84,member<f32>(actor,0xDB4),1.0f,1.0f,reg8Float(7)+0.1f);break;
    }
    case 103:
        if(actor->mB56>regShort(2)+8)gabi::call(0x0200ED84,member<f32>(actor,0xDA0),gabi::load<f32>(player+0x318)+regFloat(9),0.8f,regFloat(10)+30.0f);
        if(actor->mB56<=regShort(3)+80)break;
        actor->mB54=150;fopAcM_delete(actor);if(regShort(3)==0)cameraSwitch(actor,actor->mSwitchNo,true);[[fallthrough]];
    case 150:cameraReset(actor,camera);return;
    }
    if(actor->mB54!=0) {
        gabi::Local<cXyz> center,eye;*center.get()=actor->mB68;*eye.get()=actor->mB5C;
        gabi::call(0x02514F88,camera+0x248,center.get(),eye.get(),(f32)actor->mB84,0);
        gabi::call(0x027EC9E8,410,430,0x1000EDB8,(s16)actor->mB56);actor->mB56=(s16)((s16)actor->mB56+1);
    }
}
BOOL daFganon_Execute(fganon_class* actor) {
    WWHD_FUNC(0x02134B28,BOOL,actor);
    executeAnimation(actor);demoCameraAction(actor);energyBallAction(actor);executeCollision(actor);
    return TRUE;
}
VERIFY(0x02134B28,daFganon_Execute);

void pos_move(fganon_class* actor,u8 direct) {
    WWHD_FUNC(0x02134840,void,actor,direct);
    gabi::Local<cXyz> delta;
    if(direct==0) {
        gabi::Local<cXyz> result;
        gabi::call(0x0201ADE0,member<cXyz>(actor,0x5C0),result.get(),member<cXyz>(actor,0x314));
        delta->x=result->x; delta->y=result->y; delta->z=result->z;
        s16 yaw=gabi::call<s16>(0x020195B0,(f32)delta->x,(f32)delta->z);
        f32 z=delta->z,x=delta->x;
        f32 horizontal=gabi::call<f32>(0x028F4384,gabi::fmadds(x,x,z*z));
        s16 pitch=(s16)(-gabi::call<s16>(0x020195B0,(f32)delta->y,horizontal));
        f32 max=read<f32>(actor,0x5D0)*read<f32>(actor,0x5D4);
        s16 scale=(s16)(gabi::load<s16>(0x1047B68E)+5);
        gabi::call(0x0200F428,member<s16>(actor,0x322),yaw,scale,(s16)gabi::ftoi(max));
        max=read<f32>(actor,0x5D0)*read<f32>(actor,0x5D4);
        scale=(s16)(gabi::load<s16>(0x1047B68E)+5);
        gabi::call(0x0200F428,member<s16>(actor,0x320),pitch,scale,(s16)gabi::ftoi(max));
    }
    gabi::call(0x0200ED84,member<f32>(actor,0x5D4),1.0f,1.0f,0.05f);
    delta->y=0.0f; delta->x=0.0f; delta->z=read<f32>(actor,0x370);
    gabi::call(0x025F1884,gabi::load<u32>(0x1018C7B0),read<s16>(actor,0x322));
    gabi::call(0x025F1BF4,gabi::load<u32>(0x1018C7B0),read<s16>(actor,0x320));
    gabi::call(0x0200FCD8,delta.get(),member<cXyz>(actor,0x33C));
    gabi::call(0x028E8D88,member<cXyz>(actor,0x314),member<cXyz>(actor,0x33C),member<cXyz>(actor,0x314));
}
VERIFY(0x02134840,pos_move);

void fly_se_set(fganon_class* actor) {
    WWHD_FUNC(0x021349B8,void,actor);
    gabi::Local<cXyz> delta;
    gabi::call(0x0201ADE0,member<cXyz>(actor,0x314),delta.get(),member<cXyz>(actor,0x300));
    f32 magnitude=gabi::call<f32>(0x028E8DD0,delta.get());
    f32 distance=gabi::call<f32>(0x028F4384,magnitude);
    f32 value=distance*3.5f;
    /* bge is taken on NaN: a NaN volume goes through the high-range path (result 0) */
    u32 volume= !(value<2147483648.0f) ? (u32)gabi::ftoi(value-2147483648.0f)+0x80000000u : (u32)gabi::ftoi(value);
    if(volume>100)volume=100;
    sound(actor,0x303E,volume);
}
VERIFY(0x021349B8,fly_se_set);

void* mahou_se_set(fopAc_ac_c* actor,void* context) {
    WWHD_FUNC(0x02134A90,void*,actor,context);
    if(!gabi::call<s32>(0x025D4604,actor)||!actor)return nullptr;
    u32 p=gabi::ea(actor);
    if(gabi::load<s16>(p+8)!=0xF2)return nullptr;
    if(gabi::load<s8>(p+0x3A1)==0 && p+0x314) {
        s32 reverb=gabi::call<s32>(0x02520540,gabi::load<s8>(p+0x326));
        gabi::call(0x025E1A40,0x6238,gabi::at<cXyz>(p+0x314),0,reverb);
    }
    return actor;
}
VERIFY(0x02134A90,mahou_se_set);

BOOL daFganon_IsDelete(fganon_class* actor) {
    WWHD_FUNC(0x02137808,BOOL,actor);return TRUE;
}
VERIFY(0x02137808,daFganon_IsDelete);

BOOL daFganon_Delete(fganon_class* actor) {
    WWHD_FUNC(0x02137810,BOOL,actor);
    if(read<u8>(actor,0xDBE)!=0) {
        gabi::call(0x025204C8,member<void>(actor,0x3D0),STR(0x1000EF30));
        if(read<u8>(actor,0xDBE)!=1)
            gabi::call(0x025204C8,member<void>(actor,0x3C8),STR(0x1000EF38));
    }
    if(read<u8>(actor,0xDBC)!=0) {
        s8 child=gabi::load<s8>(0x10463D50);
        gabi::store<u8>(0x101B4C98,0);
        gabi::call(0x025F0A18,child);
    }
    gabi::call(0x025E1B34,member<void>(actor,0x614));
    u32 capeId=read<u32>(actor,0x8E8);
    if(capeId!=0xFFFF) {
        gabi::Local<be<u32>> id;*id.get()=capeId;
        void* cape=nullptr;
        if(capeId!=0xFFFFFFFF)cape=gabi::call<void*>(0x025D5218,0x025E1234,id.get());
        gabi::call(0x025D57E0,cape);
    }
    if(read<s8>(actor,0xDBD)!=0)gabi::call(0x025E1928);
    for(u32 offset=0x600;offset<0x610;offset+=4) {
        u32 emitter=read<u32>(actor,offset);
        if(emitter) {
            u32 flags=gabi::load<u32>(emitter+0x254);
            gabi::store<s32>(emitter+0x5C,-1);
            gabi::store<u32>(emitter+0x254,flags|1);
        }
    }
    for(u32 offset=0x5F8;offset<0x600;offset+=4) {
        u32 emitter=read<u32>(actor,offset);
        if(emitter) {
            u32 flags=gabi::load<u32>(emitter+0x254);
            gabi::store<s32>(emitter+0x5C,-1);
            gabi::store<u32>(emitter+0x254,flags|1);
        }
    }
    return TRUE;
}
VERIFY(0x02137810,daFganon_Delete);

fganon_class* actorConstructor(fganon_class* actor) {
    WWHD_FUNC(0x02137D1C,fganon_class*,actor);
    if(!actor) {
        actor=gabi::call<fganon_class*>(0x0273AD10,0xDD0);
        if(!actor)return actor;
    }
    gabi::call(0x025D4ED0,actor);write<u32>(actor,0xB4,0x1000ED84);
    // Three embedded lighting records receive the same typed default members.
    f32 floats[17]={};u8 bytes[4];s16 shorts[4];
    for(u32 i=0;i<6;++i)floats[i]=gabi::load<f32>(0x1016E414+i*4);
    for(u32 i=0;i<4;++i)bytes[i]=gabi::load<u8>(0x1016E42C+i);
    for(u32 i=0;i<4;++i)shorts[i]=gabi::load<s16>(0x1016E430+i*2);
    for(u32 i=9;i<17;++i)floats[i]=gabi::load<f32>(0x1016E414+i*4);
    for(u32 record: {0x3F0u,0x4B0u,0x534u}) {
        for(u32 i=0;i<6;++i)write<f32>(actor,record+i*4,floats[i]);
        for(u32 i=0;i<4;++i)write<u8>(actor,record+0x18+i,bytes[i]);
        for(u32 i=0;i<4;++i)write<s16>(actor,record+0x1C+i*2,shorts[i]);
        for(u32 i=9;i<17;++i)write<f32>(actor,record+i*4,floats[i]);
    }
    gabi::call(0x025166F0,member<void>(actor,0x640));
    gabi::call(0x025166F0,member<void>(actor,0x76C));
    gabi::call(0x024EFE94,member<void>(actor,0x8EC));
    gabi::call(0x024F0474,member<void>(actor,0x92C));
    write<u32>(actor,0x93C,0x1000ED04);write<u32>(actor,0x94C,0x1000ED14);
    write<u32>(actor,0x940,0x1000ED24);write<u8>(actor,0x944,1);
    gabi::call(0x0200BD2C,member<void>(actor,0xAF0));
    gabi::call(0x02515DA0,member<void>(actor,0xB0C));
    write<u32>(actor,0xB08,0x1004AE88);write<u32>(actor,0xB0C,0x1004AEC0);
    gabi::call(0x02515FB8,member<void>(actor,0xB2C));
    write<u32>(actor,0xC40,0x100015A8);write<u32>(actor,0xC3C,0x1000EC14);
    gabi::call(0x02018590,member<void>(actor,0xC44));
    write<u32>(actor,0xB68,0x1004B108);write<u32>(actor,0xC40,0x1004B160);
    write<u32>(actor,0xC58,0x1004B150);
    gabi::call(0x025166F0,member<void>(actor,0xC5C));
    return actor;
}
VERIFY(0x02137D1C,actorConstructor);

static u32 newAnimation(u32 size,u32 constructor) {
    u32 p=gabi::call<u32>(0x0273AD10,size);
    if(p)p=gabi::call<u32>(constructor,p);
    return p;
}
static BOOL initializeAnimation(fganon_class* actor,u32 offset,u32 modelData,u32 resource,s32 index,u32 target,s32 loop) {
    void* resourceData=dComIfG_getObjectRes(STR(resource),index,0x1000EBFC);
    return gabi::call<BOOL>(target,read<u32>(actor,offset),modelData,resourceData,1,loop,1.0f,0,-1,0,0);
}
BOOL useHeapInit(fopAc_ac_c* base) {
    WWHD_FUNC(0x02137954,BOOL,base);
    fganon_class* actor=(fganon_class*)base;
    void* modelData=dComIfG_getObjectRes(STR(0x1000EF40),0x1A,0x1000EBFC);
    void* animation=dComIfG_getObjectRes(STR(0x1000EF40),0x16,0x1000EBFC);
    u32 morf=gabi::call<u32>(0x025E4F64,0,modelData,0,0,animation,2,1.0f,0,-1,1,0,0,0x11020203);
    write<u32>(actor,0x3DC,morf);
    if(!morf||!gabi::load<u32>(morf+0x90))return FALSE;
    u32 model=gabi::load<u32>(morf+0x90);
    u32 data=gabi::load<u32>(model+0xAC);
    write<u32>(gabi::at<fganon_class>(model),0xB8,gabi::ea(actor));
    u32 joints=gabi::call<u32>(0x027F3F94,data);
    u16 count=gabi::load<u16>(joints+8);
    for(u16 i=0;i<count;i=(u16)(i+1)) {
        if((i>=2&&i<=4)||(i>=6&&i<=7)) {
            data=gabi::load<u32>(model+0xAC);
            u32 actualCount=gabi::load<u32>(data+4);
            u32 joint=gabi::load<u32>(data+8);
            if(i<actualCount)joint+=i*0x1C;
            gabi::store<u32>(joint+8,0x02134230);
        }
        joints=gabi::call<u32>(0x027F3F94,gabi::load<u32>(model+0xAC));
        count=gabi::load<u16>(joints+8);
    }
    u32 brk=newAnimation(0x78,0x025E80D0);write<u32>(actor,0x3E0,brk);
    if(!brk)return FALSE;
    model=morfModel(actor);
    void* resource=dComIfG_getObjectRes(STR(0x1000EF40),0x21,0x1000EBFC);
    if(!gabi::call<BOOL>(0x025E8154,read<u32>(actor,0x3E0),gabi::load<u32>(model+0xAC),resource,1,0,1.0f,0,-1,0,0))return FALSE;
    resource=dComIfG_getObjectRes(STR(0x1000EF48),4,0x1000EBFC);
    u32 sword=gabi::call<u32>(0x025E38E0,resource,0,0x11020203);
    write<u32>(actor,0x3E4,sword);if(!sword)return FALSE;
    data=gabi::load<u32>(sword+0xAC);
    brk=newAnimation(0x78,0x025E80D0);write<u32>(actor,0x3E8,brk);
    if(!brk||!initializeAnimation(actor,0x3E8,data,0x1000EF40,0x1F,0x025E8154,0))return FALSE;
    resource=dComIfG_getObjectRes(STR(0x1000EF40),0x1B,0x1000EBFC);
    data=gabi::ea(resource);
    model=gabi::call<u32>(0x025E38E0,resource,0,0x11020203);
    write<u32>(actor,0x8A8,model);if(!model)return FALSE;
    u32 btk=newAnimation(0x74,0x025E7C6C);write<u32>(actor,0x8AC,btk);
    if(!btk||!initializeAnimation(actor,0x8AC,data,0x1000EF40,0x2A,0x025E7CE0,2))return FALSE;
    brk=newAnimation(0x78,0x025E80D0);write<u32>(actor,0x8B0,brk);
    if(!brk||!initializeAnimation(actor,0x8B0,data,0x1000EF40,0x26,0x025E8154,0))return FALSE;
    return TRUE;
}
VERIFY(0x02137954,useHeapInit);

cPhs_State daFganon_Create(fopAc_ac_c* base) {
    WWHD_FUNC(0x02137F9C,cPhs_State,base);
    fganon_class* actor=(fganon_class*)base;
    u32 condition=read<u32>(actor,0x2E4);
    if(!(condition&8)) {
        if(actor) {actorConstructor(actor);condition=read<u32>(actor,0x2E4);}
        write<u32>(actor,0x2E4,condition|8);
    }
    u8 switchNo;
    if(read<u8>(actor,0xC)==0) {
        switchNo=(u8)(read<u32>(actor,0xB0)>>16);write<u8>(actor,0x3DA,switchNo);
    } else switchNo=read<u8>(actor,0x3DA);
    if(switchNo!=0xFF) {
        u32 save=gabi::load<u32>(0x101F84DC);
        s8 room=gabi::load<s8>(0x1047E6C8);
        if(gabi::call<BOOL>(0x025BA0C0,save+0x20,switchNo,room)) {
            if((read<u32>(actor,0xB0)&15)==2 && !gabi::call<BOOL>(0x025B8B94,gabi::load<u32>(0x101F84DC)+0x644,0x3A08))
                gabi::call(0x025D5834,0x1CF,5,member<cXyz>(actor,0x314),read<s8>(actor,0x326),0,0,-1,0);
            return cPhs_ERROR_e;
        }
    }
    write<u8>(actor,0xDBE,1);
    cPhs_State result=gabi::call<cPhs_State>(0x02520460,member<void>(actor,0x3D0),STR(0x1000EF50));
    if(result!=cPhs_COMPLEATE_e)return result;
    write<u8>(actor,0xDBE,2);
    result=gabi::call<cPhs_State>(0x02520460,member<void>(actor,0x3C8),STR(0x1000EF58));
    if(result!=cPhs_COMPLEATE_e)return result;
    u32 params=read<u32>(actor,0xB0);
    write<u8>(actor,0x3D8,params&15);write<u8>(actor,0x3D9,(u8)(params>>8));
    write<u8>(actor,0x3DA,(u8)(params>>16));write<u8>(actor,0x3DB,(u8)(params>>24));
    if((params&15)==3)write<u8>(actor,0x8C3,(u8)((read<u32>(actor,0xB0)>>4)&15));
    if(!gabi::call<BOOL>(0x025D63E8,actor,0x02137954,0x96000))return cPhs_ERROR_e;
    write<u32>(actor,0x39C,4);write<u8>(actor,0x38A,4);
    if(gabi::load<u8>(0x101B4C98)==0) {
        write<u8>(actor,0xDBC,1);gabi::store<u8>(0x101B4C98,1);
        u8 variant=read<u8>(actor,0x3D8);
        u32 label=variant==2?0x1000EF60:variant==1?0x1000EF74:0x1000EF88;
        s8 child=gabi::call<s8>(0x025F0A10,STR(label),0x10463D50);
        gabi::store<s8>(0x10463D50,child);
    }
    gabi::call(0x024F06B4,member<void>(actor,0x92C),member<cXyz>(actor,0x314),member<cXyz>(actor,0x300),actor,1,member<void>(actor,0x8EC),member<cXyz>(actor,0x33C),0,0);
    write<u8>(actor,0x938,0);
    gabi::call(0x024EFF44,member<void>(actor,0x8EC),200.0f,200.0f);
    u32 status=gabi::ea(actor)+0xAF0;
    gabi::call(0x02515F14,status,0xFA,0xFF,actor);
    gabi::call(0x02516518,member<void>(actor,0xB2C),0x101B4D88);
    u32 flags=read<u32>(actor,0xBC0);
    write<u32>(actor,0xB70,status);write<u32>(actor,0xBC0,flags|4);
    gabi::call(0x0251677C,member<void>(actor,0x640),0x101B4D08);
    flags=read<u32>(actor,0x6D4);
    write<u32>(actor,0x684,status);write<u32>(actor,0x6D4,flags|4);
    gabi::call(0x0251677C,member<void>(actor,0x76C),0x101B4D48);
    write<u32>(actor,0x7B0,status);
    gabi::call(0x0251677C,member<void>(actor,0xC5C),0x101B4CC8);
    u8 copy=read<u8>(actor,0x8C3);write<u32>(actor,0xCA0,status);
    if(copy==0) {
        gabi::store<u32>(0x10463D30,gabi::ea(actor));
        if(read<u8>(actor,0x3D8)==0) {
            write<s16>(actor,0x5BC,0);write<s16>(actor,0x5BA,20);kieru_brk(actor,0);
            f32 y=read<f32>(actor,0x318);
            write<u8>(actor,0x3A1,30);write<u8>(actor,0x3A0,30);write<s8>(actor,0x8C4,10);
            write<f32>(actor,0x318,y+10000);
        } else {
            f32 y=read<f32>(actor,0x318);
            write<u8>(actor,0x3A1,100);write<u8>(actor,0x3A0,100);
            write<s16>(actor,0x5BC,-1);write<s16>(actor,0x5BA,0);write<f32>(actor,0x318,y+10000);
        }
    } else {
        write<s16>(actor,0x5BC,0);write<s16>(actor,0x5BA,10);deru_brk(actor);
    }
    f32 gravity=gabi::load<f32>(0x1047BAB0)+300;
    s8 room=read<s8>(actor,0x326);
    write<f32>(actor,0x3B0,300);write<f32>(actor,0x3AC,gravity);
    u32 cape=gabi::call<u32>(0x025D5834,0xC0,1,member<cXyz>(actor,0x314),room,0,0,-1,0);
    write<u32>(actor,0x8E8,cape);gabi::call(0x02134B28,actor);
    return result;
}
VERIFY(0x02137F9C,daFganon_Create);

void* HIOConstructor(void* h) {
    WWHD_FUNC(0x0213846C,void*,h);
    if(!h) {h=gabi::call<void*>(0x0273AD10,0x4C);if(!h)return h;}
    u32 p=gabi::ea(h);
    // HD places the vtable after the data, so these member offsets differ from GameCube.
    gabi::store<s8>(p,-1);gabi::store<u8>(p+1,0);gabi::store<u8>(p+2,0);
    gabi::store<s16>(p+4,60);gabi::store<s16>(p+6,500);
    gabi::store<f32>(p+8,1500);gabi::store<f32>(p+0xC,400);
    gabi::store<f32>(p+0x10,1500);gabi::store<f32>(p+0x14,400);
    gabi::store<f32>(p+0x18,45);gabi::store<f32>(p+0x1C,45);
    gabi::store<f32>(p+0x20,2);gabi::store<f32>(p+0x24,2);
    gabi::store<f32>(p+0x28,30);gabi::store<f32>(p+0x2C,-270);
    gabi::store<f32>(p+0x30,-600);gabi::store<f32>(p+0x34,270);
    gabi::store<s16>(p+0x38,50);gabi::store<s16>(p+0x3A,1);gabi::store<s16>(p+0x3C,32);
    gabi::store<f32>(p+0x40,600);gabi::store<s16>(p+0x44,150);
    gabi::store<u32>(p+0x48,0x1000ED74);
    return h;
}
VERIFY(0x0213846C,HIOConstructor);

void staticInitializer() {
    WWHD_FUNC(0x0213856C,void,(u32)0);
    gabi::store<u32>(0x10463D48,0);gabi::store<u32>(0x10463D40,0);
    gabi::store<u32>(0x10463D4C,0);gabi::store<u32>(0x10463D44,0);
    gabi::call(0x028F026C,0x101B4DCC);
    gabi::store<f32>(0x10463D34,-3.1415927410125732f);
    gabi::store<f32>(0x10463D38,3.1415927410125732f);
    gabi::call(0x028ED6F8,0x10463D3C);
    gabi::call(0x028F026C,0x101B4DD8);
    gabi::call(0x028EAB2C,0x10463D3D);
    gabi::call(0x028F026C,0x101B4DE4);
    HIOConstructor(gabi::at<void>(0x10463D50));
}
VERIFY(0x0213856C,staticInitializer);

void* findActor(u32 id) {
    WWHD_FUNC(0x0213860C,void*,id);
    if(id==0xFFFFFFFF)return nullptr;
    gabi::Local<be<u32>> local;*local.get()=id;
    return gabi::call<void*>(0x025D5218,0x025E1234,local.get());
}
VERIFY(0x0213860C,findActor);

void trivialDestructor(void* p,s32 flags) {
    WWHD_FUNC(0x02138648,void,p,flags);
    if(p&&(flags&1))gabi::call(0x0273AF40,p);
}
VERIFY(0x02138648,trivialDestructor);

void emitterSetFlags(void* p,u32 flags) {
    WWHD_FUNC(0x0213865C,void,p,flags);
    u32 address=gabi::ea(p)+0x254;
    gabi::store<u32>(address,gabi::load<u32>(address)|flags);
}
VERIFY(0x0213865C,emitterSetFlags);

void emptyVirtual(void* p) {WWHD_FUNC(0x0213866C,void,p);}
VERIFY(0x0213866C,emptyVirtual);

void actorDestructor(fganon_class* actor,s32 flags) {
    WWHD_FUNC(0x0213D41C,void,actor,flags);
    if(!actor)return;
    gabi::call(0x02515AE8,member<void>(actor,0xC5C),2);
    gabi::call(0x02515A70,member<void>(actor,0xB2C),2);
    gabi::call(0x02515860,member<void>(actor,0xAF0),2);
    write<u32>(actor,0x94C,0x1000ED14);write<u32>(actor,0x940,0x1000ED24);
    gabi::call(0x024EFD9C,member<void>(actor,0x92C),0);
    gabi::call(0x02018034,member<void>(actor,0x900),2);
    gabi::call(0x02515AE8,member<void>(actor,0x76C),2);
    gabi::call(0x02515AE8,member<void>(actor,0x640),2);
    gabi::call(0x025D50BC,actor,0);
    if(flags&1)gabi::call(0x0273AF40,actor);
}
VERIFY(0x0213D41C,actorDestructor);
void actorEmptyVirtual(void* p) {WWHD_FUNC(0x0213D4DC,void,p);}
VERIFY(0x0213D4DC,actorEmptyVirtual);
