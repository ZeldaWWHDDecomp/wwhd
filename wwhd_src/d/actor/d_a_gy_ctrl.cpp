/* Gyorg controller, reconstructed from WWHD. */
#include "d/actor/d_a_gy_ctrl.h"
template<class T> static T rd(u32 p,u32 o=0) { return gabi::load<T>(p+o); }
template<class T> static void wr(u32 p,u32 o,T v) { gabi::store<T>(p+o,v); }
static u32 play() { return gabi::call<u32>(0x025200D4); }
static void copy3(u32 dst,u32 src) { for(u32 i=0;i<12;i+=4) wr<u32>(dst,i,rd<u32>(src,i)); }
static u32 shiftRight(u32 v,u32 n) { n&=63; return n<32?v>>n:0; }
static u32 shiftLeft(u32 v,u32 n) { n&=63; return n<32?v<<n:0; }
void* daGyCtrl_SearchNear(daGyCtrl_c* self,void* actor) {
    WWHD_FUNC(0x0216B6E8,void*,self,actor);
    if(!gabi::call<BOOL>(0x025D4604,actor)) return nullptr;
    f32 distance=gabi::call<f32>(0x025D6958,self,actor);
    u32 p=gabi::ea(actor); s16 id=actor?rd<s16>(p,8):0;
    if(self->mType==0 && actor && id==0xE6 && distance<6000.0f) return actor;
    if(actor && (id==0xE1 || id==0x44 || id==0x76) && distance<6000.0f) return actor;
    return nullptr;
}
VERIFY(0x0216B6E8,daGyCtrl_SearchNear);
void* daGyCtrl_SearchCallback(daGyCtrl_c* self,void* actor) {
    WWHD_FUNC(0x0216B7B4,void*,self,actor);
    return gabi::call<void*>(0x0216B6E8,actor,self);
}
VERIFY(0x0216B7B4,daGyCtrl_SearchCallback);
u8 daGyCtrl_GetParam(daGyCtrl_c* self,u32 param,u32 shift,u32 bits) {
    WWHD_FUNC(0x0216B7C4,u8,self,param,shift,bits);
    return (u8)(shiftRight(param,shift)&(shiftLeft(1,bits)-1));
}
VERIFY(0x0216B7C4,daGyCtrl_GetParam);
void daGyCtrl_GetArg(daGyCtrl_c* self) {
    WWHD_FUNC(0x0216B7E0,void,self);
    u32 param=rd<u32>(gabi::ea(self),0xB0);
    self->mType=gabi::call<u8>(0x0216B7C4,self,param,0,4);
    self->mCount=gabi::call<u8>(0x0216B7C4,self,param,4,4);
    u8 distance=gabi::call<u8>(0x0216B7C4,self,param,8,8);
    self->mSwitch=gabi::call<u8>(0x0216B7C4,self,param,24,8);
    if(self->mType==15) self->mType=0;
    if(self->mCount==15) self->mCount=1;
    self->mMaxDistance=distance==255?10000.0f:(f32)distance*1000.0f;
}
VERIFY(0x0216B7E0,daGyCtrl_GetArg);
BOOL daGyCtrl_CheckExist(daGyCtrl_c* self) {
    WWHD_FUNC(0x0216B8B8,BOOL,self);
    gabi::Local<be<s16>> id; *id=0xE5;
    u32 actor=gabi::call<u32>(0x025D5218,0x025E121C,id.get());
    return actor && rd<u8>(actor,0x444)==0 && rd<u8>(actor,0x44D)!=0;
}
VERIFY(0x0216B8B8,daGyCtrl_CheckExist);
void daGyCtrl_ModeProc(daGyCtrl_c* self,s32 action,s32 mode) {
    WWHD_FUNC(0x0216B924,void,self,action,mode);
    u32 descriptor;
    if(action==0) { self->mMode=mode; descriptor=0x10010C64+(u32)mode*20; }
    else if(action==1) descriptor=0x10010C64+(u32)(s32)self->mMode*20+8;
    else return;
    u32 receiver=gabi::ea(self)+(s32)rd<s16>(descriptor);
    s16 slot=rd<s16>(descriptor,2); u32 target;
    if(slot<0) target=rd<u32>(descriptor,4);
    else target=rd<u32>(rd<u32>(receiver+(s32)rd<s16>(descriptor,6)),(u32)(s32)slot*8+4);
    gabi::call_ptr(target,receiver);
}
VERIFY(0x0216B924,daGyCtrl_ModeProc);
void daGyCtrl_CreateInitNoArea(daGyCtrl_c* self) {
    WWHD_FUNC(0x0216B9D0,void,self);
    gabi::call(0x025D537C,self); self->mLayerInitialized=1;
    self->mRoom=rd<s8>(gabi::ea(self),0x326);
}
VERIFY(0x0216B9D0,daGyCtrl_CreateInitNoArea);
void daGyCtrl_CreateInit(daGyCtrl_c* self) {
    WWHD_FUNC(0x0216BA10,void,self);
    gabi::call(0x0216B924,self,0,self->mSwitch==255?1:0);
    copy3(gabi::ea(self)+0x454,gabi::ea(self)+0x314); self->mTarget=0;
    if(self->mType==0) gabi::call(0x0216B9D0,self);
}
VERIFY(0x0216BA10,daGyCtrl_CreateInit);
s32 daGyCtrl_Create(daGyCtrl_c* self) {
    WWHD_FUNC(0x0216BA8C,s32,self);
    u32 p=gabi::ea(self),status=rd<u32>(p,0x2E4);
    if(!(status&8)) {
        if(self) {
            gabi::call(0x025D4ED0,self); wr<u32>(p,0xB4,0x10010D78);
            gabi::call(0x02008FEC,p+0x498);
            wr<u8>(p,0x4F8,0);wr<u8>(p,0x4F5,0);wr<u32>(p,0x4B8,0x10010D48);
            wr<u32>(p,0x500,1);wr<u8>(p,0x4F9,0);wr<u8>(p,0x4F7,0);
            wr<u32>(p,0x4FC,0x10010D58);wr<u32>(p,0x49C,p+0x4FC);wr<u32>(p,0x498,p+0x4F0);
            wr<u32>(p,0x4F0,0x10010D68);wr<u8>(p,0x4F4,1);wr<u32>(p,0x4A8,0x10010D38);
            wr<u8>(p,0x4F6,0);wr<u8>(p,0x4FA,0); status=rd<u32>(p,0x2E4);
        }
        wr<u32>(p,0x2E4,status|8);
    }
    gabi::call(0x0216B7E0,self);
    if(self->mType==0 && gabi::call<BOOL>(0x0216B8B8,self)) return 5;
    if(!gabi::call<BOOL>(0x02520C0C,0x2D)) return 5;
    gabi::call(0x0216BA10,self); return 4;
}
VERIFY(0x0216BA8C,daGyCtrl_Create);
s32 daGyCtrl_CreateWrapper(daGyCtrl_c* self) { WWHD_FUNC(0x0216BBB0,s32,self); return gabi::call<s32>(0x0216BA8C,self); }
VERIFY(0x0216BBB0,daGyCtrl_CreateWrapper);
BOOL daGyCtrl_Delete(daGyCtrl_c* self) { WWHD_FUNC(0x0216BBB4,BOOL,self); return TRUE; }
VERIFY(0x0216BBB4,daGyCtrl_Delete);
void daGyCtrl_SetTarget(daGyCtrl_c* self) {
    WWHD_FUNC(0x0216BBBC,void,self);
    if((rd<u32>(play(),0x5CD8)&0x10000) || (rd<u32>(play(),0x5CD8)&0x1000000)) {
        if(self->mPathClear && !gabi::call<s32>(0x0211D2F8,&self->mCameraTimer)) self->mCameraTimer=30;
        self->mCameraMode=2;self->mTarget=1;
    } else {
        if(rd<u32>(play(),0x5CD8)&0x100000) {
            if(!gabi::call<s32>(0x0211D2F8,&self->mCameraTimer)) self->mCameraTimer=30;
            self->mCameraMode=1;
        } else { self->mCameraMode=0;self->mCameraTimer=30; }
        self->mTarget=0;
    }
    if(!self->mPathClear || !self->mTargetInRange) self->mCameraMode=0;
    self->mAllChildrenDead=1; u32 p=gabi::ea(self);
    for(u32 i=0;i<(u8)self->mCount;++i) {
        gabi::Local<be<u32>> id;*id=rd<u32>(p,0x47C+4*i);
        if((u32)*id!=0xFFFFFFFF) {
            u32 child=gabi::call<u32>(0x025D5218,0x025E1234,id.get());
            if(child && rd<u32>(child,0x3CC)) self->mAllChildrenDead=0;
        }
    }
    if(self->mAllChildrenDead) self->mCameraMode=0;
    else {
        s32 mode=self->mCameraMode;
        if(mode==1 || mode==2) {
            u32 camera=gabi::call<u32>(0x024F8044);
            gabi::call(0x02514EE4,camera,mode==1?STR(0x10010D98):STR(0x10010DA4),0);
        }
    }
}
VERIFY(0x0216BBBC,daGyCtrl_SetTarget);
f32 daGyCtrl_WaterY(daGyCtrl_c* self,cXyz* pos) {
    WWHD_FUNC(0x0216BDB8,f32,self,pos);pos->y=(f32)pos->y+1000.0f;return gabi::call<f32>(0x024F17D4,pos);
}
VERIFY(0x0216BDB8,daGyCtrl_WaterY);
BOOL daGyCtrl_SetPathTarget(daGyCtrl_c* self) {
    WWHD_FUNC(0x0216BDD4,BOOL,self);
    gabi::Local<cXyz> pos;
    if(self->mTarget==0) {
        copy3(gabi::ea(pos.get()),rd<u32>(play(),0x5B2C)+0x314);
        self->mAngle=(s16)((s16)self->mAngle+rd<s16>(0x10464634,0xA));self->mDesiredRadius=rd<f32>(0x10464634,0x1C);
    } else if(self->mTarget==1) {
        u32 ship=rd<u32>(play(),0x5B3C);if(ship) {
            copy3(gabi::ea(pos.get()),ship+0x314);
            self->mDesiredRadius=rd<f32>(0x10464634,0x18);self->mAngle=(s16)((s16)self->mAngle+rd<s16>(0x10464634,0xC));
        }
    }
    pos->y=gabi::call<f32>(0x0216BDB8,self,pos.get());
    if(self->mType!=0) {
        gabi::Local<cXyz> difference,flat;
        gabi::call(0x0201ADE0,&self->mCenter,difference.get(),pos.get());
        flat->x=difference->x;flat->y=0;flat->z=difference->z;
        f32 square=gabi::call<f32>(0x028E8DD0,flat.get());
        f64 distance=gabi::call<f64>(0x028F4384,square);
        if(!(distance<(f32)self->mMaxDistance)) return FALSE;
    }
    self->current.pos.x=pos->x; self->current.pos.y=pos->y; self->current.pos.z=pos->z;return TRUE;
}
VERIFY(0x0216BDD4,daGyCtrl_SetPathTarget);
BOOL daGyCtrl_Line(daGyCtrl_c* self,cXyz* start,cXyz* end) {
    WWHD_FUNC(0x0216BF50,BOOL,self,start,end);
    u32 p=gabi::ea(self);gabi::call(0x024F1AFC,p+0x498,start,end,self);
    if(!gabi::call<BOOL>(0x02008860,play()+0x12A0,p+0x498)) return FALSE;
    copy3(gabi::ea(end),p+0x4C8);return TRUE;
}
VERIFY(0x0216BF50,daGyCtrl_Line);

BOOL daGyCtrl_CheckPath(daGyCtrl_c* self) {
    WWHD_FUNC(0x0216BFE0,BOOL,self);
    if(rd<u8>(play(),0x5292)) return FALSE;
    for(u32 i=0;i<16;++i) {
        f32 radius=self->mRadius;
        gabi::call(0x028E93CC,0x1048D0CC,(f32)self->current.pos.x,(f32)self->current.pos.y,(f32)self->current.pos.z);
        gabi::call(0x025F1C28,0x1048D0CC,(s16)(i*0xFFF));gabi::call(0x025F24E0,radius,0.0f,0.0f);
        u32 path=gabi::ea(self)+0x508+i*12;gabi::Local<cXyz> start;
        wr<u32>(path,0,rd<u32>(0x1048D0CC,0xC));wr<u32>(path,4,rd<u32>(0x1048D0CC,0x1C));wr<u32>(path,8,rd<u32>(0x1048D0CC,0x2C));
        start->x=rd<f32>(path);start->y=rd<f32>(path,4)+rd<f32>(0x10464634,0x24);start->z=rd<f32>(path,8);
        wr<f32>(path,4,rd<f32>(0x1048D0CC,0x1C)+rd<f32>(0x10464634,0x28));
        if(gabi::call<BOOL>(0x0216BF50,self,start.get(),path)) return FALSE;
    }
    return TRUE;
}
VERIFY(0x0216BFE0,daGyCtrl_CheckPath);
s32 daGyCtrl_Execute(daGyCtrl_c* self) {
    WWHD_FUNC(0x0216C13C,s32,self);
    gabi::call(0x0216BBBC,self);self->mTargetInRange=gabi::call<BOOL>(0x0216BDD4,self);
    self->mPathClear=gabi::call<BOOL>(0x0216BFE0,self);gabi::call(0x0216B924,self,1,4);
    self->mRoom=rd<s8>(gabi::ea(self),0x326);return 0;
}
VERIFY(0x0216C13C,daGyCtrl_Execute);
s32 daGyCtrl_ExecuteWrapper(daGyCtrl_c* self) { WWHD_FUNC(0x0216C1A0,s32,self);return gabi::call<s32>(0x0216C13C,self); }
VERIFY(0x0216C1A0,daGyCtrl_ExecuteWrapper);
BOOL daGyCtrl_Draw(daGyCtrl_c* self) {
    WWHD_FUNC(0x0216C1A4,BOOL,self);
    if(rd<u8>(0x10464634,4)) { for(u32 i=0;i<(u8)self->mCount;++i) {} for(u32 i=0;i<16;++i) {} }
    return TRUE;
}
VERIFY(0x0216C1A4,daGyCtrl_Draw);
BOOL daGyCtrl_DrawWrapper(daGyCtrl_c* self) { WWHD_FUNC(0x0216C1DC,BOOL,self);return gabi::call<BOOL>(0x0216C1A4,self); }
VERIFY(0x0216C1DC,daGyCtrl_DrawWrapper);
void daGyCtrl_ModeSwitchWait(daGyCtrl_c* self) {
    WWHD_FUNC(0x0216C1E0,void,self);
    s32 room=self->mType==1?rd<s8>(gabi::ea(self),0x326):(s32)self->mRoom;
    u32 save=rd<u32>(0x101F84DC);u8 sw=self->mSwitch;
    if(gabi::call<BOOL>(0x025BA0C0,save+0x20,sw,room)) gabi::call(0x0216B924,self,0,1);
}
VERIFY(0x0216C1E0,daGyCtrl_ModeSwitchWait);
void daGyCtrl_ModeCreateInit(daGyCtrl_c* self) { WWHD_FUNC(0x0216C26C,void,self);self->mCreateTimer=rd<s16>(0x10464634,6); }
VERIFY(0x0216C26C,daGyCtrl_ModeCreateInit);
void daGyCtrl_SetPathPos(daGyCtrl_c* self) {
    WWHD_FUNC(0x0216C27C,void,self);
    u32 p=gabi::ea(self);
    for(u32 i=0;i<(u8)self->mCount;++i) {
        u32 count=(u8)self->mCount;
        s16 angle=(s16)((s16)self->mAngle+(65535/count)*i);wr<s16>(p,0x424+i*2,angle);
        f32 radius=gabi::fmadds(rd<f32>(0x104A44F8+((u16)angle>>3)*8),rd<f32>(0x10464634,0x10),(f32)self->mRadius);
        s16 checkAngle=(s16)(angle+0x500);
        gabi::call(0x028E93CC,0x1048D0CC,(f32)self->current.pos.x,(f32)self->current.pos.y,(f32)self->current.pos.z);
        gabi::call(0x025F1C28,0x1048D0CC,angle);gabi::call(0x025F24E0,radius,0.0f,0.0f);
        u32 spawn=p+0x3AC+i*12,check=p+0x3E8+i*12;
        wr<u32>(spawn,0,rd<u32>(0x1048D0CC,0xC));wr<u32>(spawn,4,rd<u32>(0x1048D0CC,0x1C));wr<u32>(spawn,8,rd<u32>(0x1048D0CC,0x2C));
        wr<f32>(spawn,4,gabi::call<f32>(0x0216BDB8,self,spawn));
        f32 height=rd<f32>(0x10464634,0x28);
        gabi::call(0x028E93CC,0x1048D0CC,(f32)self->current.pos.x,(f32)self->current.pos.y,(f32)self->current.pos.z);
        gabi::call(0x025F1C28,0x1048D0CC,checkAngle);gabi::call(0x025F24E0,radius,0.0f,height);
        wr<u32>(check,0,rd<u32>(0x1048D0CC,0xC));wr<u32>(check,4,rd<u32>(0x1048D0CC,0x1C));wr<u32>(check,8,rd<u32>(0x1048D0CC,0x2C));
        gabi::Local<cXyz> start;start->x=rd<f32>(check);start->y=rd<f32>(check,4);start->z=rd<f32>(check,8);
        start->y=(f32)start->y+rd<f32>(0x10464634,0x24);
        wr<f32>(check,4,gabi::call<f32>(0x0216BDB8,self,check));
        wr<u8>(p,0x42E + i,(u8)(gabi::call<u32>(0x0216BF50,self,start.get(),check)^1));
    }
    gabi::call(0x0200ED84,&self->mRadius,(f32)self->mDesiredRadius,0.1f,rd<f32>(0x10464634,0x2C));
}
VERIFY(0x0216C27C,daGyCtrl_SetPathPos);
static bool mustHide(daGyCtrl_c* self) {
    return (self->mType==1 && !self->mTargetInRange) || !self->mPathClear || gabi::call<u32>(0x025D5218,0x0216B7B4,self)!=0;
}
void daGyCtrl_ModeCreate(daGyCtrl_c* self) {
    WWHD_FUNC(0x0216C4A8,void,self);
    if(mustHide(self)) { gabi::call(0x0216B924,self,0,3);return; }
    s32 index=self->mSpawnIndex;u32 count=(u8)self->mCount,p=gabi::ea(self);
    bool pending=index<(s32)count;
    if(pending && !gabi::call<s32>(0x0211D2F8,&self->mCreateTimer)) {
        index=self->mSpawnIndex;gabi::Local<cXyz> pos;u32 path=p+0x3AC+(u32)index*12;
        pos->x=rd<f32>(path);pos->y=rd<f32>(path,4);pos->z=rd<f32>(path,8);
        gabi::Local<csXyz> angles;gabi::call(0x0201A478,angles.get(),0,0,0);
        s32 room=rd<s8>(p,0x1C9);index=self->mSpawnIndex;pos->y=-1000.0f;
        u32 parent=rd<u32>(p,4);angles->y=rd<s16>(p,0x424+(u32)index*2);
        u32 destination=p+0x47C+(u32)index*4;
        wr<u32>(destination,0,gabi::call<u32>(0x025D5A20,0xE4,parent,-1,pos.get(),room,angles.get(),0,-1,0));
        index=self->mSpawnIndex;wr<u8>(p,0x490+(u32)index,1);self->mCreateTimer=rd<s16>(0x10464634,8);
    }
    if(pending) { count=(u8)self->mCount;index=self->mSpawnIndex; }
    if((u32)index==count) gabi::call(0x0216B924,self,0,2);
    gabi::call(0x0216C27C,self);
}
VERIFY(0x0216C4A8,daGyCtrl_ModeCreate);
void daGyCtrl_DeadCheck(daGyCtrl_c* self) {
    WWHD_FUNC(0x0216C634,void,self);
    u32 count=(u8)self->mCount,dead=0,p=gabi::ea(self);
    for(u32 i=0;i<count;++i) {
        if(!rd<u8>(p,0x490+i)) continue;
        gabi::Local<be<u32>> id;*id=rd<u32>(p,0x47C+4*i);
        if((u32)*id==0xFFFFFFFF) ++dead;
        else {u32 child=gabi::call<u32>(0x025D5218,0x025E1234,id.get());count=(u8)self->mCount;if(!child) ++dead;}
    }
    if(dead==count) gabi::call(0x025D57E0,self);
}
VERIFY(0x0216C634,daGyCtrl_DeadCheck);
void daGyCtrl_ModeWait(daGyCtrl_c* self) {
    WWHD_FUNC(0x0216C6E8,void,self);
    if(mustHide(self)) { gabi::call(0x0216B924,self,0,3);return; }
    gabi::call(0x0216C634,self);gabi::call(0x0216C27C,self);
}
VERIFY(0x0216C6E8,daGyCtrl_ModeWait);
void daGyCtrl_ModeHideInit(daGyCtrl_c* self) { WWHD_FUNC(0x0216C780,void,self);self->mHideTimer=rd<s16>(0x1047BB18)+30; }
VERIFY(0x0216C780,daGyCtrl_ModeHideInit);
void daGyCtrl_ModeHide(daGyCtrl_c* self) {
    WWHD_FUNC(0x0216C794,void,self);
    bool ready=(self->mType==1?self->mTargetInRange!=0:self->mType==0) && self->mPathClear!=0;
    if(gabi::call<s32>(0x0211D2F8,&self->mHideTimer)) return;
    if(gabi::call<u32>(0x025D5218,0x0216B7B4,self)) {self->mHideTimer=rd<s16>(0x1047BB1A)+30;return;}
    if(ready && self->mPathClear) gabi::call(0x0216B924,self,0,1);
}
VERIFY(0x0216C794,daGyCtrl_ModeHide);
BOOL daGyCtrl_IsDelete(daGyCtrl_c* self) { WWHD_FUNC(0x0216C9C8,BOOL,self);return TRUE; }
VERIFY(0x0216C9C8,daGyCtrl_IsDelete);
void daGyCtrl_ModeSwitchWaitInit(daGyCtrl_c* self) { WWHD_FUNC(0x0216C9D0,void,self); }
VERIFY(0x0216C9D0,daGyCtrl_ModeSwitchWaitInit);
void daGyCtrl_ModeWaitInit(daGyCtrl_c* self) { WWHD_FUNC(0x0216C9D4,void,self); }
VERIFY(0x0216C9D4,daGyCtrl_ModeWaitInit);
void daGyCtrl_Destructor(daGyCtrl_c* self,u32 flags) {
    WWHD_FUNC(0x0216C9D8,void,self,flags);if(!self) return;u32 p=gabi::ea(self);
    wr<u32>(p,0x4F0,0x10010D28);wr<u32>(p,0x4FC,0x10010CE8);wr<u32>(p,0x4B8,0x10010CD8);
    gabi::call(0x02008B4C,p+0x498,0);gabi::call(0x025D50BC,self,0);
    if(flags&1) gabi::call(0x0273AF40,self);
}
VERIFY(0x0216C9D8,daGyCtrl_Destructor);

void* daGyCtrl_HioConstructor(void* self) {
    WWHD_FUNC(0x0216C870,void*,self);
    if(!self) self=gabi::call<void*>(0x0273AD10,0x30);
    if(!self) return nullptr;u32 p=gabi::ea(self);
    wr<f32>(p,0x28,-500.0f);wr<s16>(p,0xC,250);wr<f32>(p,0x1C,700.0f);wr<f32>(p,0x10,10.0f);
    wr<s16>(p,8,240);wr<f32>(p,0x18,1000.0f);wr<f32>(p,0x20,10000.0f);wr<s16>(p,0xA,200);
    wr<s16>(p,6,100);wr<u8>(p,4,0);wr<f32>(p,0x2C,10.0f);wr<u8>(p,5,0);wr<f32>(p,0x24,500.0f);
    wr<u32>(p,0,0x10010D88);return self;
}
VERIFY(0x0216C870,daGyCtrl_HioConstructor);
void daGyCtrl_StaticInit() {
    WWHD_FUNC(0x0216C928,void);
    wr<u32>(0x10464664,8,0);wr<u32>(0x10464664,0,0);wr<u32>(0x10464664,0xC,0);wr<u32>(0x10464664,4,0);
    gabi::call(0x028F026C,0x101B6D70);
    wr<f32>(0x10464628,0,-3.1415927410125732f);wr<f32>(0x1046462C,0,3.1415927410125732f);
    gabi::call(0x028ED6F8,0x10464630);gabi::call(0x028F026C,0x101B6D7C);
    gabi::call(0x028EAB2C,0x10464631);gabi::call(0x028F026C,0x101B6D88);gabi::call(0x0216C870,0x10464634);
}
VERIFY(0x0216C928,daGyCtrl_StaticInit);
