/* Per-TU constructors and wrappers, WWHD movebox. */
#include "d/actor/d_a_obj_movebox.h"
namespace daObjMovebox {
static Bgc_c* Bgc_ct(Bgc_c* p) {
    WWHD_FUNC(0x02376278,Bgc_c*,p);
    if(p==nullptr) p=gabi::call<Bgc_c*>(0x0273AD10,0x184);
    if(p!=nullptr) {
        for(s32 i=0;i<23;i++) { p->mGroundY[i]=0.0f; p->mWallPos[i].copy(*gabi::at<cXyz>(0x101FFBA8)); }
        p->mWaterY=0.0f; p->mMaxGroundIdx=-1; p->mWallIdx=-1; p->mNearestWallDist=3.4028234663852886e38f; p->mStateFlags=0;
    }
    return p;
}
VERIFY(0x02376278,Bgc_ct);
static BOOL Mthd_Delete(Act_c* p) {
    WWHD_FUNC(0x0237A670,BOOL,p); BOOL result=p->MoveBGDelete();
    if(p->mbShouldAppear) dComIfG_resDelete(&p->mPhs,p->arc()); return result;
}
VERIFY(0x0237A670,Mthd_Delete);
static BOOL Mthd_Execute(Act_c* p) { WWHD_FUNC(0x0237A6D4,BOOL,p); return p->MoveBGExecute(); }
VERIFY(0x0237A6D4,Mthd_Execute);
static BOOL Mthd_Draw(Act_c* p) { WWHD_FUNC(0x0237A6D8,BOOL,p); return p->Draw_v(); }
VERIFY(0x0237A6D8,Mthd_Draw);
static BOOL Mthd_IsDelete(Act_c* p) { WWHD_FUNC(0x0237A6E8,BOOL,p); return p->IsDelete_v(); }
VERIFY(0x0237A6E8,Mthd_IsDelete);
static void __sinit_d_a_obj_movebox_cpp() {
    WWHD_FUNC(0x0237A6F8,void,(u32)0);
    for(s32 i=0;i<4;i++) gabi::store<u32>(0x1046A6D4+4*i,0);
    __register_global_object(0x101CB1CC);
    gabi::store<f32>(0x1046A680,-3.1415927410125732f); gabi::store<f32>(0x1046A684,3.1415927410125732f);
    gabi::call(0x028ED6F8,0x1046A688); __register_global_object(0x101CB1D8);
    gabi::call(0x028EAB2C,0x1046A689); __register_global_object(0x101CB1E4);
    /* Nonzero line-source direction components initialized by this TU. */
    const u32 positive20[]={0x28,0x2C,0x3C,0x58,0x78,0x7C,0x8C,0xA8,0xC8,0xCC,0xDC,0xF8,0x118,0x11C,0x12C,0x148};
    const u32 negative20[]={0x38,0x48,0x4C,0x5C,0x88,0x98,0x9C,0xAC,0xD8,0xE8,0xEC,0xFC,0x128,0x138,0x13C,0x14C};
    for(u32 off:positive20) gabi::store<f32>(0x101CB2B4+off,1.0f);
    for(u32 off:negative20) gabi::store<f32>(0x101CB2B4+off,-1.0f);
    const u32 positive5[]={0x18,0x1C,0x2C,0x48}; const u32 negative5[]={0x28,0x38,0x3C,0x4C};
    for(u32 off:positive5) gabi::store<f32>(0x101CB264+off,1.0f);
    for(u32 off:negative5) gabi::store<f32>(0x101CB264+off,-1.0f);
    gabi::call(0x028EFFD0,0x1046A7CC,23,0x54,0x0237AABC); __register_global_object(0x101CB1F0);
    gabi::call(0x024F22DC,0x1046A77C); __register_global_object(0x101CB1FC);
    gabi::call(0x028EFFD0,0x1046AF58,23,0x6C,0x0237AB6C); __register_global_object(0x101CB208);
    for(s32 i=0;i<13;i++) gabi::store<f32>(0x101CB424+0x9C*i+0xC,75.0f);
}
VERIFY(0x0237A6F8,__sinit_d_a_obj_movebox_cpp);
static void arraydtor_gnd() { WWHD_FUNC(0x0237A8F8,void,(u32)0); gabi::call(0x028F0164,0x1046A7CC,23,0x54,0x0237A940,0,0); }
VERIFY(0x0237A8F8,arraydtor_gnd);
static void arraydtor_wall() { WWHD_FUNC(0x0237A91C,void,(u32)0); gabi::call(0x028F0164,0x1046AF58,23,0x6C,0x0237AA30,0,0); }
VERIFY(0x0237A91C,arraydtor_wall);
static void gnd_dt(void* p,s32 flags) {
    WWHD_FUNC(0x0237A940,void,p,flags);
    if(p!=nullptr) {
        u32 b=gabi::ea(p); gabi::store<u32>(b+0x20,0x1002D004); gabi::store<u32>(b+0x40,0x1002D024); gabi::store<u32>(b+0x4C,0x1002CFE4);
        gabi::call(0x02008DAC,p,0); if(flags&1) operator_delete(p);
    }
}
VERIFY(0x0237A940,gnd_dt);
static void water_dt(void* p,s32 flags) {
    WWHD_FUNC(0x0237A9B8,void,p,flags);
    if(p!=nullptr) {
        u32 b=gabi::ea(p); gabi::store<u32>(b+0x20,0x1002D074); gabi::store<u32>(b+0x24,0x1002D094); gabi::store<u32>(b+0x30,0x1002CFE4);
        gabi::call(0x02008B4C,gabi::at<u8>(b+0x10),0); if(flags&1) operator_delete(p);
    }
}
VERIFY(0x0237A9B8,water_dt);
static void wall_dt(void* p,s32 flags) {
    WWHD_FUNC(0x0237AA30,void,p,flags);
    if(p!=nullptr) {
        u32 b=gabi::ea(p); gabi::store<u32>(b+0x58,0x1002D104); gabi::store<u32>(b+0x64,0x1002CFE4); gabi::store<u32>(b+0x20,0x1002CFD4);
        gabi::call(0x02008B4C,p,0); if(flags&1) operator_delete(p);
    }
}
VERIFY(0x0237AA30,wall_dt);
static void trivial_dt(void* p,s32 flags) { WWHD_FUNC(0x0237AAA8,void,p,flags); if(p!=nullptr && (flags&1)) operator_delete(p); }
VERIFY(0x0237AAA8,trivial_dt);
static dBgS_GndChk* gnd_ct(dBgS_GndChk* p) {
    WWHD_FUNC(0x0237AABC,dBgS_GndChk*,p);
    if(p==nullptr) p=gabi::call<dBgS_GndChk*>(0x0273AD10,0x54);
    if(p!=nullptr) dBgS_GndChk_ct(p,{0x1002D034,0x1002D044,0x1002D064,0x1002D054},true);
    return p;
}
VERIFY(0x0237AABC,gnd_ct);
static dBgS_LinChk* wall_ct(dBgS_LinChk* p) {
    WWHD_FUNC(0x0237AB6C,dBgS_LinChk*,p);
    if(p==nullptr) p=gabi::call<dBgS_LinChk*>(0x0273AD10,0x6C);
    if(p!=nullptr) dBgS_LinChk_ct(p,{0x1002D114,0x1002D124,0x1002D144,0x1002D134},true);
    return p;
}
VERIFY(0x0237AB6C,wall_ct);
static void empty_virtual(void* p) { WWHD_FUNC(0x0237AC1C,void,p); }
VERIFY(0x0237AC1C,empty_virtual);
static void smoke_draw(void* p) { WWHD_FUNC(0x0237AC20,void,p); }
VERIFY(0x0237AC20,smoke_draw);
static BOOL actor_IsDelete(Act_c* p) { WWHD_FUNC(0x0237AC24,BOOL,p); return TRUE; }
VERIFY(0x0237AC24,actor_IsDelete);
static void actor_empty(Act_c* p) { WWHD_FUNC(0x0237AC2C,void,p); }
VERIFY(0x0237AC2C,actor_empty);
static EffSmokeCB* smoke_ct(EffSmokeCB* p) {
    WWHD_FUNC(0x0237AC30,EffSmokeCB*,p);
    if(p==nullptr) p=gabi::call<EffSmokeCB*>(0x0273AD10,0x34);
    if(p!=nullptr) { gabi::call(0x025A5B18,p,1); gabi::store<u32>(gabi::ea(p),0x1002D1D0); }
    return p;
}
VERIFY(0x0237AC30,smoke_ct);
static void smoke_dt(EffSmokeCB* p,s32 flags) { WWHD_FUNC(0x0237AC88,void,p,flags); if(p!=nullptr && (flags&1)) operator_delete(p); }
VERIFY(0x0237AC88,smoke_dt);
static void actor_dt(Act_c* p,s32 flags) {
    WWHD_FUNC(0x0237AC9C,void,p,flags);
    if(p!=nullptr) {
        gabi::call(0x028F0164,&p->mSmokeCbs[0],2,0x34,0x0237AC88,0,0);
        gabi::call(0x02515A70,&p->mCyl,2); gabi::call(0x02515860,&p->mStts,2); gabi::call(0x025D50BC,p,0);
        if(flags&1) operator_delete(p);
    }
}
VERIFY(0x0237AC9C,actor_dt);
static s32 Mthd_Create(Act_c* p) {
    WWHD_FUNC(0x0237A468,s32,p);
    u32 self=gabi::ea(p);u32 condition=gabi::load<u32>(self+0x2E4);
    if(!(condition&8)) {
        if(p!=nullptr) {
            gabi::call(0x024F1D40,p);gabi::store<u32>(self+0xB4,0x1002D79C);
            gabi::call(0x0200BD2C,&p->mStts);gabi::call(0x02515DA0,gabi::at<u8>(self+0x43C));
            gabi::store<u32>(self+0x438,0x1004AE88);gabi::store<u32>(self+0x43C,0x1004AEC0);
            gabi::call(0x02515FB8,&p->mCyl);
            gabi::store<u32>(self+0x570,0x100015A8);gabi::store<u32>(self+0x56C,0x1002CFC4);
            gabi::call(0x02018590,gabi::at<u8>(self+0x574));
            gabi::store<u32>(self+0x498,0x1004B108);gabi::store<u32>(self+0x570,0x1004B160);gabi::store<u32>(self+0x588,0x1004B150);
            Bgc_ct(&p->mBgc);gabi::call(0x028EFFD0,&p->mSmokeCbs[0],2,0x34,0x0237AC30);
            condition=gabi::load<u32>(self+0x2E4);
        }
        gabi::store<u32>(self+0x2E4,condition|8);
    }
    p->mType=p->prm(4,24);p->prmX_init();p->prmZ_init();
    s32 phase=5;u32 appear=gabi::call<u32>(0x02376F9C,p);p->mbShouldAppear=appear;
    if(appear) {
        phase=dComIfG_resLoad(&p->mPhs,p->arc());
        if(phase==4) {
            s32 heap=p->attr(0x1002D168,0x1002D1AC)->mDZBHeapSize;p->path_init();
            const Attr_c* a=p->attr(0x1002D168,0x1002D1AC);
            const char* arc=p->arc();s32 dzb=a->mDZBFileIndex;
            phase=gabi::call<s32>(0x024F1D9C,p,arc,dzb,0x024EE76C,heap);
            if(phase!=4 && phase!=5) JUT_ASSERT_fail(STR(0x1002D168),0x7D4,STR(0x1002D218));
        }
    }
    return phase;
}
VERIFY(0x0237A468,Mthd_Create);

}
