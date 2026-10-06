/** WWHD salvage actor, reconstructed from GameCube source and WWHD disassembly.
 * See wwhd_src/README.md. */
#include "bindings.h"
#include "d/actor/d_a_salvage.h"
namespace {
inline u32 registry() { return gabi::load<u32>(0x10475634); }
inline s32 salvageId() { return gabi::load<s32>(0x10475638); }
inline u32 save() { return gabi::load<u32>(0x101F84DC); }
inline void setProc(daSalvage_c* self, u32 addr) {
    self->mProcAdjustment = 0; self->mProcIndex = -1; self->mProc = addr;
}
}
static void debug_print2() { WWHD_FUNC(0x02467D30, void, (u32)0); }
VERIFY(0x02467D30, debug_print2);
static BOOL daSalvageIsDelete(void* self) { WWHD_FUNC(0x02467CCC, BOOL, self); return TRUE; }
VERIFY(0x02467CCC, daSalvageIsDelete);
static BOOL proc_salvage(void* self) { WWHD_FUNC(0x02467CD4, BOOL, self); return TRUE; }
VERIFY(0x02467CD4, proc_salvage);
static cPhs_State salvage_createCB(fopAc_ac_c* self) {
    WWHD_FUNC(0x02465900, cPhs_State, self);
    gabi::store<u32>(gabi::ea(self)+0x2E0,gabi::load<u32>(gabi::ea(self)+0x2E0)|0x8000);
    return cPhs_COMPLEATE_e;
}
VERIFY(0x02465900, salvage_createCB);
static BOOL proc_wait_init(daSalvage_c* self) {
    WWHD_FUNC(0x02465E9C, BOOL, self);
    setProc(self, 0x02467364); self->mChestId = 0xFFFFFFFF; return TRUE;
}
VERIFY(0x02465E9C, proc_wait_init);
static BOOL proc_salvage_init(daSalvage_c* self) {
    WWHD_FUNC(0x024679A0, BOOL, self);
    setProc(self,0x02467CD4);
    s32 id=salvageId(); u32 reg=registry(); gabi::call(0x025B4B4C,reg,id,1); return TRUE;
}
VERIFY(0x024679A0, proc_salvage_init);
static BOOL isEffectKind(daSalvage_c* self, s32 id) {
    WWHD_FUNC(0x02465A48, BOOL, self,id);
    u8 kind=gabi::call<u8>(0x025B4CC8,registry(),id);
    return kind<=4 || kind==6;
}
VERIFY(0x02465A48,isEffectKind);
static BOOL getDistance(daSalvage_c* self,s32 id,be<f32>* dist) {
    WWHD_FUNC(0x024664F4, BOOL,self,id,dist);
    if (!gabi::call<BOOL>(0x025B48BC,registry(),id)) return FALSE;
    *dist=gabi::call<f32>(0x025B4CFC,registry(),id); return TRUE;
}
VERIFY(0x024664F4,getDistance);
static void createEnemy(daSalvage_c* self) {
    WWHD_FUNC(0x02465E54,void,self);
    u32 play=gabi::call<u32>(0x025200D4);
    u32 player=gabi::load<u32>(play+0x5B2C);
    gabi::call(0x025D5834,0xE3,2,player+0x314,-1,0,0,-1,0);
}
VERIFY(0x02465E54,createEnemy);
static BOOL end_salvage(daSalvage_c* self) {
    WWHD_FUNC(0x02465D30,BOOL,self);
    if(!gabi::load<u8>(0x1046D81A)) {
        u8 room=(u8)gabi::call<s8>(0x025B4C74,registry(),salvageId());
        u8 kind=gabi::call<u8>(0x025B4CC8,registry(),salvageId());
        u8 slot=gabi::call<u8>(0x025B4CA8,registry(),salvageId());
        if(kind==0) {
            gabi::call(0x025B8140,save()+0xE4,(s32)slot-1);
            gabi::call(0x025B8B68,save()+0x644,0x3E02);
        } else if(kind>=2 && kind<=4) gabi::call(0x025B89B4,save()+0x5E0,room,(u16)slot);
        else if(kind==6) {
            u16 bit=gabi::load<u16>(0x1004BD1C+2*slot);
            gabi::call(0x025B8B68,save()+0x644,bit);
        }
        if(kind!=6) gabi::call(0x025B4B14,registry(),salvageId());
        gabi::store<s32>(0x10475638,-1);
    }
    return TRUE;
}
VERIFY(0x02465D30,end_salvage);
static void eventOrder(daSalvage_c* self) {
    WWHD_FUNC(0x024662F0,void,self);
    u32 play=gabi::call<u32>(0x025200D4);
    if(!(gabi::load<u32>(play+0x5CD8)&0x10000)) return;
    u8 order=self->mOrder; s16 event;
    if(order==2) event=self->mGetItemEvent;
    else if(order==1) event=self->mGetItemLeftEvent;
    else if(order==4) event=self->mHazureEvent;
    else if(order==3) event=self->mHazureLeftEvent;
    else return;
    gabi::call(0x025D7A58,self,event,0xFF,0xFFFF,0,1);
    gabi::store<u16>(gabi::ea(self)+0xFA,gabi::load<u16>(gabi::ea(self)+0xFA)|2);
}
VERIFY(0x024662F0,eventOrder);
static void colorFromGXColor(be<f32>* out,be<u8>* in) {
    WWHD_FUNC(0x02466874,void,out,in);
    f32 r=(f32)(u8)in[0]/255.0f,g=(f32)(u8)in[1]/255.0f;
    f32 b=(f32)(u8)in[2]/255.0f,a=(f32)(u8)in[3]/255.0f;
    out[0]=r; out[1]=g;out[2]=b;out[3]=a;
}
VERIFY(0x02466874,colorFromGXColor);
static void daSalvage_HIO_c_dt(void* self,s32 flags) {
    WWHD_FUNC(0x02467CB8,void,self,flags); if(self && (flags&1)) operator_delete(self);
}
VERIFY(0x02467CB8,daSalvage_HIO_c_dt);
static void daSalvage_c_dt(daSalvage_c* self,s32 flags) {
    WWHD_FUNC(0x02467CDC,void,self,flags);
    if(self) { gabi::call(0x025D50BC,self,0); if(flags&1) operator_delete(self); }
}
VERIFY(0x02467CDC,daSalvage_c_dt);
static void* daSalvage_HIO_c_ct(void* self) {
    WWHD_FUNC(0x02467B50,void*,self);
    if(!self) {self=gabi::call<void*>(0x0273AD10,0x34); if(!self) return nullptr;}
    u32 p=gabi::ea(self);
    gabi::store<f32>(p+0x2C,80);gabi::store<f32>(p+0x20,15);gabi::store<f32>(p+0x1C,250);
    gabi::store<u8>(p+0x16,0);gabi::store<u8>(p+0x17,0);gabi::store<u8>(p+0x18,0);
    gabi::store<s8>(p+4,-1);gabi::store<u8>(p+0x31,0);gabi::store<u8>(p+0x19,255);
    gabi::store<u8>(p+0x15,0);gabi::store<u8>(p+0x14,0);gabi::store<u32>(p+8,0);
    gabi::store<u8>(p+0x1A,0);gabi::store<u8>(p+0x30,20);gabi::store<f32>(p+0x24,20);
    gabi::store<f32>(p+0x10,1000);gabi::store<f32>(p+0x28,10);gabi::store<u32>(p,0x10039AA0);
    gabi::store<f32>(p+0xC,1000);return self;
}
VERIFY(0x02467B50,daSalvage_HIO_c_ct);
static void __sinit_d_a_salvage_cpp() {
    WWHD_FUNC(0x02467C18,void,(u32)0);
    sinit_header_statics(0x1046D7E4,0x101CFEB0);
    gabi::call(0x02467B50,0x1046D800);
}
VERIFY(0x02467C18,__sinit_d_a_salvage_cpp);
static BOOL daSalvage_delete(daSalvage_c* self) {
    WWHD_FUNC(0x02465C68,BOOL,self);
    if(self->mInitialized) {
        u32 emitter=gabi::ea((void*)self->mEmitter);
        if(emitter) {
            u32 flags=gabi::load<u32>(emitter+0x254);
            gabi::store<s32>(emitter+0x5C,-1);gabi::store<u32>(emitter+0x254,flags|1);
            self->mEmitter=0;
        }
        for(s32 i=0;i<160;i++) if(isEffectKind(self,i) && gabi::call<BOOL>(0x025B4D0C,registry(),i)) {
            u32 pos=gabi::call<u32>(0x025B4BE0,registry(),i);gabi::call(0x025E1B34,pos);
        }
        u32 reg=registry(); if(reg) {gabi::call(0x025B4A98,reg);gabi::store<u32>(0x10475634,0);}
    }
    return TRUE;
}
VERIFY(0x02465C68,daSalvage_delete);
static s32 checkXZDistance(daSalvage_c* self) {
    WWHD_FUNC(0x02467290,s32,self);
    u32 play=gabi::call<u32>(0x025200D4);u32 ship=gabi::load<u32>(play+0x5B3C);
    if(!ship || !gabi::load<u32>(ship+0x71C)) return -1;
    for(s32 i=0;i<160;i++) {
        if(!gabi::call<BOOL>(0x025B48BC,registry(),i)) continue;
        if(!gabi::call<BOOL>(0x025B4D0C,registry(),i)) continue;
        gabi::Local<be<f32>> distance;
        if(!getDistance(self,i,distance.get())) continue;
        f32 radius=gabi::call<f32>(0x025B4C54,registry(),i);
        if((f32)*distance.get()<radius) return i;
    }
    return -1;
}
VERIFY(0x02467290,checkXZDistance);
static BOOL onSalvageForOship(daSalvage_c* self,void* other) {
    WWHD_FUNC(0x024679F4,BOOL,self,other);
    u32 ship=gabi::ea(other);
    if(!ship || gabi::load<u8>(ship+0x3CD)==255) return FALSE;
    for(s32 i=128;i<160;i++) {
        if(!gabi::call<BOOL>(0x025B48BC,registry(),i)) continue;
        s32 sw=gabi::call<s32>(0x025B4C98,registry(),i);
        u8 slot=gabi::call<u8>(0x025B4CA8,registry(),i);
        u8 room=(u8)gabi::call<s8>(0x025B4C74,registry(),i);
        gabi::Local<cXyz> ignored;gabi::call(0x025B4B7C,registry(),ignored.get(),i);
        gabi::call<u8>(0x025B4CC8,registry(),i);
        if(sw==255 || (u32)sw!=gabi::load<u8>(ship+0x3CD)) continue;
        if(gabi::call<BOOL>(0x025B8A50,save()+0x5E0,room,slot)) continue;
        gabi::Local<cXyz> pos;gabi::call(0x025B4B7C,registry(),pos.get(),i);
        f32 y=pos->y;f32 x=gabi::load<f32>(ship+0x314);f32 z=gabi::load<f32>(ship+0x31C);
        gabi::Local<cXyz> moved;moved->x=x;moved->y=y;moved->z=z;
        gabi::call(0x025B4998,registry(),i,moved.get());
        gabi::call(0x025B49BC,registry(),i,255);self->mFadeDelay=90;return TRUE;
    }
    return FALSE;
}
VERIFY(0x024679F4,onSalvageForOship);
static s32 checkDistance(daSalvage_c* self) {
    WWHD_FUNC(0x02467124,s32,self);
    u32 play=gabi::call<u32>(0x025200D4),ship=gabi::load<u32>(play+0x5B3C);
    if(!ship) return -1;u32 top=gabi::load<u32>(ship+0x71C);
    if(!top || gabi::load<u8>(ship+0x635)!=10 || gabi::load<u8>(ship+0x636)!=10) return -1;
    if(!(gabi::load<f32>(ship+0x370)<1.0f) || (gabi::load<u32>(ship+0x644)&1) || gabi::load<u32>(ship+0x704) || gabi::load<u32>(ship+0x70C)) return -1;
    s32 found=-1;
    for(s32 i=0;i<160;i++) {
        if(!gabi::call<BOOL>(0x025B48BC,registry(),i) || !gabi::call<BOOL>(0x025B4D0C,registry(),i)) continue;
        gabi::Local<cXyz> crane;crane->x=gabi::load<f32>(top);crane->y=gabi::load<f32>(top+4);crane->z=gabi::load<f32>(top+8);
        if(gabi::call<BOOL>(0x02466F88,self,crane.get(),i)) {
            if(gabi::call<u8>(0x025B4CC8,registry(),i)==5) found=i;else return i;
        }
    }
    return found;
}
VERIFY(0x02467124,checkDistance);
static BOOL CreateInit(daSalvage_c* self) {
    WWHD_FUNC(0x02465914,BOOL,self);
    s8 room=self->current.roomNo;gabi::store<s8>(0x10475654,room);
    setProc(self,0x02465E9C);self->mPreviousRoom=room;
    fopAcM_setStageLayer(self);self->mInitialized=1;
    const u32 names[]={0x100399C4,0x100399E8,0x100399D4,0x100399F8};
    be<s16>* events[]={&self->mGetItemEvent,&self->mHazureEvent,&self->mGetItemLeftEvent,&self->mHazureLeftEvent};
    for(int i=0;i<4;i++) {
        u32 play=gabi::call<u32>(0x025200D4);
        *events[i]=gabi::call<s16>(0x02543F10,play+0x52C4,names[i],255);
    }
    u32 play=gabi::call<u32>(0x025200D4),particles=gabi::load<u32>(play+0x5AB0);
    u32 emitter=gabi::call<u32>(0x025A847C,particles,0,0x437,&self->current.pos,0,0,255,0,-1,0,0,0);
    self->mEmitter=gabi::at<void>(emitter);if(emitter) gabi::store<u32>(emitter+0x254,gabi::load<u32>(emitter+0x254)|1);
    return TRUE;
}
VERIFY(0x02465914,CreateInit);
static cPhs_State daSalvage_create(daSalvage_c* self) {
    WWHD_FUNC(0x02465A8C,cPhs_State,self);
    if(!fopAcM_CheckCondition(self,fopAcCnd_INIT_e)) {
        if(self) {fopAc_ac_c_ct(self);self->__vtbl=0x10039974;}
        fopAcM_OnCondition(self,fopAcCnd_INIT_e);
    }
    cPhs_State result=gabi::call<cPhs_State>(0x02520460,&self->mPhase,gabi::load<u32>(0x101CFED4));
    if(result!=cPhs_COMPLEATE_e) return result;
    u32 reg=registry();
    if(!reg) {
        if(!gabi::call<BOOL>(0x025D63E8,self,0x024658FC,0xB000)) return cPhs_ERROR_e;
        CreateInit(self);reg=registry();
        if(!reg) {gabi::call(0x0273AA24,0x10039A0C,0x1DE,0x10039A1C);reg=registry();}
        gabi::call(0x025B44A0,reg,self,0);return cPhs_COMPLEATE_e;
    }
    s8 current=self->current.roomNo,old=gabi::load<s8>(0x10475654);
    if(old!=current && current) {
        for(s32 i=128;i<160;i++) if(gabi::call<BOOL>(0x025B48BC,registry(),i) && isEffectKind(self,i) && gabi::call<BOOL>(0x025B4D0C,registry(),i)) {
            u32 pos=gabi::call<u32>(0x025B4BE0,registry(),i);gabi::call(0x025E1B34,pos);
        }
        gabi::call(0x025B4AD8,registry(),old);
        self->mPreviousRoom=gabi::load<s8>(0x10475654);
        current=self->current.roomNo;reg=registry();gabi::store<s8>(0x10475654,current);
    }
    gabi::call(0x025B44A0,reg,self,0);return cPhs_ERROR_e;
}
VERIFY(0x02465A8C,daSalvage_create);
static void send_agb(daSalvage_c* self) {
    WWHD_FUNC(0x02466BD0,void,self);
    gabi::call(0x025DA088,self,0x25,0x40,0x3F);u8 count=0;
    for(s32 i=0;i<160;i++) {
        if(gabi::call<u8>(0x025B4CC8,registry(),i)==5 || !gabi::call<u8>(0x025B4CC8,registry(),i)) continue;
        if(!gabi::call<BOOL>(0x025B48BC,registry(),i) || !gabi::call<BOOL>(0x025B4D0C,registry(),i)) continue;
        s8 room=gabi::call<s8>(0x025B4C74,registry(),i);
        if(room!=gabi::load<s8>(0x1047E6C8)) continue;
        gabi::Local<cXyz> pos;gabi::call(0x025B4B7C,registry(),pos.get(),i);
        f32 z=pos->z,x=pos->x,y=pos->y;
        room=gabi::call<s8>(0x025B4C74,registry(),i);
        u8 name=gabi::load<u8>(gabi::ea(self)+0x2DE);
        gabi::call(0x02590488,0x10,x,y,z,room,-32768,count,name,0);count++;
    }
}
VERIFY(0x02466BD0,send_agb);
static void calcAlpha(daSalvage_c* self) {
    WWHD_FUNC(0x02466578,void,self);
    u32 play=gabi::call<u32>(0x025200D4),player=gabi::load<u32>(play+0x5B2C);
    f32 x=gabi::load<f32>(player+0x314),z=gabi::load<f32>(player+0x31C);
    BOOL sea=gabi::call<BOOL>(0x0246B6A4,x,z);
    u8 frames=gabi::load<u8>(0x1046D830);
    u8 step=frames?255/frames:0;
    f32 cmap=gabi::load<f32>(0x1046D82C),near=gabi::load<f32>(sea?0x1046D824:0x1046D828);
    gabi::Local<be<f32>> distance;
    for(s32 i=0;i<160;i++) {
        if(!gabi::call<BOOL>(0x025B48BC,registry(),i)) continue;
        BOOL valid=getDistance(self,i,distance.get());
        u8 kind=gabi::call<u8>(0x025B4CC8,registry(),i);f32 minimum=kind==0?cmap:near;
        BOOL used=gabi::call<BOOL>(0x025B4D0C,registry(),i);
        f32 dist=*distance.get();
        if(used && valid && !(dist>25000) && !(dist<minimum*100)) {
            play=gabi::call<u32>(0x025200D4);
            if(!gabi::load<u8>(play+0x5292)) {s32 delay=self->mFadeDelay;if(delay>0) self->mFadeDelay=delay-1;}
            kind=gabi::call<u8>(0x025B4CC8,registry(),i);
            if(kind==2 && self->mFadeDelay!=0) continue;
            u32 alpha=gabi::call<u32>(0x025B4CD8,registry(),i);
            gabi::call<BOOL>(0x0200F4FC,alpha,255,step);
            gabi::call(0x025B49CC,registry(),i,1);
            if(!gabi::call<u8>(0x025B4CC8,registry(),i)) {
                u32 emitter=gabi::ea((void*)self->mEmitter);
                if(emitter) {
                    gabi::store<u32>(emitter+0x254,gabi::load<u32>(emitter+0x254)&~1U);
                    gabi::Local<cXyz> pos;emitter=gabi::ea((void*)self->mEmitter);
                    gabi::call(0x025B4B7C,registry(),pos.get(),i);
                    f32 px=pos->x,py=pos->y,pz=pos->z;u8 plane=gabi::load<u8>(emitter+0x262);
                    gabi::store<f32>(emitter+0x22C,px);gabi::store<f32>(emitter+0x230,py);gabi::store<f32>(emitter+0x234,pz);
                    if(plane>=7) gabi::store<f32>(emitter+0x230,-gabi::load<f32>(emitter+0x230));
                }
            }
        } else {
            u32 alpha=gabi::call<u32>(0x025B4CD8,registry(),i);
            BOOL done=gabi::call<BOOL>(0x0200F4FC,alpha,0,step);
            gabi::call(0x025B49CC,registry(),i,done?0:1);
            if(!gabi::call<u8>(0x025B4CC8,registry(),i)) {
                u32 emitter=gabi::ea((void*)self->mEmitter);
                if(emitter) gabi::store<u32>(emitter+0x254,gabi::load<u32>(emitter+0x254)|1);
            }
        }
    }
}
VERIFY(0x02466578,calcAlpha);
static BOOL checkArea(daSalvage_c* self,cXyz* crane,s32 id) {
    WWHD_FUNC(0x02466F88,BOOL,self,crane,id);
    gabi::call<f32>(0x025B4C64,registry(),id);
    f32 radius=gabi::call<f32>(0x025B4C54,registry(),id);
    gabi::Local<cXyz> pos;gabi::call(0x025B4B7C,registry(),pos.get(),id);
    gabi::Local<cXyz> delta;gabi::call(0x0201ADE0,crane,delta.get(),pos.get());
    gabi::Local<cXyz> flat;flat->x=delta->x;flat->y=0;flat->z=delta->z;
    f32 squared=gabi::call<f32>(0x028E8DD0,flat.get());
    f32 distance=gabi::call<f32>(0x028F4384,squared);
    if(!(distance<radius)) return FALSE;
    u32 play=gabi::call<u32>(0x025200D4),ship=gabi::load<u32>(play+0x5B3C);
    f32 z=crane->z,x=crane->x;
    BOOL sea=gabi::call<BOOL>(0x0246B6A4,x,z);x=crane->x;
    f32 water;
    if(sea) {z=crane->z;water=gabi::call<f32>(0x0246BA0C,x,z);}
    else {
        f32 y=gabi::load<f32>(ship+0x318)+100.0f;z=crane->z;
        gabi::Local<cXyz> probe;probe->x=x;probe->y=y;probe->z=z;
        water=gabi::call<f32>(0x024F1478,probe.get());
    }
    u8 depth=self->mRndDepthIdx;
    f32 bottom=water-gabi::load<f32>(0x10039A5C+depth*4);
    return (f32)crane->y<bottom;
}
VERIFY(0x02466F88,checkArea);
static BOOL daSalvage_execute(daSalvage_c* self) {
    WWHD_FUNC(0x02466408,BOOL,self);
    if(gabi::load<u8>(0x1046D818)) for(s32 i=0;i<160;i++) gabi::call(0x025B4B64,registry(),i,1);
    gabi::call(0x025B48DC,registry());gabi::call(0x02465EC4,self);
    s16 index=self->mProcIndex;
    if(index) {
        s16 adjustment=self->mProcAdjustment;u32 adjusted=gabi::ea(self)+adjustment;
        if(index<0) gabi::call_ptr((u32)self->mProc,adjusted);
        else {
            s16 off=gabi::load<s16>(gabi::ea(self)+0x42A);
            u32 table=gabi::load<u32>(adjusted+off);
            gabi::call_ptr(gabi::load<u32>(table+index*8+4),adjusted);
        }
    }
    eventOrder(self);self->mStayNo=gabi::load<s8>(0x1047E6C8);return TRUE;
}
VERIFY(0x02466408,daSalvage_execute);
static void set_mtx(daSalvage_c* self,J3DModel* model,s32 id) {
    WWHD_FUNC(0x02466928,void,self,model,id);
    gabi::Local<cXyz> pos;gabi::call(0x025B4B7C,registry(),pos.get(),id);
    gabi::Local<csXyz> rot;rot->x=0;rot->y=0;rot->z=0;
    f32 x=pos->x,z=pos->z;
    if(gabi::call<BOOL>(0x0246B6A4,x,z)) {
        f32 y1=gabi::call<f32>(0x0246BA0C,(f32)pos->x-50.0f,(f32)pos->z-50.0f);
        f32 y2=gabi::call<f32>(0x0246BA0C,(f32)pos->x-50.0f,(f32)pos->z+50.0f);
        f32 y3=gabi::call<f32>(0x0246BA0C,(f32)pos->x+50.0f,(f32)pos->z-50.0f);
        x=pos->x;z=pos->z;
        gabi::Local<cXyz> a,b,c;a->x=x-50;a->y=y1;a->z=z-50;
        b->x=x-50;b->y=y2;b->z=z+50;c->x=x+50;c->y=y3;c->z=z-50;
        struct Triangle_l {u8 bytes[0x38];};gabi::Local<Triangle_l> tri;
        gabi::call(0x020190B8,tri.get(),a.get(),b.get(),c.get());
        gabi::call(0x02017264,tri.get(),&rot->x,&rot->z);
        u32 emitter=gabi::ea((void*)self->mEmitter);
        if(emitter) {s16 rx=rot->x,rz=rot->z,ry=rot->y;gabi::call(0x028245AC,rx,ry,rz,emitter+0x1F0);}
        x=pos->x;z=pos->z;pos->y=gabi::call<f32>(0x0246BA0C,x,z);
    } else {
        f32 water=gabi::call<f32>(0x024F1478,pos.get());if(water!=-1e9f)pos->y=water;
    }
    gabi::Local<cXyz> moved,scale;
    f32 y=pos->y;x=pos->x;z=pos->z;moved->x=x;moved->y=y;moved->z=z;
    scale->x=x;scale->y=y;scale->z=z;
    gabi::call(0x025B4998,registry(),id,moved.get());
    gabi::call(0x025B4BF0,registry(),scale.get(),id);
    f32 sx=scale->x,sy=scale->y,sz=scale->z;
    z=pos->z;y=pos->y;x=pos->x;
    u32 m=gabi::ea(model);gabi::store<f32>(m+0xC4,sz);gabi::store<f32>(m+0xBC,sx);gabi::store<f32>(m+0xC0,sy);
    gabi::call(0x028E93CC,0x1048D0CC,x,y,z);
    s16 rz=rot->z,rx=rot->x,ry=rot->y;gabi::call(0x025F19F8,0x1048D0CC,rx,ry,rz);
    J3DModel_setBaseTRMtx(model,gabi::at<Mtx34>(0x1048D0CC));
}
VERIFY(0x02466928,set_mtx);
static BOOL CreateHeap(daSalvage_c* self) {
    WWHD_FUNC(0x024656EC,BOOL,self);
    u32 reg=gabi::call<u32>(0x0273AD10,0x2304);gabi::store<u32>(0x10475634,reg);
    if(!reg)return FALSE;gabi::call(0x025B49DC,reg);
    auto resource=[](s32 index) {
        u32 control=gabi::load<u32>(0x101F4F28),name=gabi::load<u32>(0x101CFED4);
        struct SafeString_l {be<u32> text,vt;};gabi::Local<SafeString_l> key;key->text=name;key->vt=0x100398DC;
        return gabi::call<u32>(0x026066C4,control,key.get(),index);
    };
    u32 data=resource(5);self->mModelData=gabi::at<void>(data);
    if(!data) {gabi::call(0x0273AA24,0x10039988,0x156,0x100399B0);data=gabi::ea((void*)self->mModelData);}
    for(int i=0;i<16;i++) {
        u32 model=gabi::call<u32>(0x025E38E0,data,0x80000,0x11000222);self->mModels[i]=gabi::at<void>(model);
        if(!model)return FALSE;if(i!=15)data=gabi::ea((void*)self->mModelData);
    }
    u32 animation=resource(8);
    if(!animation)gabi::call(0x0273AA24,0x10039988,0x16B,0x10039998);
    u32 brk=gabi::call<u32>(0x025E80D0,0);self->mpBrk=gabi::at<void>(brk);if(!brk)return FALSE;
    data=gabi::ea((void*)self->mModelData);
    if(!gabi::call<BOOL>(0x025E8154,brk,data,animation,1,2,1.0f,0,-1,0,0))return FALSE;
    animation=resource(11);
    if(!animation)gabi::call(0x0273AA24,0x10039988,0x179,0x100399A4);
    u32 btk=gabi::call<u32>(0x025E7C6C,0);self->mpBtk=gabi::at<void>(btk);if(!btk)return FALSE;
    data=gabi::ea((void*)self->mModelData);
    return gabi::call<BOOL>(0x025E7CE0,btk,data,animation,1,2,1.0f,0,-1,0,0)!=0;
}
VERIFY(0x024656EC,CreateHeap);
static u32 materialCall(u32 material,u32 off,s32 index) {
    u32 block=gabi::load<u32>(material+0x18),vt=gabi::load<u32>(block+4);
    return gabi::call_ptr<u32>(gabi::load<u32>(vt+off),block,index);
}
static BOOL daSalvage_draw(daSalvage_c* self) {
    WWHD_FUNC(0x02466D30,BOOL,self);
    calcAlpha(self);int count=0;
    for(s32 i=0;i<160;i++) {
        if(!gabi::call<BOOL>(0x025B48BC,registry(),i) || !gabi::call<BOOL>(0x025B4D0C,registry(),i) || !isEffectKind(self,i) || !gabi::call<s32>(0x025B4CEC,registry(),i))continue;
        u32 data=gabi::ea((void*)self->mModelData),material=gabi::load<u32>(data+0x10);
        u32 alpha=gabi::call<u32>(0x025B4CD8,registry(),i);u8 value=gabi::load<u8>(alpha);
        if(!material)break;
        u32 color=materialCall(material,0x4C,3);u8 old=gabi::load<u8>(color+3);
        color=materialCall(material,0x4C,3);gabi::store<u8>(color+3,value);
        color=materialCall(material,0x4C,3);
        u32 block=gabi::load<u32>(material+0x18),vt=gabi::load<u32>(block+4);
        gabi::call_ptr(gabi::load<u32>(vt+0x3C),block,3,color);
        struct Color_l {be<f32> rgba[4];};gabi::Local<Color_l> normalized,adjusted;
        colorFromGXColor(normalized->rgba,gabi::at<be<u8>>(color));
        gabi::call(0x0274D458,adjusted.get(),normalized.get(),1.0f);
        gabi::store<u32>(material+0xA0,gabi::load<u32>(material+0xA0)|0x400);
        u32 output=gabi::call<u32>(0x027F9F0C,material+0xA0,10);
        f32 a=(f32)gabi::load<u8>(color+3)/255.0f;
        f32 x=adjusted->rgba[0],z=adjusted->rgba[2],y=adjusted->rgba[1];
        gabi::store<f32>(output+4,y);gabi::store<f32>(output+8,z);gabi::store<f32>(output,x);gabi::store<f32>(output+12,a);
        J3DModel* model=(J3DModel*)(void*)self->mModels[count];set_mtx(self,model,i);
        u32 brk=gabi::ea((void*)self->mpBrk);data=gabi::ea((void*)self->mModelData);f32 frame=gabi::load<f32>(brk+4);gabi::call(0x025E83FC,brk,data,frame);
        u32 btk=gabi::ea((void*)self->mpBtk);data=gabi::ea((void*)self->mModelData);frame=gabi::load<f32>(btk+4);gabi::call(0x025E7FC4,btk,data,frame);
        model=(J3DModel*)(void*)self->mModels[count];gabi::call(0x025E2DE0,model,0);
        color=materialCall(material,0x4C,3);gabi::store<u8>(color+3,old);
        if(++count>=16)break;
    }
    gabi::call(0x025E742C,(void*)self->mpBrk);gabi::call(0x025E742C,(void*)self->mpBtk);send_agb(self);return TRUE;
}
VERIFY(0x02466D30,daSalvage_draw);
static BOOL CheckCreateHeap(daSalvage_c* self){WWHD_FUNC(0x024658FC,BOOL,self);return CreateHeap(self);}
VERIFY(0x024658FC,CheckCreateHeap);
static cPhs_State daSalvageCreate(daSalvage_c* self){WWHD_FUNC(0x02465C64,cPhs_State,self);return daSalvage_create(self);}
VERIFY(0x02465C64,daSalvageCreate);
static BOOL daSalvageDelete(daSalvage_c* self){WWHD_FUNC(0x02465D2C,BOOL,self);return daSalvage_delete(self);}
VERIFY(0x02465D2C,daSalvageDelete);
static BOOL daSalvageExecute(daSalvage_c* self){WWHD_FUNC(0x024664F0,BOOL,self);return daSalvage_execute(self);}
VERIFY(0x024664F0,daSalvageExecute);
static BOOL daSalvageDraw(daSalvage_c* self){WWHD_FUNC(0x02466F84,BOOL,self);return daSalvage_draw(self);}
VERIFY(0x02466F84,daSalvageDraw);
namespace {
inline BOOL eventCheck(u32 fn,s16 event) {u32 play=gabi::call<u32>(0x025200D4);return gabi::call<BOOL>(fn,play+0x52C4,event);}
inline void eventReset() {u32 play=gabi::call<u32>(0x025200D4);gabi::store<u16>(play+0x52B8,gabi::load<u16>(play+0x52B8)|8);}
}
static void checkOrder(daSalvage_c* self) {
    WWHD_FUNC(0x02465EC4,void,self);
    u32 play=gabi::call<u32>(0x025200D4),ship=gabi::load<u32>(play+0x5B3C);
    u8 type=gabi::call<u8>(0x025B4CB8,registry(),salvageId());
    u8 kind=gabi::call<u8>(0x025B4CC8,registry(),salvageId());
    if(gabi::load<u16>(gabi::ea(self)+0xF8)!=2) return;
    BOOL skipCountdown=FALSE;
    BOOL starting=(eventCheck(0x0254407C,self->mGetItemEvent) && self->mOrder==2) ||
        (eventCheck(0x0254407C,self->mGetItemLeftEvent) && self->mOrder==1) ||
        (eventCheck(0x0254407C,self->mHazureEvent) && self->mOrder==4) ||
        (eventCheck(0x0254407C,self->mHazureLeftEvent) && self->mOrder==3);
    if(starting) {
        gabi::store<u32>(ship+0x644,gabi::load<u32>(ship+0x644)|0x800);
        u8 order=self->mOrder;type=1;u32 top=gabi::load<u32>(ship+0x71C);self->mBoxId=0xFFFFFFFF;
        if(order==3 || order==4) {self->mHasBox=0;type=0;}
        else {
            s8 room=gabi::load<s8>(0x10475654);
            self->mBoxId=gabi::call<u32>(0x025D5834,0x125,0,top,room,&self->current.angle,0,-1,0x02465900);
            self->mHasBox=1;if(kind==0)type=2;
        }
        u8 item=gabi::call<u8>(0x025B4C88,registry(),salvageId());
        s8 room=gabi::load<s8>(0x10475654);
        u32 chest=gabi::call<u32>(0x025D5834,0x192,((u32)type<<8)|item,top,room,&self->current.angle,&self->scale,-1,0x02465900);
        self->mChestId=chest;self->mOrder=0;
        if(chest==0xFFFFFFFF || (self->mBoxId==0xFFFFFFFF && self->mHasBox)) eventReset();
        else {setProc(self,0x024679A0);self->mTimeout=99;skipCountdown=TRUE;}
    }
    if(!skipCountdown) {
        if(self->mTimeout==0) {
            gabi::Local<be<u32>> actor1,actor2;
            BOOL found=gabi::call<BOOL>(0x025D54C4,(u32)self->mChestId,actor1.get());
            if(self->mHasBox)found &=gabi::call<BOOL>(0x025D54C4,(u32)self->mBoxId,actor2.get());
            if(!found) {
                gabi::call(0x025D57E4,(u32)self->mChestId);gabi::call(0x025D57E4,(u32)self->mBoxId);
                self->mHasBox=0;eventReset();
            }
        }
        self->mTimeout--;
    }
    if(eventCheck(0x025440C8,self->mGetItemEvent) || eventCheck(0x025440C8,self->mHazureEvent) ||
       eventCheck(0x025440C8,self->mGetItemLeftEvent) || eventCheck(0x025440C8,self->mHazureLeftEvent)) {
        gabi::store<u32>(ship+0x644,gabi::load<u32>(ship+0x644)&~0x800U);
        gabi::call(0x025D57E4,(u32)self->mChestId);self->mHasBox=0;end_salvage(self);eventReset();
        if((eventCheck(0x025440C8,self->mHazureEvent) || eventCheck(0x025440C8,self->mHazureLeftEvent)) && (type==2 || kind==5)) createEnemy(self);
        proc_wait_init(self);
    }
}
VERIFY(0x02465EC4,checkOrder);
static BOOL proc_wait(daSalvage_c* self) {
    WWHD_FUNC(0x02467364,BOOL,self);
    u32 play=gabi::call<u32>(0x025200D4),ship=gabi::load<u32>(play+0x5B3C);
    gabi::call<u32>(0x025200D4);if(!ship)return TRUE;
    gabi::Local<be<f32>> distance;
    for(s32 i=0;i<160;i++) {
        s32 sw=gabi::call<s32>(0x025B4C98,registry(),i);
        u8 slot=gabi::call<u8>(0x025B4CA8,registry(),i);
        u8 room=(u8)gabi::call<s8>(0x025B4C74,registry(),i);
        gabi::Local<cXyz> ignored;gabi::call(0x025B4B7C,registry(),ignored.get(),i);
        u8 kind=gabi::call<u8>(0x025B4CC8,registry(),i);
        if(!gabi::call<BOOL>(0x025B48BC,registry(),i))continue;
        getDistance(self,i,distance.get());
        if(kind==2) {
            if(room!=255 && sw!=255) {
                if(!gabi::call<BOOL>(0x025B4D0C,registry(),i) && gabi::call<BOOL>(0x025BA0C0,save()+0x20,sw,room) && !gabi::call<BOOL>(0x025B8A50,save()+0x5E0,room,slot)) gabi::call(0x025B4B64,registry(),i,1);
                if(!gabi::call<BOOL>(0x025BA0C0,save()+0x20,sw,room) && !gabi::call<BOOL>(0x025B8A50,save()+0x5E0,room,slot)) {
                    gabi::Local<be<u16>> name;*name.get()=0xE1;
                    u32 octo=gabi::call<u32>(0x025D5218,0x025E121C,name.get());
                    if(octo && gabi::load<u8>(octo+0x690)==(u32)sw) {
                        gabi::Local<cXyz> pos;gabi::call(0x025B4B7C,registry(),pos.get(),i);
                        f32 x=gabi::load<f32>(octo+0x314),y=pos->y,z=gabi::load<f32>(octo+0x31C);
                        gabi::Local<cXyz> moved;moved->x=x;moved->y=y;moved->z=z;gabi::call(0x025B4998,registry(),i,moved.get());
                    }
                }
            }
        } else if(kind==0) {
            if(gabi::call<BOOL>(0x025B80C8,save()+0xE4,(s32)slot-1) && !gabi::call<BOOL>(0x025B8228,save()+0xE4,(s32)slot-1)) {
                u8 item=gabi::call<u8>(0x025B4C88,registry(),i);
                BOOL triforce=gabi::call<BOOL>(0x025510D0,item);
                if(!triforce || gabi::call<BOOL>(0x025B8308,save()+0xE4,(s32)(u8)(item-0x60)-1))gabi::call(0x025B4B64,registry(),i,1);
            }
        } else if(kind==4) {
            if(gabi::call<s32>(0x02556D14)!=0) {
                if(!gabi::call<BOOL>(0x025B8A50,save()+0x5E0,room,slot))gabi::call(0x025B4B64,registry(),i,1);
            } else if(!gabi::call<s32>(0x02556D14))gabi::call(0x025B4B4C,registry(),i,1);
        } else if(kind==6) {
            if(!gabi::call<s32>(0x02560828) && gabi::call<BOOL>(0x02565DAC) && !gabi::call<BOOL>(0x025B8B94,save()+0x644,gabi::load<u16>(0x1004BD1C+slot*2)))gabi::call(0x025B4B64,registry(),i,1);
            else gabi::call(0x025B4B4C,registry(),i,1);
        }
        if(!isEffectKind(self,i) || !gabi::call<BOOL>(0x025B4D0C,registry(),i))continue;
        if(!((f32)*distance.get()<5000))continue;
        u32 pos=gabi::call<u32>(0x025B4BE0,registry(),i);s8 roomNo=self->mStayNo;
        s32 reverb=gabi::call<s32>(0x02520540,roomNo);gabi::call(0x025E1A40,0x1069,pos,0,reverb);
        f32 dist=*distance.get();if(dist>2000)continue;
        f32 volume=100.0f-dist/20.0f;if(volume<0)volume=0;else if(volume>100)volume=100;
        pos=gabi::call<u32>(0x025B4BE0,registry(),i);
        u32 encoded=volume>=2147483648.0f?(u32)gabi::ftoi(volume-2147483648.0f)+0x80000000U:(u32)gabi::ftoi(volume);
        roomNo=self->mStayNo;reverb=gabi::call<s32>(0x02520540,roomNo);gabi::call(0x025E1A40,0x10A9,pos,encoded,reverb);
    }
    if(checkXZDistance(self)==-1)self->mRndDepthIdx=gabi::ftoi(cM_rndF(10000))%3;
    s32 id=checkDistance(self);gabi::store<s32>(0x10475638,id);if(id==-1)return TRUE;
    play=gabi::call<u32>(0x025200D4);if(gabi::load<u8>(play+0x5292))return TRUE;
    u8 type=gabi::call<u8>(0x025B4CB8,registry(),salvageId()),kind=gabi::call<u8>(0x025B4CC8,registry(),salvageId());
    u8 debug=gabi::load<u8>(0x1046D819);if(debug!=255)type=debug;
    gabi::Local<cXyz> pos;gabi::call(0x025B4B7C,registry(),pos.get(),salvageId());
    u32 x=gabi::load<u32>(gabi::ea(pos.get())),y=gabi::load<u32>(gabi::ea(pos.get())+4),z=gabi::load<u32>(gabi::ea(pos.get())+8);
    gabi::store<u32>(gabi::ea(self)+0x390,x);gabi::store<u32>(gabi::ea(self)+0x394,y);gabi::store<u32>(gabi::ea(self)+0x398,z);
    if(type==1)self->mOrder=gabi::load<s16>(ship+0x680)<0?1:2;
    else if(kind==5 || type==0 || type==2 || type==3)self->mOrder=gabi::load<s16>(ship+0x680)<0?3:4;
    return TRUE;
}
VERIFY(0x02467364,proc_wait);
