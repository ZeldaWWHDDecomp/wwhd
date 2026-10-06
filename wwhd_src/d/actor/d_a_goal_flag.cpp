/** Finish-line flag, cloth simulation and boat-race controller (WWHD).
 * See wwhd_src/README.md. */
#include "d/actor/d_a_goal_flag.h"
namespace {
constexpr u32 RopePaths=offsetof(GoalFlagActorLayout,ropePaths), BuoyCounts=offsetof(GoalFlagActorLayout,buoyCounts), RopeCount=offsetof(GoalFlagActorLayout,ropeCount), RopeLines=offsetof(GoalFlagActorLayout,ropeLines);
constexpr u32 Packet=offsetof(GoalFlagActorLayout,packet), TimerId=offsetof(GoalFlagActorLayout,timerId), RaceState=offsetof(GoalFlagActorLayout,raceEndState), CameraFrames=offsetof(GoalFlagActorLayout,cameraFrames), StartState=offsetof(GoalFlagActorLayout,raceStartState), PreviousSide=offsetof(GoalFlagActorLayout,previousSide), TimerEnded=offsetof(GoalFlagActorLayout,timerEnded);
constexpr u32 ActionAdjustment=offsetof(GoalFlagActorLayout,actionAdjustment), ActionDispatch=offsetof(GoalFlagActorLayout,actionDispatch), ActionTarget=offsetof(GoalFlagActorLayout,actionTarget);
template<class T> T read(void* p,u32 off) { return gabi::load<T>(gabi::ea(p)+off); }
template<class T> void write(void* p,u32 off,T value) { gabi::store<T>(gabi::ea(p)+off,value); }
void* member(void* p,u32 off) { return gabi::at<void>(gabi::ea(p)+off); }
}
static BOOL Goal_CreateHeap(void* actor) {
    WWHD_FUNC(0x0215D304,BOOL,actor);
    for(s32 rope=0;rope<read<s32>(actor,RopeCount);++rope) {
        u16 points=(u16)(read<u32>(actor,BuoyCounts+4*rope)*4+1);
        if(!gabi::call<s32>(0x025E9B80,member(actor,RopeLines+0x148*rope),1,points,0)) return FALSE;
    }
    return TRUE;
}
VERIFY(0x0215D304,Goal_CreateHeap);
static BOOL Goal_checkCreateHeap(void* actor) {
    WWHD_FUNC(0x0215D3C0,BOOL,actor);
    return Goal_CreateHeap(actor);
}
VERIFY(0x0215D3C0,Goal_checkCreateHeap);
static BOOL Goal_getRacePath(void* actor,u8 pathId) {
    WWHD_FUNC(0x0215DD94,BOOL,actor,pathId);
    void* path=gabi::call<void*>(0x025AAF88,pathId,read<s8>(actor,0x326));
    write<u32>(actor,RopePaths,gabi::ea(path));
    if(!path) return FALSE;
    s32 rope=0;
    do {
        write<u32>(actor,BuoyCounts+4*rope,read<u16>(path,0));
        ++rope;
        pathId=read<u8>(gabi::at<void>(read<u32>(actor,RopePaths+4*(rope-1))),3);
        if(rope>=4 || pathId==0xFF) break;
        path=gabi::call<void*>(0x025AAF88,pathId,read<s8>(actor,0x326));
        write<u32>(actor,RopePaths+4*rope,gabi::ea(path));
    } while(true);
    write<s32>(actor,RopeCount,rope);
    return TRUE;
}
VERIFY(0x0215DD94,Goal_getRacePath);
static void Goal_copyMatrix(void* destination,void* source) {
    WWHD_FUNC(0x0215F7CC,void,destination,source);
    f32 values[12];
    for(u32 i=0;i<12;++i) values[i]=read<f32>(source,4*i);
    for(u32 i=0;i<12;++i) write<f32>(destination,4*i,values[i]);
}
VERIFY(0x0215F7CC,Goal_copyMatrix);
static void Goal_colorS10ToFloat(void* destination,void* source) {
    WWHD_FUNC(0x0215F86C,void,destination,source);
    f32 values[4];
    for(u32 i=0;i<4;++i) values[i]=(f32)read<s16>(source,2*i)/255.0f;
    for(u32 i=0;i<4;++i) write<f32>(destination,4*i,values[i]);
}
VERIFY(0x0215F86C,Goal_colorS10ToFloat);
static void Goal_colorToFloat(void* destination,void* source) {
    WWHD_FUNC(0x0215F930,void,destination,source);
    f32 values[4];
    for(u32 i=0;i<4;++i) values[i]=(f32)read<u8>(source,i)/255.0f;
    for(u32 i=0;i<4;++i) write<f32>(destination,4*i,values[i]);
}
VERIFY(0x0215F930,Goal_colorToFloat);
static void* Goal_vertexBufferCtor(void* buffer) {
    WWHD_FUNC(0x02160F24,void*,buffer);
    if(!buffer) buffer=gabi::call<void*>(0x0273AD10,0x254);
    if(buffer) {
        gabi::call(0x027B5BD8,member(buffer,4));
        gabi::call(0x027BF734,member(buffer,0x158));
        write<u32>(buffer,0x250,0);write<u32>(buffer,0x24C,0);
    }
    return buffer;
}
VERIFY(0x02160F24,Goal_vertexBufferCtor);
static void* Goal_bufferEntryCtor(void* entry) {
    WWHD_FUNC(0x02160F80,void*,entry);
    return entry?entry:gabi::call<void*>(0x0273AD10,0x10);
}
VERIFY(0x02160F80,Goal_bufferEntryCtor);
static void Goal_bufferEntryDtor(void* entry,s32 flags) {
    WWHD_FUNC(0x02160FAC,void,entry,flags);
    if(entry && (flags&1)) gabi::call(0x0273AF40,entry);
}
VERIFY(0x02160FAC,Goal_bufferEntryDtor);
static BOOL Goal_IsDelete(void* actor) {
    WWHD_FUNC(0x02160FC0,BOOL,actor);
    return TRUE;
}
VERIFY(0x02160FC0,Goal_IsDelete);
static void Goal_vertexBufferDtor(void* buffer,s32 flags) {
    WWHD_FUNC(0x02160FC8,void,buffer,flags);
    if(buffer) {
        gabi::call(0x027BF880,member(buffer,0x158),2);
        gabi::call(0x027B5CBC,member(buffer,4),2);
        if(flags&1) gabi::call(0x0273AF40,buffer);
    }
}
VERIFY(0x02160FC8,Goal_vertexBufferDtor);
static void Goal_HIODtor(void* hio,s32 flags) {
    WWHD_FUNC(0x021618A4,void,hio,flags);
    if(hio) {
        write<u8>(hio,4,0xFF);write<u32>(hio,0,0x1001037C);
        if(flags&1) gabi::call(0x0273AF40,hio);
    }
}
VERIFY(0x021618A4,Goal_HIODtor);
static void Goal_HIOCallback(void* hio) {
    WWHD_FUNC(0x021618CC,void,hio);
}
VERIFY(0x021618CC,Goal_HIOCallback);
static s32 Goal_getDemoAction(void* actor,s32 staff) {
    WWHD_FUNC(0x02160788,s32,actor,staff);
    u32 play=gabi::call<u32>(0x025200D4);
    return gabi::call<s32>(0x02542EDC,gabi::at<void>(play+0x52C4),staff,gabi::at<void>(0x101B5CF8),5,0,0);
}
VERIFY(0x02160788,Goal_getDemoAction);
static BOOL Goal_Execute(void* actor) {
    WWHD_FUNC(0x0215F748,BOOL,actor);
    s16 dispatch=read<s16>(actor,ActionDispatch);
    if(dispatch) {
        void* adjusted=member(actor,read<s16>(actor,ActionAdjustment));
        u32 target;
        if(dispatch<0) target=read<u32>(actor,ActionTarget);
        else {
            u32 table=read<u32>(adjusted,read<s16>(actor,0x2FB2));
            target=gabi::load<u32>(table+(u32)dispatch*8+4);
        }
        gabi::call(target,adjusted);
    }
    gabi::call(0x0215EA00,actor);
    gabi::call(0x0215F4D0,actor);
    return TRUE;
}
VERIFY(0x0215F748,Goal_Execute);
static BOOL Goal_CreateBuoyRaces(void* actor) {
    WWHD_FUNC(0x0215DE4C,BOOL,actor);
    gabi::Local<cXyz> position;
    for(s32 rope=0;rope<read<s32>(actor,RopeCount);++rope) {
        void* path=gabi::at<void>(read<u32>(actor,RopePaths+rope*4));
        void* points=gabi::at<void>(read<u32>(path,8));
        void* line=member(actor,RopeLines+rope*0x148);
        void* segment=gabi::at<void>(gabi::load<u32>(read<u32>(line,0x144)));
        s32 buoy=0;
        for(;buoy<read<s32>(actor,BuoyCounts+rope*4);++buoy) {
            position->x=read<f32>(points,4);position->y=read<f32>(points,8);position->z=read<f32>(points,12);
            gabi::call<s32>(0x025D5A20,0x10C,read<u32>(actor,4),buoy|(rope<<8),position.get(),read<s8>(actor,0x326),0,0,-1,0);
            f32 x=read<f32>(points,4),y=read<f32>(points,8)+250.0f,z=read<f32>(points,12);
            write<f32>(segment,0,x);write<f32>(segment,8,z);write<f32>(segment,4,y);
            segment=member(segment,12);points=member(points,16);
        }
        void* first=gabi::at<void>(gabi::load<u32>(read<u32>(line,0x144)));
        for(u32 component=0;component<3;++component) write<u32>(segment,component*4,read<u32>(first,component*4));
    }
    return TRUE;
}
VERIFY(0x0215DE4C,Goal_CreateBuoyRaces);
static void Goal_clothSpring(cXyz* position,cXyz* neighbor,cXyz* correction,f32 idealDistance) {
    WWHD_FUNC(0x0215D26C,void,position,neighbor,correction,idealDistance);
    gabi::Local<cXyz> delta,direction;
    gabi::call(0x0201ADE0,neighbor,delta.get(),position);
    gabi::call(0x0201B12C,delta.get(),direction.get());
    f32 square=gabi::call<f32>(0x028E8DD0,delta.get());
    f32 length=gabi::call<f32>(0x028F4384,square);
    f32 stiffness=gabi::load<f32>(0x104641A0);
    gabi::call(0x028E8E64,direction.get(),direction.get(),(length-idealDistance)*stiffness);
    gabi::call(0x028E8D88,correction,direction.get(),correction);
}
VERIFY(0x0215D26C,Goal_clothSpring);
static void Goal_setBackNrm(void* packet) {
    WWHD_FUNC(0x0215E964,void,packet);
    u32 array=(u32)read<u8>(packet,0x2648)*0x21C;
    void* front=member(packet,0x1BBC+array);
    void* back=member(packet,0x1FF4+array);
    for(s32 i=0;i<45;++i) {
        for(u32 component=0;component<3;++component) write<f32>(back,component*4,0.0f);
        gabi::call(0x028E8DAC,back,front,back);
        back=member(back,12);front=member(front,12);
    }
}
VERIFY(0x0215E964,Goal_setBackNrm);
static s32 Goal_goalCheck(void* actor) {
    WWHD_FUNC(0x02160618,s32,actor);
    gabi::Local<cXyz> playerOffset,line,normalized,rawLine,horizontal,normal;
    u32 play=gabi::call<u32>(0x025200D4);
    void* player=gabi::at<void>(gabi::load<u32>(play+0x5B2C));
    gabi::call(0x0201ADE0,member(player,0x314),playerOffset.get(),member(actor,0x2A38));
    gabi::call(0x0201ADE0,member(actor,0x2A44),line.get(),member(actor,0x2A38));
    line->y=0.0f;
    gabi::call(0x0201B12C,line.get(),normalized.get());
    line->copy(*normalized);
    playerOffset->y=0.0f;
    gabi::call(0x0201ADE0,member(actor,0x2A44),rawLine.get(),member(actor,0x2A38));
    horizontal->set(rawLine->x,0.0f,rawLine->z);
    f32 square=gabi::call<f32>(0x028E8DD0,horizontal.get());
    f32 length=gabi::call<f32>(0x028F4384,square);
    f32 along=gabi::call<f32>(0x028E8F44,line.get(),playerOffset.get());
    normal->set(line->z,0.0f,-(f32)line->x);
    f32 side=gabi::call<f32>(0x028E8F44,normal.get(),playerOffset.get());
    s32 result=0;
    if(along>0.0f && along<length) {
        f32 previous=read<f32>(actor,PreviousSide);
        if(previous>0.0f) { if(!(side>0.0f)) result=1; }
        else if(side>0.0f) result=-1;
    }
    write<f32>(actor,PreviousSide,side);
    return result;
}
VERIFY(0x02160618,Goal_goalCheck);
static void* Goal_eventManager() { return gabi::at<void>(gabi::call<u32>(0x025200D4)+0x52C4); }
static void Goal_cutEnd(s32 staff) { gabi::call(0x02543280,Goal_eventManager(),staff); }
static BOOL Goal_RaceStart(void* actor) {
    WWHD_FUNC(0x021607D4,BOOL,actor);
    s32 staff=gabi::call<s32>(0x02542D88,Goal_eventManager(),gabi::at<void>(0x10010564),0,0);
    s32 action=Goal_getDemoAction(actor,staff);
    if(!read<u8>(actor,StartState)) {
        if(staff==-1 || action!=0) return TRUE;
        Goal_cutEnd(staff);
        write<u8>(actor,StartState,read<u8>(actor,StartState)+1);
        return TRUE;
    }
    void* timer=gabi::call<void*>(0x025DE50C,read<u32>(actor,TimerId));
    u32 singleton=gabi::load<u32>(0x101F8344);
    void* starter=gabi::at<void>(gabi::load<u32>(singleton+0x204));
    gabi::call<u32>(0x025200D4); // HD retains this accessor before the starter action.
    if(action==1 && starter) {
        gabi::call(0x020063C0,member(starter,0x18),gabi::at<void>(0x1048F27C));
        Goal_cutEnd(staff);
    }
    if(timer && starter && read<u8>(starter,0x51)) {
        Goal_cutEnd(staff);gabi::call(0x025C6250,timer,7);
        if(read<u8>(actor,StartState)==1) {
            gabi::call(0x025E18EC,0x8000000Bu);
            write<u8>(actor,StartState,read<u8>(actor,StartState)+1);
        }
    }
    if(gabi::call<s32>(0x0254457C,Goal_eventManager(),gabi::at<void>(0x1001056C))) {
        write<s16>(actor,ActionDispatch,-1);write<u32>(actor,ActionTarget,0x02160954);
        write<u8>(actor,StartState,0);write<s16>(actor,ActionAdjustment,0);
    }
    return TRUE;
}
VERIFY(0x021607D4,Goal_RaceStart);
static BOOL Goal_RaceEnd(void* actor) {
    WWHD_FUNC(0x02160C50,BOOL,actor);
    s32 id=read<s32>(actor,TimerId);
    write<s16>(actor,CameraFrames,(s16)(read<s16>(actor,CameraFrames)+1));
    if(id!=-1) {
        void* timer=gabi::call<void*>(0x025DE50C,id);
        if(timer && gabi::call<s32>(0x025C6278,timer)) {
            gabi::call(0x025DF944,timer);write<s32>(actor,TimerId,-1);
        }
    }
    s32 staff=gabi::call<s32>(0x02542D88,Goal_eventManager(),gabi::at<void>(0x100105C4),0,0);
    if(staff!=-1) Goal_cutEnd(staff);
    u32 name=gabi::load<u32>(0x101B5D14+(read<s16>(actor,RaceState)!=3?4:0));
    bool end=gabi::call<s32>(0x0254457C,Goal_eventManager(),gabi::at<void>(name))!=0;
    if(!end && read<s16>(actor,CameraFrames)>gabi::load<s32>(0x10464184)) {
        end=gabi::call<s32>(0x02007898,0)!=0;
        if(!end) end=gabi::call<s32>(0x020078BC,0)!=0;
        if(!end) end=gabi::call<s32>(0x02007940,0)!=0;
    }
    if(end) gabi::call(0x0252012C,gabi::at<void>(0x100105C0),1,0x30,-1,0,1,0,0.0f);
    return TRUE;
}
VERIFY(0x02160C50,Goal_RaceEnd);
static BOOL Goal_Delete(void* actor) {
    WWHD_FUNC(0x0215F3B4,BOOL,actor);
    u32 parameter=read<u32>(actor,0xB0);
    gabi::call(0x025204C8,member(actor,0x29F8),gabi::at<void>(0x100102FC));
    u32 name=gabi::load<u32>(0x101B594C+((parameter&1)*4));
    gabi::call(0x025204C8,member(actor,0x2A00),gabi::at<void>(name));
    u32 play=gabi::call<u32>(0x025200D4);
    if(gabi::load<u8>(play+0x5CEA)==1) {
        play=gabi::call<u32>(0x025200D4);
        if(gabi::load<u8>(play+0x5CEE)==1) {
            gabi::store<u32>(0x101D5F1C,2);
            play=gabi::call<u32>(0x025200D4);
            gabi::store<s32>(0x101D5F18,gabi::load<s16>(play+0x5CEC));
        } else {
            play=gabi::call<u32>(0x025200D4);
            if(gabi::load<u8>(play+0x5CEE)==2) {
                gabi::store<u32>(0x101D5F1C,1);
                play=gabi::call<u32>(0x025200D4);
                gabi::store<s32>(0x101D5F18,gabi::load<s16>(play+0x5CEC));
            }
        }
        play=gabi::call<u32>(0x025200D4);
        u16 state=gabi::load<u16>(play+0x5CE8);
        gabi::store<u8>(play+0x5CEE,0);gabi::store<u8>(play+0x5CEA,0);gabi::store<u16>(play+0x5CE8,state^1);
    }
    u32 singleton=gabi::load<u32>(0x101F8344);
    void* terminater=gabi::at<void>(gabi::load<u32>(singleton+0x200));
    if(terminater) {
        write<u8>(terminater,0x64,0);
        gabi::call(0x020063C0,member(terminater,0x18),gabi::at<void>(0x1049DD60));
        singleton=gabi::load<u32>(0x101F8344);
    }
    void* another=gabi::at<void>(gabi::load<u32>(singleton+0x1FC));
    if(another) gabi::call(0x026266C8,another);
    return TRUE;
}
VERIFY(0x0215F3B4,Goal_Delete);
static void Goal_clothFactor(void* actor,cXyz* output,cXyz* positions,cXyz* normals,cXyz* wind,s32 column,s32 row) {
    WWHD_FUNC(0x0215E154,void,actor,output,positions,normals,wind,column,row);
    gabi::Local<cXyz> center,scaled,correction;
    s32 index=row*9+column;
    center->set(positions[index].x,positions[index].y,positions[index].z);
    f32 pressure=gabi::call<f32>(0x028E8F44,wind,&normals[index]);
    if((row==0 || row==4) && (column==0 || column==8)) {
        if(!output) output=gabi::call<cXyz*>(0x0273AD10,12);
        if(output) for(u32 i=0;i<3;++i) write<f32>(output,i*4,gabi::load<f32>(0x101FFBA8+i*4));
        return;
    }
    gabi::call(0x0201AE48,&normals[index],scaled.get(),pressure);
    correction->x=scaled->x;correction->z=scaled->z;
    correction->y=gabi::fmadds(gabi::load<f32>(0x1046419C),0.25f*(f32)row,(f32)scaled->y);
    auto spring=[&](s32 neighbor,f32 distance) { Goal_clothSpring(center.get(),&positions[neighbor],correction.get(),distance); };
    if(column!=0) {
        spring(index-1,250.0f);
        if(row!=0) { spring(index-9,120.0f);spring(index-10,277.3085021972656f); }
        if(row!=4) { spring(index+9,120.0f);spring(index+8,277.3085021972656f); }
        if(column!=8) {
            spring(index+1,250.0f);
            if(row!=0) spring(index-8,277.3085021972656f);
            if(row!=4) spring(index+10,277.3085021972656f);
        }
    } else {
        spring(index+1,250.0f);
        if(row!=0) {spring(index-9,120.0f);spring(index-8,277.3085021972656f);}
        if(row!=4) {spring(index+9,120.0f);spring(index+10,277.3085021972656f);}
    }
    if(!output) output=gabi::call<cXyz*>(0x0273AD10,12);
    if(output) output->set(correction->x,correction->y,correction->z);
    return;
}
VERIFY(0x0215E154,Goal_clothFactor);
static void Goal_assertRopeIndex(u32 rope) {
    if(rope>=4) gabi::call(0x0273AA24,gabi::at<void>(0x100104B0),0x9B,gabi::at<void>(0x100104D0));
}
static void* Goal_ropePoint(void* line,s32 anchor) {
    return gabi::at<void>(gabi::load<u32>(read<u32>(line,0x144))+(u32)anchor*0x30);
}
static void Goal_RopeMove(void* actor) {
    WWHD_FUNC(0x0215F4D0,void,actor);
    gabi::Local<cXyz> from,to,sum;
    for(s32 rope=0;rope<read<s32>(actor,RopeCount);++rope) {
        s32 count=read<s32>(actor,BuoyCounts+rope*4);
        Goal_assertRopeIndex(rope);
        void* line=member(actor,RopeLines+rope*0x148);
        void* end=Goal_ropePoint(line,count);
        Goal_assertRopeIndex(rope);
        void* first=Goal_ropePoint(line,0);
        for(u32 i=0;i<3;++i) write<u32>(end,i*4,read<u32>(first,i*4));
        for(s32 segment=0;segment<(s32)(read<u32>(actor,BuoyCounts+rope*4)*4);++segment) {
            s32 subdivision=segment&3,anchor=segment>>2;
            if(subdivision) {
                Goal_assertRopeIndex(rope);
                void* a=Goal_ropePoint(line,anchor);
                Goal_assertRopeIndex(rope);
                void* b=Goal_ropePoint(line,anchor+1);
                void* destination=member(a,subdivision*12);
                f32 t=(f32)subdivision*0.25f;
                gabi::call(0x0201AE48,a,from.get(),1.0f-t);
                gabi::call(0x0201AE48,b,to.get(),t);
                gabi::call(0x0201AD78,from.get(),sum.get(),to.get());
                // lfs/stfs copy the bits (signalling NaNs stay signalling); only the sag subtraction rounds.
                write<u32>(destination,0,read<u32>(sum.get(),0));
                u32 y=read<u32>(sum.get(),4);write<u32>(destination,4,y);
                write<u32>(destination,8,read<u32>(sum.get(),8));
                write<f32>(destination,4,gabi::fsubs_ppc(gabi::f32_from_bits(y),gabi::load<f32>(0x100104C0+subdivision*4)));
            }
        }
    }
}
VERIFY(0x0215F4D0,Goal_RopeMove);
static BOOL Goal_TimerExecute(void* actor) {
    WWHD_FUNC(0x02160954,BOOL,actor);
    void* timer=gabi::call<void*>(0x025DE50C,read<u32>(actor,TimerId));
    if(!timer) gabi::call(0x0273AA24,gabi::at<void>(0x1001057C),0x59B,gabi::at<void>(0x10010590));
    s32 crossing=Goal_goalCheck(actor);
    if(crossing==1) {
        if(read<s16>(actor,RaceState)==1) {
            if(!read<u8>(actor,TimerEnded)) {
                write<u8>(actor,TimerEnded,1);gabi::call(0x025C58A8,timer,-1);
                u32 play=gabi::call<u32>(0x025200D4);
                write<s16>(actor,RaceState,gabi::load<s16>(play+0x5CEC)==0?2:3);
            }
        } else write<s16>(actor,RaceState,1);
    } else if(crossing==-1 && !read<u8>(actor,TimerEnded)) {
        write<u8>(actor,TimerEnded,1);gabi::call(0x025C58A8,timer,-1);
        gabi::call(0x025E1988,0x847);write<s16>(actor,RaceState,2);
    }
    if(gabi::call<s32>(0x025C5814,timer)<=0 && !read<u8>(actor,TimerEnded)) {
        write<u8>(actor,TimerEnded,1);gabi::call(0x025C58A8,timer,-1);
        if(read<s16>(actor,RaceState)!=3) {
            write<s16>(actor,RaceState,2);gabi::call(0x025E1988,0x847);
        }
    }
    s16 state=read<s16>(actor,RaceState);
    if(state!=3 && state!=2) return TRUE;
    if(read<u16>(actor,0xF8)!=2) {
        u32 name=gabi::load<u32>(0x101B5D0C+(state!=3?4:0));
        gabi::call(0x025D77DC,actor,gabi::at<void>(name),1,0xFFFF);
        write<u16>(actor,0xFA,read<u16>(actor,0xFA)|2);return TRUE;
    }
    gabi::call(0x025C577C,timer);
    s32 finish=0;
    if(read<s16>(actor,RaceState)==3) {
        u32 play=gabi::call<u32>(0x025200D4);
        finish=gabi::load<s16>(play+0x5CEC)<gabi::load<s32>(0x10464180)?1:2;
        play=gabi::call<u32>(0x025200D4);gabi::store<u8>(play+0x5CEE,1);
    } else {
        u32 play=gabi::call<u32>(0x025200D4);gabi::store<u8>(play+0x5CEE,2);
        gabi::call<u32>(0x025200D4);
    }
    gabi::call(0x025E18EC,(finish==1 || finish==2)?0x8000000Cu:0x8000000Du);
    u32 play=gabi::call<u32>(0x025200D4);
    gabi::store<u32>(play+0x5CD8,gabi::load<u32>(play+0x5CD8)&~0x00200000u);
    s16 finalState=read<s16>(actor,RaceState);
    u32 singleton=gabi::load<u32>(0x101F8344);
    if(finalState==3) {
        void* terminater=gabi::at<void>(gabi::load<u32>(singleton+0x200));
        if(terminater) gabi::call(0x02627E14,terminater,finish);
    } else {
        void* failure=gabi::at<void>(gabi::load<u32>(singleton+0x1FC));
        if(failure) write<u8>(failure,0x60,1);
    }
    write<s16>(actor,CameraFrames,0);write<s16>(actor,ActionDispatch,-1);
    write<u32>(actor,ActionTarget,0x02160C50);write<s16>(actor,ActionAdjustment,0);write<u8>(actor,StartState,0);
    return TRUE;
}
VERIFY(0x02160954,Goal_TimerExecute);
static void Goal_setNrmVtx(void* packet,cXyz* output,s32 column,s32 row) {
    WWHD_FUNC(0x0215E520,void,packet,output,column,row);
    gabi::Local<cXyz> center,average,horizontal,vertical,triangle,temp,normalized;
    cXyz* positions=gabi::at<cXyz>(gabi::ea(packet)+0x1784+(u32)read<u8>(packet,0x2648)*0x21C);
    s32 index=row*9+column;
    center->set(positions[index].x,positions[index].y,positions[index].z);
    average->set(0,0,0);
    auto edge=[&](s32 neighbor,cXyz* destination) {
        gabi::call(0x0201ADE0,&positions[neighbor],temp.get(),center.get());destination->copy(*temp);
    };
    auto accumulate=[&](cXyz* a,cXyz* b) {
        gabi::call(0x0201B080,a,temp.get(),b);triangle->copy(*temp);
        gabi::call(0x0201B12C,triangle.get(),temp.get());triangle->copy(*temp);
        gabi::call(0x028E8D88,average.get(),triangle.get(),average.get());
    };
    if(column!=0) {
        edge(index-1,horizontal.get());
        if(row!=0) {edge(index-9,vertical.get());accumulate(vertical.get(),horizontal.get());}
        if(row!=4) {edge(index+9,vertical.get());accumulate(horizontal.get(),vertical.get());}
    }
    if(column!=8) {
        edge(index+1,horizontal.get());
        if(row!=0) {edge(index-9,vertical.get());accumulate(horizontal.get(),vertical.get());}
        if(row!=4) {edge(index+9,vertical.get());accumulate(vertical.get(),horizontal.get());}
    }
    gabi::call(0x0201B1E4,average.get(),normalized.get());average->copy(*normalized);
    gabi::call(0x0200FCF0);
    s32 phase=column*-0x400+row*0x100;
    u16 angle=(u16)(phase+read<s16>(packet,0x98));
    f32 sine=gabi::load<f32>(0x104A44F8+(angle>>3)*8);
    gabi::call(0x025F1C28,gabi::at<void>(gabi::load<u32>(0x1018C7B0)),(s16)gabi::ftoi(512.0f*sine));
    angle=(u16)(phase+read<s16>(packet,0x98));
    f32 cosine=gabi::load<f32>(0x104A44FC+(angle>>3)*8);
    gabi::call(0x025F1BF4,gabi::at<void>(gabi::load<u32>(0x1018C7B0)),(s16)gabi::ftoi(512.0f*cosine));
    gabi::call(0x0200FCD8,average.get(),triangle.get());
    gabi::call(0x0201B12C,triangle.get(),normalized.get());output->copy(*normalized);
    gabi::call(0x0200FD38);
}
VERIFY(0x0215E520,Goal_setNrmVtx);
static f32 Goal_sine(u16 angle) {return gabi::load<f32>(0x104A44F8+(angle>>3)*8);}
static void Goal_flagMove(void* actor) {
    WWHD_FUNC(0x0215EA00,void,actor);
    gabi::Local<cXyz> wind,normalized,factor,temp;
    u8 oldArray=read<u8>(actor,0x29F4),newArray=(oldArray^1)&1;
    s16 normalPhase=read<s16>(actor,0x444),windPhase=read<s16>(actor,0x2A50),wavePhase=read<s16>(actor,0x446);
    write<u8>(actor,0x29F4,newArray);
    windPhase=(s16)(windPhase+gabi::load<u32>(0x10464170));write<s16>(actor,0x2A50,windPhase);
    write<s16>(actor,0x444,(s16)(normalPhase+gabi::load<u32>(0x10464174)));
    write<s16>(actor,0x446,(s16)(wavePhase+gabi::load<u32>(0x10464178)));
    f32 blend=gabi::fmadds(0.5f,Goal_sine((u16)windPhase),0.5f);
    f32 windScale=gabi::fmadds(gabi::load<f32>(0x10464194),blend,gabi::load<f32>(0x10464198)*(1.0f-blend));
    cXyz* previous=gabi::at<cXyz>(gabi::ea(actor)+0x16F8+oldArray*0x21C);
    cXyz* previousNormals=gabi::at<cXyz>(gabi::ea(actor)+0x1F68+oldArray*0x21C);
    cXyz* current=gabi::at<cXyz>(gabi::ea(actor)+0x16F8+newArray*0x21C);
    cXyz* velocity=gabi::at<cXyz>(gabi::ea(actor)+0x27D8);
    gabi::call(0x025F1884,gabi::at<void>(gabi::load<u32>(0x1018C7B0)),(s16)-read<s16>(actor,0x322));
    void* worldWind=gabi::call<void*>(0x0257DAA8);
    gabi::call(0x0200FCD8,worldWind,wind.get());
    f32 magnitude=gabi::call<f32>(0x028E8DD0,wind.get());
    if(std::fabs(magnitude)<gabi::load<f32>(0x100030B8)) {
        for(u32 i=0;i<3;++i) write<u32>(wind.get(),i*4,gabi::load<u32>(0x101FFBA8+i*4));
    } else {
        gabi::call(0x0201B084,wind.get(),normalized.get());
        wind->y=normalized->y;wind->z=normalized->z;wind->x=(f32)normalized->x*0.2f;
        gabi::call(0x028E8E64,wind.get(),wind.get(),windScale);
    }
    for(s32 column=0;column<9;++column) for(s32 row=0;row<5;++row) {
        s32 index=column+row*9;
        current[index].copy(previous[index]);
        Goal_clothFactor(actor,factor.get(),previous,previousNormals,wind.get(),column,row);
        temp->copy(*factor);
        gabi::call(0x028E8D88,&velocity[index],temp.get(),&velocity[index]);
        gabi::call(0x028E8E64,&velocity[index],&velocity[index],0.85f);
        gabi::call(0x028E8D88,&current[index],&velocity[index],&current[index]);
    }
    cXyz* display=gabi::at<cXyz>(gabi::ea(actor)+0x1B30+(u32)read<u8>(actor,0x29F4)*0x21C);
    for(s32 column=0;column<9;++column) for(s32 row=0;row<5;++row) {
        s32 index=column+row*9;
        f32 distance=std::fabs(4.5f-(f32)column);
        f32 falloff=gabi::fnmsubs(distance,distance,20.25f)/20.25f;
        display[index].set(current[index].x,current[index].y,current[index].z);
        f32 sine=Goal_sine((u16)(column*0x4000+row*0x2000+read<s16>(actor,0x446)));
        display[index].z=gabi::fmadds(40.0f*falloff,sine,(f32)display[index].z);
    }
    cXyz* normals=gabi::at<cXyz>(gabi::ea(actor)+0x1F68+(u32)read<u8>(actor,0x29F4)*0x21C);
    for(s32 row=0;row<5;++row) for(s32 column=0;column<9;++column) Goal_setNrmVtx(member(actor,Packet),&normals[row*9+column],column,row);
    Goal_setBackNrm(member(actor,Packet));
    gabi::call(0xC00088B8,member(actor,0x1B30+(u32)read<u8>(actor,0x29F4)*0x21C),0x21C);
    gabi::call(0xC00088B8,member(actor,0x1F68+(u32)read<u8>(actor,0x29F4)*0x21C),0x21C);
    gabi::call(0xC00088B8,member(actor,0x23A0+(u32)read<u8>(actor,0x29F4)*0x21C),0x21C);
}
VERIFY(0x0215EA00,Goal_flagMove);
static void Goal_staticInit() {
    WWHD_FUNC(0x02160DA4,void,(u32)0);
    for(s32 i=3;i>=0;--i) gabi::store<u32>(0x104641A4+i*4,0);
    gabi::call(0x028F026C,gabi::at<void>(0x101B5D1C));
    gabi::store<f32>(0x10464148,-3.1415927410125732f);gabi::store<f32>(0x1046414C,3.1415927410125732f);
    gabi::call(0x028ED6F8,gabi::at<void>(0x10464160));
    gabi::call(0x028F026C,gabi::at<void>(0x101B5D28));
    gabi::call(0x028EAB2C,gabi::at<void>(0x10464161));
    gabi::call(0x028F026C,gabi::at<void>(0x101B5D34));
    void* hio=gabi::at<void>(0x10464164);
    write<u8>(hio,4,0xFF);gabi::store<f32>(0x10464158,10000.0f);
    write<u8>(hio,5,1);write<u32>(hio,0,0x1001037C);
    gabi::store<f32>(0x1046415C,10000.0f);write<u32>(hio,0xC,750);
    gabi::store<f32>(0x10464150,50000.0f);write<u32>(hio,0x10,0x800);
    write<u32>(hio,0x14,0xBD1);write<u32>(hio,0x18,240);
    write<u32>(hio,0x1C,150);write<u32>(hio,0x20,150);
    write<f32>(hio,0x30,12.0f);write<u32>(hio,0x24,255);write<u32>(hio,0x28,128);write<u32>(hio,0x2C,128);
    write<u8>(hio,7,0);write<u8>(hio,6,0);write<u8>(hio,8,0);write<u8>(hio,9,0);write<u8>(hio,10,0);
    write<f32>(hio,0x34,4.0f);write<f32>(hio,0x38,-1.0f);gabi::store<f32>(0x10464154,50000.0f);write<f32>(hio,0x3C,0.4f);
    gabi::call(0x028F026C,gabi::at<void>(0x101B5D40));
}
VERIFY(0x02160DA4,Goal_staticInit);
static BOOL Goal_Draw(void* actor) {
    WWHD_FUNC(0x0215FE9C,BOOL,actor);
    gabi::Local<be<f32>[12]> identity;
    gabi::Local<be<u8>[4]> color;
    void* light=gabi::call<void*>(0x02555D0C);
    void* tev=member(actor,0x110);
    gabi::call(0x025626A4,light,0,member(actor,0x314),tev);
    f32 x=read<f32>(actor,0x314),y=read<f32>(actor,0x318),z=read<f32>(actor,0x31C);
    gabi::call(0x0200FAD8,0,x,y,z);
    gabi::call(0x025F1C28,gabi::at<void>(gabi::load<u32>(0x1018C7B0)),read<s16>(actor,0x322));
    x=read<f32>(actor,0x330);y=read<f32>(actor,0x334);z=read<f32>(actor,0x338);
    gabi::call(0x0200FC74,1,x,y,z);
    gabi::call(0x028E9098,identity.get());
    gabi::call(0x028E9108,identity.get(),gabi::at<void>(gabi::load<u32>(0x1018C7B0)),member(actor,0x16C4));
    write<u32>(actor,0x16F4,gabi::ea(tev));
    gabi::call(0x027F0E04,gabi::at<void>(gabi::load<u32>(0x104B4634)),member(actor,Packet),0);
    gabi::call(0x0215F9E4,member(actor,Packet));
    for(s32 rope=0;rope<read<s32>(actor,RopeCount);++rope) {
        u16 count=(u16)(read<u32>(actor,BuoyCounts+rope*4)*4+1);
        (*color)[0]=(u8)gabi::load<u32>(0x10464188);(*color)[1]=(u8)gabi::load<u32>(0x1046418C);
        (*color)[2]=(u8)gabi::load<u32>(0x10464190);(*color)[3]=255;
        void* line=member(actor,RopeLines+rope*0x148);
        gabi::call(0x025EA548,line,count,color.get(),0,tev,20.0f);
        u32 play=gabi::call<u32>(0x025200D4);
        u32 target=gabi::load<u32>(read<u32>(line,0x130)+0x14);
        s32 type=gabi::call<s32>(target,line);
        gabi::call(0x025EDD04,gabi::at<void>(play+0x5FB4+(u32)type*0x9C),line);
    }
    return TRUE;
}
VERIFY(0x0215FE9C,Goal_Draw);
static void Goal_setTexObj(void* packet,u32 archive) {
    WWHD_FUNC(0x0215DFB4,void,packet,archive);
    gabi::Local<SafeString> name;
    gabi::call(0x0274FBF8,gabi::at<void>(gabi::load<u32>(0x101F8B18)));
    name->mStringTop=gabi::load<u32>(0x101B594C+(u32)archive*4);name->__vtbl=0x10010334;
    void* resource=gabi::call<void*>(0x026066C4,gabi::at<void>(gabi::load<u32>(0x101F4F28)),name.get(),3);
    if(!resource) gabi::call(0x0273AA24,gabi::at<void>(0x1001043C),0xDC,gabi::at<void>(0x10010450));
    gabi::call(0x02773798,member(packet,0xE44),gabi::at<void>(read<u32>(resource,0x20)));
    gabi::call(0x0274FCCC,gabi::at<void>(gabi::load<u32>(0x101F8B18)));
    const u32 fields[]={4,8,12,16,20,24,56,52,28};
    bool equal=true;
    for(u32 offset:fields) if(read<u32>(packet,0xF64+offset)!=read<u32>(packet,0xE44+offset)) {equal=false;break;}
    if(!equal) {
        gabi::call(0x027BDEB4,member(packet,0xF64),member(packet,0xE44));
        u8 flags=read<u8>(packet,0x10F4)|2;
        write<u32>(packet,0x10C4,2);write<u32>(packet,0x10C0,2);write<u32>(packet,0x10C8,2);write<u8>(packet,0x10F4,flags);
    } else {
        u32 image=read<u32>(packet,0xE6C),mips=read<u32>(packet,0xE74);
        write<u32>(packet,0x1038,image);
        u8 flags=read<u8>(packet,0x10F4)|2;
        write<u32>(packet,0xF94,mips);write<u32>(packet,0x1040,mips);
        write<u32>(packet,0x10C0,2);write<u32>(packet,0x10C4,2);write<u8>(packet,0x10F4,flags);
        write<u32>(packet,0xF8C,image);write<u32>(packet,0x10C8,2);
    }
}
VERIFY(0x0215DFB4,Goal_setTexObj);
static void Goal_freeAt(void* owner,u32 offset) {
    void* heap=gabi::call<void*>(0x02755FEC,gabi::at<void>(gabi::load<u32>(0x101F8B4C)),gabi::at<void>(read<u32>(owner,offset)));
    u32 target=gabi::load<u32>(read<u32>(heap,0xC)+0x3C);
    gabi::call(target,heap,gabi::at<void>(read<u32>(owner,offset)));
}
static void Goal_releaseVertices(void* vertices) {
    for(s32 pair=0;pair<2;++pair) for(s32 side=0;side<2;++side) {
        void* buffer=member(vertices,pair*0x4A8+side*0x254);
        gabi::call(0x027BF7E8,member(buffer,0x158));
        bool allocated=read<u32>(buffer,0x250)!=0;
        write<u32>(buffer,0,0);
        if(allocated) { (void)read<u32>(buffer,0x24C);Goal_freeAt(buffer,0x250);write<u32>(buffer,0x24C,0);write<u32>(buffer,0x250,0); }
    }
    write<u32>(vertices,0x960,0);
}
static void Goal_releaseTextures(void* table) {
    if(!read<u32>(table,4)) return;
    for(s32 entry=0;entry<read<s32>(table,0);++entry) {
        void* pair=member(gabi::at<void>(read<u32>(table,4)),entry*0x14);
        if(!pair) continue;
        bool allocated=read<u32>(pair,8)!=0;
        write<u32>(pair,0,0);
        if(allocated) {
            for(s32 i=0;i<read<s32>(pair,4);++i) {
                void* texture=member(gabi::at<void>(read<u32>(pair,8)),i*0xF4);
                gabi::call(gabi::load<u32>(read<u32>(texture,0xF0)+0xC),texture,2);
            }
            Goal_freeAt(pair,8);write<u32>(pair,4,0);write<u32>(pair,8,0);
        }
        if(read<u32>(pair,0x10)) {
            for(s32 i=0;i<read<s32>(pair,0xC);++i) {
                void* texture=member(gabi::at<void>(read<u32>(pair,0x10)),i*0xF4);
                gabi::call(gabi::load<u32>(read<u32>(texture,0xF0)+0xC),texture,2);
            }
            Goal_freeAt(pair,0x10);write<u32>(pair,0xC,0);write<u32>(pair,0x10,0);
        }
    }
    Goal_freeAt(table,4);write<u32>(table,0,0);write<u32>(table,4,0);
}
static void Goal_materialOwnerDtor(void* owner) {
    gabi::Local<be<u32>[4]> callFrame;
    gabi::call(0x027FD764,owner,2);
}
static void Goal_releasePacketMembers(void* packet) {
    void* vertices=member(packet,0xA8);
    Goal_releaseVertices(vertices);
    for(s32 material=0;material<read<s32>(packet,0xA10);++material) {
        u32 data=read<u32>(packet,0xA14);
        if((u32)material<read<u32>(packet,0xA10)) data+=(u32)material*0x23C;
        gabi::call(0x027BEBEC,gabi::at<void>(data+0x10));
        gabi::call(0x027BEBEC,gabi::at<void>(data+0x2C));
    }
    for(u32 block:{0xA2Cu,0xAD4u}) for(u32 i=0;i<2;++i) gabi::call(0x027BEBEC,member(packet,block+i*0x1C));
    gabi::call(0x027BE2B0,member(packet,0x10FC),2);gabi::call(0x027BE2B0,member(packet,0xF64),2);
    gabi::call(0x027B54A0,member(packet,0xE2C),2);
    gabi::call(0x027FB528,member(packet,0xAC4),0);gabi::call(0x027FB528,member(packet,0xA1C),0);
    Goal_materialOwnerDtor(member(packet,0xA10));
    if(vertices) {
        Goal_releaseVertices(vertices);
        gabi::call(0x028F0164,vertices,4,0x254,gabi::at<void>(0x02160FC8),0,0);
    }
    Goal_releaseTextures(member(packet,0xA0));
}
static void Goal_packetDtor(void* packet,s32 flags) {
    WWHD_FUNC(0x02161028,void,packet,flags);
    if(packet) {
        write<u32>(packet,0xC,0x1001061C);Goal_releasePacketMembers(packet);
        gabi::call(0x027F13DC,packet,0);if(flags&1) gabi::call(0x0273AF40,packet);
    }
}
VERIFY(0x02161028,Goal_packetDtor);
static void Goal_actorDtor(void* actor,s32 flags) {
    WWHD_FUNC(0x02161454,void,actor,flags);
    if(actor) {
        gabi::call(0x028F0164,member(actor,RopeLines),4,0x148,gabi::at<void>(0x025E99E0),0,0);
        void* packet=member(actor,Packet);
        write<u32>(packet,0xC,0x1001061C);Goal_releasePacketMembers(packet);
        gabi::call(0x027F13DC,packet,0);gabi::call(0x025D50BC,actor,0);
        if(flags&1) gabi::call(0x0273AF40,actor);
    }
}
VERIFY(0x02161454,Goal_actorDtor);
static void Goal_packetEmpty(void* packet) {WWHD_FUNC(0x02161450,void,packet);}
VERIFY(0x02161450,Goal_packetEmpty);
static void* Goal_packetCtor(void* packet) {
    WWHD_FUNC(0x0215D9D4,void*,packet);
    if(!packet) packet=gabi::call<void*>(0x0273AD10,0x264C);
    if(!packet) return nullptr;
    gabi::call(0x027F1278,packet);
    write<s16>(packet,0x98,0);write<u32>(packet,0x9C,0);write<u32>(packet,0xC,0x1001061C);
    void* table=member(packet,0xA0);
    if(!table) table=gabi::call<void*>(0x0273AD10,8);
    if(table) {write<u32>(table,4,0);write<u32>(table,0,0);}
    void* vertices=member(packet,0xA8);
    if(!vertices) vertices=gabi::call<void*>(0x0273AD10,0x968);
    if(vertices) {
        gabi::call(0x028EFFD0,vertices,4,0x254,gabi::at<void>(0x02160F24));
        write<u32>(vertices,0x950,0);write<u32>(vertices,0x960,0);write<u32>(vertices,0x958,0x20);
        write<u32>(vertices,0x954,0);write<u8>(vertices,0x964,0);
        for(u32 i=0;i<4;++i) write<u32>(vertices,i*0x254,0);
    }
    gabi::call(0x027FD6F4,member(packet,0xA10));
    gabi::call(0x027FB40C,member(packet,0xA1C));write<u32>(packet,0xA28,0x1016EF84);
    gabi::call(0x028F521C,member(packet,0xA90),0x34);
    if(!member(packet,0xA90)) gabi::call<void*>(0x0273AD10,0x30);
    gabi::call(0x027FB40C,member(packet,0xAC4));write<u32>(packet,0xAD0,0x1016EFB4);
    gabi::call(0x028F521C,member(packet,0xB38),0x2F0);
    // Identity matrices and default material vectors in the shader uniform block.
    const u32 zeroOffsets[]={0xB38,0xB3C,0xB40,0xB48,0xB70,0xB50,0xBA0,0xBC0,0xB90,0xB68,0xB8C,0xBBC,0xBAC,0xB7C,0xB58,0xB6C,0xB5C,0xB80,0xB4C,0xBA8,0xBB0,0xB9C,0xB88,0xBB8,0xB60,0xB98,0xB78,0xBC8,0xBCC,0xBD0,0xBD8,0xBDC,0xBE0};
    f32 zero=gabi::load<f32>(0x10145180),one=gabi::load<f32>(0x1014517C);
    for(u32 offset:zeroOffsets) write<f32>(packet,offset,zero);
    for(u32 offset:{0xB64u,0xB54u,0xBB4u,0xB74u,0xBC4u,0xB94u,0xBA4u,0xB44u,0xB84u,0xBD4u,0xBE4u}) write<f32>(packet,offset,one);
    for(u32 offset:{0xBE8u,0xC08u,0xC28u}) gabi::call(0x028EFFD0,member(packet,offset),2,0x10,gabi::at<void>(0x02160F80));
    for(u32 offset=0xC48;offset<=0xD98;offset+=0x30) if(!member(packet,offset)) gabi::call<void*>(0x0273AD10,0x30);
    for(u32 offset=0xDC8;offset<=0xE18;offset+=0x10) if(!member(packet,offset)) gabi::call<void*>(0x0273AD10,0x10);
    write<u8>(packet,0xE28,0);gabi::call(0x027B5430,member(packet,0xE2C));
    gabi::call(0x027BE6B8,member(packet,0xE44));gabi::call(0x027BE6B8,member(packet,0xED4));
    gabi::call(0x027BDF7C,member(packet,0xF64));gabi::call(0x027BDF7C,member(packet,0x10FC));
    write<u8>(packet,0x1294,0);write<u8>(packet,0x2648,0);
    gabi::call(0x0215D3C4,packet);
    return packet;
}
VERIFY(0x0215D9D4,Goal_packetCtor);
static void* Goal_allocate(u32 size,u32 alignment) {
    void* heap=gabi::call<void*>(0x02756140,gabi::at<void>(gabi::load<u32>(0x101F8B4C)));
    return gabi::call<void*>(gabi::load<u32>(read<u32>(heap,12)+0x34),heap,size,alignment);
}
static void Goal_releaseTexturePair(void* pair) {
    bool allocated=read<u32>(pair,8)!=0;write<u32>(pair,0,0);
    for(u32 countOffset:{4u,12u}) {
        if((countOffset==4?allocated:read<u32>(pair,countOffset+4)!=0)) {
            for(s32 i=0;i<read<s32>(pair,countOffset);++i) {
                void* texture=member(gabi::at<void>(read<u32>(pair,countOffset+4)),i*0xF4);
                gabi::call(gabi::load<u32>(read<u32>(texture,0xF0)+0xC),texture,2);
            }
            Goal_freeAt(pair,countOffset+4);write<u32>(pair,countOffset,0);write<u32>(pair,countOffset+4,0);
        }
    }
}
static void Goal_packetInit(void* packet) {
    WWHD_FUNC(0x0215D3C4,void,packet);
    gabi::Local<SafeString> name;
    name->mStringTop=0x10010428;name->__vtbl=0x10010334;
    void* archive=gabi::call<void*>(0x027FFCBC,packet);
    s32 index=gabi::call<s32>(0x027B90AC,gabi::at<void>(read<u32>(archive,4)),name.get());
    void* resource=nullptr;
    if(index>=0) {
        u32 count=read<u32>(archive,8),data=read<u32>(archive,12);
        void* entry=gabi::at<void>(data+((u32)index<count?(u32)index*0x24:0));
        if(!read<u8>(entry,0x20)) {
            void* root=gabi::at<void>(read<u32>(archive,4));
            u32 max=read<u32>(root,0x1C);
            void* shader=(u32)index<max?gabi::at<void>(read<u32>(root,0x20)+(u32)index*0x84):nullptr;
            gabi::call(0x02800B0C,entry,shader,0);
            count=read<u32>(archive,8);data=read<u32>(archive,12);
        }
        resource=gabi::at<void>(data+((u32)index<count?(u32)index*0x24:0));
    }
    { gabi::Local<be<u32>[4]> callFrame; gabi::call(0x0280068C,member(packet,0x9C),resource,0); }
    write<u32>(packet,0x9FC,0x13);write<u32>(packet,0xA04,0x10010610);
    void* vertices=member(packet,0xA8);
    for(u32 side=0;side<2;++side) for(u32 pair=0;pair<2;++pair) {
        void* buffer=member(vertices,side*0x254+pair*0x4A8);
        u32 vertexData=read<u32>(buffer,0);
        if(!vertexData) {
            void* allocation=Goal_allocate(0x5A0,0x40);
            if(allocation) {write<u32>(buffer,0x250,gabi::ea(allocation));write<u32>(buffer,0x24C,45);}
            vertexData=read<u32>(buffer,0x250);write<u32>(buffer,0,vertexData);
        }
        gabi::call(0x027FF478,member(buffer,4),gabi::at<void>(vertexData),45,member(vertices,0x954));
    }
    write<u32>(vertices,0x960,0);write<u8>(vertices,0x964,1);
    for(u32 material=0;material<read<u32>(packet,0x9C);++material) {
        u32 data=read<u32>(packet,0xA4);
        if(material<read<u32>(packet,0xA0)) data+=material*0x14;
        void* pair=gabi::at<void>(data);u32 materialId=read<u32>(pair,0);
        Goal_releaseTexturePair(pair);write<u32>(pair,0,materialId);
        for(u32 countOffset:{4u,12u}) {
            void* textures=Goal_allocate(0x1E8,4);
            for(u32 i=0;i<2;++i) if(member(textures,i*0xF4)) gabi::call(0x027BF734,member(textures,i*0xF4));
            if(textures) {write<u32>(pair,countOffset+4,gabi::ea(textures));write<u32>(pair,countOffset,2);}
        }
        for(u32 side=0;side<2;++side) for(u32 pairIndex=0;pairIndex<2;++pairIndex) {
            u32 countOffset=4+side*8;
            void* texture=gabi::at<void>(read<u32>(pair,countOffset+4)+(pairIndex<read<u32>(pair,countOffset)?pairIndex*0xF4:0));
            gabi::call(0x027FF530,materialId,texture,member(vertices,4+side*0x254+pairIndex*0x4A8),member(vertices,0x954),0);
        }
    }
    gabi::call(0x027FE084,member(packet,0xA10),1,0);
    gabi::call(0x027B54E0,member(packet,0xE2C),gabi::at<void>(0x10010394),4,0x48);
    write<u32>(packet,0xE30,6);
    for(u32 side=0;side<2;++side) {
        u32 active=read<u32>(vertices,0x950);
        u32 destination=read<u32>(vertices,(side*2+active)*0x254);
        u32 end=destination+0x5A0;
        for(u32 block=destination;block<end;block+=32) for(u32 i=0;i<32;i+=4) gabi::store<u32>((block&~31u)+i,0);
        destination=read<u32>(vertices,(side*2+read<u32>(vertices,0x950))*0x254);
        for(u32 vertex=0;vertex<45;++vertex) {
            u32 target=destination+vertex*32;
            f32 x=gabi::load<f32>(0x101B5974+vertex*12),y=gabi::load<f32>(0x101B5978+vertex*12),z=gabi::load<f32>(0x101B597C+vertex*12);
            gabi::store<f32>(target,x);gabi::store<f32>(target+12,0);gabi::store<f32>(target+8,z);gabi::store<f32>(target+16,0);gabi::store<f32>(target+4,y);gabi::store<f32>(target+20,0);
            gabi::store<f32>(target+24,gabi::load<f32>(0x101B5B90+vertex*8));gabi::store<f32>(target+28,gabi::load<f32>(0x101B5B94+vertex*8));
        }
    }
    u32 active=read<u32>(vertices,0x950),inactive=active==0?1:0;
    for(u32 side=0;side<2;++side) {
        for(u32 vertex=0;vertex<45;++vertex) {
            u32 source=read<u32>(vertices,(side*2+active)*0x254)+vertex*32;
            u32 target=read<u32>(vertices,(side*2+inactive)*0x254)+vertex*32;
            for(u32 i=0;i<8;++i) gabi::store<f32>(target+i*4,gabi::load<f32>(source+i*4));
        }
        active=read<u32>(vertices,0x950);
    }
    void* flush=member(vertices,read<u32>(vertices,0x950)*0x254+4);
    for(u32 side=0;side<2;++side) {gabi::call(0x027B5E94,member(flush,side*0x4A8),0,read<u32>(flush,side*0x4A8+0x14C));}
    write<u32>(vertices,0x950,read<u32>(vertices,0x950)==0?1:0);
}
VERIFY(0x0215D3C4,Goal_packetInit);
static s32 Goal_Create(void* actor) {
    WWHD_FUNC(0x0215EDF8,s32,actor);
    gabi::Local<SafeString> expectedStage,currentStage;
    gabi::Local<cXyz> playerOffset,line,normalized,normal;
    if(!(read<u32>(actor,0x2E4)&8)) {
        if(actor) {
            gabi::call(0x025D4ED0,actor);write<u32>(actor,0xB4,0x1001036C);
            Goal_packetCtor(member(actor,Packet));
            gabi::call(0x028EFFD0,member(actor,RopeLines),4,0x148,gabi::at<void>(0x025E9960));
        }
        write<u32>(actor,0x2E4,read<u32>(actor,0x2E4)|8);
    }
    u8 variant=read<u8>(actor,0xB3);
    s32 phase=gabi::call<s32>(0x02520460,member(actor,0x29F8),gabi::at<void>(0x1001030C));
    if(phase!=4) return phase;
    u8 archive=(variant==0 || variant==255)?0:1;
    phase=gabi::call<s32>(0x02520460,member(actor,0x2A00),gabi::at<void>(archive?0x1001032C:0x10010314));
    write<f32>(actor,0x330,archive?0.98f:1.05f);write<f32>(actor,0x338,1);write<f32>(actor,0x334,1);
    if(phase!=4) return phase;
    u8 pathId=read<u32>(actor,0xB0)>>16;
    if(pathId==255) return 5;
    void* path=gabi::call<void*>(0x025AAF88,pathId,read<s8>(actor,0x326));
    if(!path) return 5;
    if(Goal_getRacePath(actor,read<u8>(path,3))) {
        if(!gabi::call<s32>(0x025D63E8,actor,gabi::at<void>(0x0215D3C0),0x10000)) return 5;
        Goal_CreateBuoyRaces(actor);
    }
    cXyz* positions=gabi::at<cXyz>(gabi::ea(actor)+0x16F8+(u32)read<u8>(actor,0x29F4)*0x21C);
    for(u32 vertex=0;vertex<45;++vertex) positions[vertex].set(gabi::load<f32>(0x101B5974+vertex*12),gabi::load<f32>(0x101B5978+vertex*12),gabi::load<f32>(0x101B597C+vertex*12));
    Goal_setTexObj(member(actor,Packet),archive);
    pathId=read<u32>(actor,0xB0)>>16;
    path=gabi::call<void*>(0x025AAF88,pathId,read<s8>(actor,0x326));
    f32 x=read<f32>(actor,0x314);
    if(path) {
        void* points=gabi::at<void>(read<u32>(path,8));
        for(u32 i=0;i<3;++i) write<f32>(actor,0x2A38+i*4,read<f32>(points,4+i*4));
        points=gabi::at<void>(read<u32>(path,8));
        for(u32 i=0;i<3;++i) write<f32>(actor,0x2A44+i*4,read<f32>(points,0x14+i*4));
    }
    gabi::call(0x028E93CC,gabi::at<void>(0x1048D0CC),x,read<f32>(actor,0x318),read<f32>(actor,0x31C));
    gabi::call(0x025F1C28,gabi::at<void>(0x1048D0CC),read<s16>(actor,0x32A));
    gabi::call(0x025F2518,read<f32>(actor,0x330),read<f32>(actor,0x334),read<f32>(actor,0x338));
    gabi::call(0x028E90D4,gabi::at<void>(0x1048D0CC),member(actor,0x2A08));
    expectedStage->mStringTop=0x1001031C;expectedStage->__vtbl=0x10010334;
    u32 play=gabi::call<u32>(0x025200D4);
    currentStage->mStringTop=play+0x5134;currentStage->__vtbl=0x10010334;
    auto validateString=[](SafeString* string) {gabi::call(gabi::load<u32>((u32)string->__vtbl+0x14),string);};
    validateString(expectedStage.get());validateString(expectedStage.get());
    u32 expected=expectedStage->mStringTop;
    validateString(currentStage.get());u32 current=currentStage->mStringTop;
    bool equal=expected==current;
    if(!equal) {
        expected=expectedStage->mStringTop;current=currentStage->mStringTop;
        for(u32 i=0;i<0x40001;++i) {
            u8 a=gabi::load<u8>(expected+i),b=gabi::load<u8>(current+i);
            if(a!=b) break;
            if(a==0) {equal=true;break;}
        }
    }
    bool start=false;
    if(equal) {play=gabi::call<u32>(0x025200D4);start=gabi::load<s16>(play+0x513C)==1;}
    if(start) {
        u32 save=gabi::load<u32>(0x101F84DC);
        u16 limit=gabi::load<u16>(0x1046417E);
        u16 modifier=(u16)(gabi::call<u32>(0x025B8BB0,gabi::at<void>(save+0x644),0xAAFF)*10);
        if(limit>modifier) limit-=modifier;
        s32 timer=gabi::call<s32>(0x025DB468,0x1E0,2,limit,3,0,0,221.0f,439.0f,32.0f,419.0f);
        write<s32>(actor,TimerId,timer);
        u32 singleton=gabi::load<u32>(0x101F8344);
        void* starter=gabi::at<void>(gabi::load<u32>(singleton+0x204));
        if(starter) {write<u8>(starter,0x50,0);write<u8>(starter,0x51,0);singleton=gabi::load<u32>(0x101F8344);}
        void* terminater=gabi::at<void>(gabi::load<u32>(singleton+0x200));
        if(terminater) {write<u8>(terminater,0x64,0);gabi::call(0x020063C0,member(terminater,0x18),gabi::at<void>(0x1049DD60));singleton=gabi::load<u32>(0x101F8344);}
        void* failure=gabi::at<void>(gabi::load<u32>(singleton+0x1FC));if(failure) gabi::call(0x026266C8,failure);
        play=gabi::call<u32>(0x025200D4);u16 flags=gabi::load<u16>(play+0x5CE8);gabi::store<u8>(play+0x5CEA,1);gabi::store<u16>(play+0x5CE8,flags|1);
        play=gabi::call<u32>(0x025200D4);gabi::store<u16>(play+0x5CEC,0);
        play=gabi::call<u32>(0x025200D4);gabi::store<u8>(play+0x5D2C,0);
        write<u8>(actor,StartState,0);write<s16>(actor,ActionAdjustment,0);write<s16>(actor,ActionDispatch,-1);write<u32>(actor,ActionTarget,0x021607D4);
    }
    play=gabi::call<u32>(0x025200D4);
    void* player=gabi::at<void>(gabi::load<u32>(play+0x5B2C));
    gabi::call(0x0201ADE0,member(player,0x314),playerOffset.get(),member(actor,0x2A38));
    gabi::call(0x0201ADE0,member(actor,0x2A44),line.get(),member(actor,0x2A38));line->y=0;
    gabi::call(0x0201B12C,line.get(),normalized.get());
    line->set(normalized->x,normalized->y,normalized->z);normal->set(line->z,0,-(f32)line->x);
    f32 side=gabi::call<f32>(0x028E8F44,normal.get(),playerOffset.get());
    write<s16>(actor,RaceState,0);write<f32>(actor,PreviousSide,side);
    for(u32 i=0;i<20;++i) Goal_flagMove(actor);
    write<u8>(actor,TimerEnded,0);
    return 4;
}
VERIFY(0x0215EDF8,Goal_Create);
static void Goal_packetUpdate(void* packet) {
    WWHD_FUNC(0x0215F9E4,void,packet);
    gabi::Local<SafeString> name;
    gabi::Local<be<f32>[12]> matrix;
    gabi::Local<be<f32>[4]> color,scaledColor;
    gabi::call(0x0274FBF8,gabi::at<void>(gabi::load<u32>(0x101F8B18)));
    if(!read<u8>(packet,0x1294)) {
        name->mStringTop=0x100104F8;name->__vtbl=0x10010334;
        void* resource=gabi::call<void*>(0x026066C4,gabi::at<void>(gabi::load<u32>(0x101F4F28)),name.get(),3);
        if(!resource) gabi::call(0x0273AA24,gabi::at<void>(0x10010500),0x236,gabi::at<void>(0x10010514));
        gabi::call(0x02773798,member(packet,0xED4),gabi::at<void>(read<u32>(resource,0x20)));
        bool equal=true;
        for(u32 offset:{4u,8u,12u,16u,20u,24u,56u,52u,28u}) if(read<u32>(packet,0x10FC+offset)!=read<u32>(packet,0xED4+offset)) {equal=false;break;}
        if(!equal) {
            gabi::call(0x027BDEB4,member(packet,0x10FC),member(packet,0xED4));
            u8 flags=read<u8>(packet,0x128C)|2;
            write<u32>(packet,0x125C,2);write<u32>(packet,0x1258,2);write<u32>(packet,0x1260,2);write<u8>(packet,0x128C,flags);
        } else {
            u32 image=read<u32>(packet,0xEFC),mips=read<u32>(packet,0xF04);
            write<u32>(packet,0x11D0,image);u8 flags=read<u8>(packet,0x128C)|2;
            write<u32>(packet,0x112C,mips);write<u32>(packet,0x11D8,mips);
            write<u32>(packet,0x1258,2);write<u32>(packet,0x125C,2);write<u8>(packet,0x128C,flags);
            write<u32>(packet,0x1124,image);write<u32>(packet,0x1260,2);
        }
        write<u8>(packet,0x1294,1);
    }
    gabi::call(0x0274FCCC,gabi::at<void>(gabi::load<u32>(0x101F8B18)));
    void* vertices=member(packet,0xA8);
    for(u32 side=0;side<2;++side) {
        u32 destination=read<u32>(vertices,(side*2+read<u32>(vertices,0x950))*0x254);
        for(u32 vertex=0;vertex<45;++vertex) {
            u32 array=read<u8>(packet,0x2648)*0x21C;
            cXyz* position=gabi::at<cXyz>(gabi::ea(packet)+0x134C+array+vertex*12);
            f32 x=position->x,y=position->y,z=position->z;
            u32 target=destination+vertex*32;
            gabi::store<f32>(target,x);gabi::store<f32>(target+4,y);gabi::store<f32>(target+8,z);
            array=read<u8>(packet,0x2648)*0x21C;
            cXyz* normal=gabi::at<cXyz>(gabi::ea(packet)+(side?0x1FF4:0x1BBC)+array+vertex*12);
            x=normal->x;y=normal->y;z=normal->z;
            gabi::store<f32>(target+20,z);gabi::store<f32>(target+12,x);gabi::store<f32>(target+16,y);
        }
    }
    void* flush=member(vertices,read<u32>(vertices,0x950)*0x254+4);
    for(u32 side=0;side<2;++side) gabi::call(0x027B5E94,member(flush,side*0x4A8),0,read<u32>(flush,side*0x4A8+0x14C));
    write<u32>(vertices,0x950,read<u32>(vertices,0x950)==0?1:0);
    gabi::call(0x0255F8F4,gabi::at<void>(read<u32>(packet,0x1348)));
    gabi::call(0x0255FE90,gabi::at<void>(read<u32>(packet,0x1348)));
    Goal_copyMatrix(matrix.get(),gabi::at<void>(0x104B45F8));
    gabi::call(0x027FDA54,member(packet,0xA10),0,matrix.get(),gabi::at<void>(0x104B470C),gabi::at<void>(gabi::load<u32>(0x104B4708)+0x240));
    for(u32 lightIndex=0;lightIndex<2;++lightIndex) {
        void* tev=gabi::at<void>(read<u32>(packet,0x1348));u32 material=read<u32>(packet,0xA14);
        Goal_colorS10ToFloat(color.get(),member(tev,lightIndex?0x160:0x90));
        tev=gabi::at<void>(read<u32>(packet,0x1348));
        gabi::call(0x0274D458,scaledColor.get(),color.get(),read<f32>(tev,lightIndex?0x16C:0x28));
        for(u32 component=0;component<4;++component) gabi::store<u32>(material+0x1C4+lightIndex*16+component*4,read<u32>(scaledColor.get(),component*4));
    }
    gabi::call(0x027FDFF4,member(packet,0xA10),0);
    void* tev=gabi::at<void>(read<u32>(packet,0x1348));
    Goal_colorS10ToFloat(member(packet,0xB78),member(tev,0x90));
    Goal_colorToFloat(member(packet,0xB88),member(tev,0x98));
    void* currentTev=gabi::at<void>(read<u32>(packet,0x1348));
    gabi::call(0x0274D2AC,member(packet,0xB88),read<f32>(currentTev,0x24));
    if(read<u8>(tev,0x9F)) Goal_colorToFloat(member(packet,0xB98),member(tev,0x9C));
    else for(u32 offset:{0xB9Cu,0xB98u,0xBA0u,0xBA4u}) write<f32>(packet,offset,0);
    gabi::call(0x027FB678,member(packet,0xAC4));
    Goal_copyMatrix(matrix.get(),member(packet,0x1318));
    gabi::call(0x028E90D4,matrix.get(),member(packet,0xA90));
    gabi::call(0x027FB678,member(packet,0xA1C));
}
VERIFY(0x0215F9E4,Goal_packetUpdate);
static void Goal_bindUniforms(void* shader,void* material) {
    void* descriptor=read<u32>(material,0xC)?gabi::at<void>(read<u32>(material,0x10)):nullptr;
    s16 vertex=read<s16>(descriptor,0xC),pixel=read<s16>(descriptor,0xE),geometry=read<s16>(descriptor,0x10);
    void* block=member(shader,0x10+read<u32>(shader,0x4C)*0x1C);
    u32 size=read<u32>(block,0xC),data=read<u32>(block,4);
    if(pixel!=-1) gabi::call(0xC0006900,pixel,size,gabi::at<void>(data));
    if(vertex!=-1) gabi::call(0xC0006A38,vertex,size,gabi::at<void>(data));
    if(geometry!=-1) gabi::call(0xC00068A8,geometry,size,gabi::at<void>(data));
}
static void Goal_loadTexture(void* packet,void* material,u32 textureIndex) {
    u32 count=read<u32>(material,0x14);
    u32 descriptor=0;
    if(count>textureIndex) descriptor=read<u32>(material,0x18)+textureIndex*0x14;
    gabi::call(0x027BE53C,member(packet,textureIndex?0x10FC:0xF64),gabi::at<void>(descriptor+4),-1,0);
}
static void Goal_packetDraw(void* packet,void* drawInfo) {
    WWHD_FUNC(0x02160018,void,packet,drawInfo);
    gabi::Local<be<u8>[0x11C]> renderState; /* HD sizeof of the 02750250 draw state (vtable at +0x118); was 0x118 — strict constructor-size check 2026-10-05 */
    void* material=nullptr;
    s32 index=read<s32>(drawInfo,0xC);
    if(index<4) {
        u32 data=read<u32>(packet,0xA4);
        if((u32)index<read<u32>(packet,0xA0)) data+=(u32)index*0x14;
        material=gabi::at<void>(gabi::load<u32>(data));
    }
    void* cache=gabi::call<void*>(0x027F29D4,gabi::at<void>(0x104B45C0));
    void* shader=gabi::at<void>(read<u32>(material,0));
    if(gabi::ea(shader)!=read<u32>(cache,4)) {
        u32 oldShader=read<u32>(cache,0);u8 flags=read<u8>(shader,0);
        if(flags&2) {write<u8>(shader,0,flags&~2);gabi::call(0x027BB9E0,shader,0);}
        u32 nativeShader=gabi::load<u32>(read<u32>(shader,0x7C)+0x28);
        if(oldShader!=nativeShader) gabi::call(0x027B9F68,gabi::at<void>(nativeShader));
        u32 fetch=read<u32>(shader,0xC);
        if(fetch) gabi::call(0xC00060E0,gabi::at<void>(read<u32>(shader,4)),gabi::at<void>(fetch));
        else gabi::call(0x027BB7CC,shader);
        write<u32>(cache,4,gabi::ea(shader));write<u32>(cache,0,nativeShader);
    }
    index=read<s32>(drawInfo,0xC);
    auto upload=[&](u32 offset) {u32 target=gabi::load<u32>(read<u32>(packet,offset+12)+0x2C);gabi::call(target,member(packet,offset),material);};
    if(index==0) {
        upload(0xA1C);
        void* pass=gabi::at<void>(read<u32>(drawInfo,0x14));
        if(pass) Goal_bindUniforms(gabi::at<void>(read<u32>(pass,4)),material);
        Goal_loadTexture(packet,material,0);
    } else if(index==1 || index==2) {
        Goal_bindUniforms(gabi::at<void>(read<u32>(packet,0xA14)),material);
        upload(0xAC4);upload(0xA1C);
        if(index==2) {
            void* extra=gabi::at<void>(read<u32>(drawInfo,0x30));
            if(extra) gabi::call(gabi::load<u32>(read<u32>(extra,12)+0x2C),extra,material);
        }
        Goal_loadTexture(packet,material,0);Goal_loadTexture(packet,material,1);
    }
    if(index==2) gabi::call(0x027FFE54,drawInfo,material);
    gabi::call(0x02750250,renderState.get());write<u32>(renderState.get(),8,0);
    s32 pass=read<s32>(drawInfo,0xC);
    write<u32>(renderState.get(),12,2);
    u32 flags=read<u32>(renderState.get(),0xEC);
    write<u8>(renderState.get(),0xE0,1);write<f32>(renderState.get(),0xE8,0.5f);write<u32>(renderState.get(),0xE4,4);
    write<u32>(renderState.get(),0xEC,(((flags&0xFFFFFFF0)+7)&0xFFFFFF0F)+0x10);
    gabi::call(0x0280037C,pass,renderState.get());gabi::call(0x02750370,renderState.get());
    for(u32 side=0;side<2;++side) {
        if(side) {write<u32>(renderState.get(),8,1);gabi::call(0x02750370,renderState.get());}
        u32 data=read<u32>(packet,0xA4),entry=read<u32>(drawInfo,0xC);
        if(entry<read<u32>(packet,0xA0)) data+=entry*0x14;
        u32 countOffset=4+(read<u32>(packet,0x9F8)==0?8:0);
        u32 texture=gabi::load<u32>(data+countOffset+4);
        if(side && gabi::load<u32>(data+countOffset)>1) texture+=0xF4;
        gabi::call(0x027BFE5C,gabi::at<void>(texture));
        for(u32 row=0;row<4;++row) {
            u32 stride=read<u32>(packet,0xE3C),mode=read<u32>(packet,0xE30),indices=read<u32>(packet,0xE34),type=read<u32>(packet,0xE2C);
            gabi::call(0xC0006178,mode,0x12,type,gabi::at<void>(indices+stride*row*0x12),0,1);
        }
    }
    gabi::call(0x02750370,gabi::at<void>(0x104B474C));
}
VERIFY(0x02160018,Goal_packetDraw);
