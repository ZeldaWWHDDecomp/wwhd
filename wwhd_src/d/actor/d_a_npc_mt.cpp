/* Carlov, Nintendo Gallery. Reconstructed from the WWHD game. */
#include "d/actor/d_a_npc_mt.h"

template<class T> static T readAt(u32 p,u32 off) { return gabi::load<T>(p+off); }
template<class T> static void writeAt(u32 p,u32 off,T v) { gabi::store<T>(p+off,v); }
static u32 saveObject() { return gabi::load<u32>(0x101F84DC); }
static u32 playObject() { return gabi::call<u32>(0x025200D4); }
static u32 photoBuffer() { return gabi::call<u32>(0x02720118,saveObject()+0x12C0); }

BOOL daNpcMt_CheckCreateHeap(daNpcMt_c* a) { WWHD_FUNC(0x0229FD60,BOOL,a); return gabi::call<BOOL>(0x0229FABC,a); }
VERIFY(0x0229FD60,daNpcMt_CheckCreateHeap);
u8 daNpcMt_getPrmNpcNo(daNpcMt_c* a) { WWHD_FUNC(0x0229FD64,u8,a); return 0; }
VERIFY(0x0229FD64,daNpcMt_getPrmNpcNo);
u8 daNpcMt_isFigureGet(daNpcMt_c* a,u8 figure) {
    WWHD_FUNC(0x0229FE28,u8,a,figure);
    if(figure>=0x86) return 0;
    u16 event=gabi::load<u16>(0x101C2478+(figure>>3)*2);
    u32 value=gabi::call<u32>(0x025B8BB0,saveObject()+0x644,event);
    return value&(1u<<(figure&7));
}
VERIFY(0x0229FE28,daNpcMt_isFigureGet);
s32 daNpcMt_getFigureMakeNum(daNpcMt_c* a) {
    WWHD_FUNC(0x0229FEAC,s32,a);
    s32 count=0;
    for(u32 i=0;i<0x86;++i) if(daNpcMt_isFigureGet(a,i)) ++count;
    return count;
}
VERIFY(0x0229FEAC,daNpcMt_getFigureMakeNum);
BOOL daNpcMt_isComp(daNpcMt_c* a) { WWHD_FUNC(0x0229FF24,BOOL,a); return daNpcMt_getFigureMakeNum(a)>=0x85; }
VERIFY(0x0229FF24,daNpcMt_isComp);
void daNpcMt_clearPhotoSelection(daNpcMt_c* a) { WWHD_FUNC(0x0229FF54,void,a); writeAt<u8>(photoBuffer(),0x10,0); }
VERIFY(0x0229FF54,daNpcMt_clearPhotoSelection);
void daNpcMt_setFigure(daNpcMt_c* a,u8 figure) {
    WWHD_FUNC(0x0229FF88,void,a,figure);
    if(figure>=0x86) return;
    u32 eventAddress=0x101C2478+(figure>>3)*2;
    u32 value=gabi::call<u32>(0x025B8BB0,saveObject()+0x644,gabi::load<u16>(eventAddress));
    gabi::call(0x025B8AF4,saveObject()+0x644,gabi::load<u16>(eventAddress),(u8)(value|(1u<<(figure&7))));
    gabi::call(0x025B8B68,saveObject()+0x644,0x3A01);
}
VERIFY(0x0229FF88,daNpcMt_setFigure);
u8 daNpcMt_getPhotoSelection(daNpcMt_c* a) { WWHD_FUNC(0x022A037C,u8,a); return readAt<u8>(photoBuffer(),0x10); }
VERIFY(0x022A037C,daNpcMt_getPhotoSelection);
s16 daNpcMt_XyCallback(daNpcMt_c* a,s32 value) { WWHD_FUNC(0x022A04A4,s16,a,value); return gabi::call<s16>(0x022A03AC,a,value); }
VERIFY(0x022A04A4,daNpcMt_XyCallback);
void daNpcMt_setCollision(daNpcMt_c* a,void* cylinder,cXyz* center,f32 radius,f32 height) {
    WWHD_FUNC(0x022A0588,void,a,cylinder,center,radius,height);
    u32 c=gabi::ea(cylinder);
    gabi::call(0x020182E0,c+0x118,center);
    gabi::call(0x020184DC,c+0x118,radius);
    gabi::call(0x02018428,c+0x118,height);
    gabi::call(0x0200E240,playObject()+0x26A4,cylinder);
}
VERIFY(0x022A0588,daNpcMt_setCollision);
s32 daNpcMt_phase2(daNpcMt_c* a) {
    WWHD_FUNC(0x022A07F8,s32,a);
    s32 phase=gabi::call<s32>(0x02520460,gabi::ea(a)+0x7DC,gabi::load<u32>(0x101C231C));
    if(phase==4) {
        if(gabi::call<s32>(0x025D63E8,a,0x0229FD60,0x2900)) return gabi::call<s32>(0x022A0618,a);
        writeAt<u32>(gabi::ea(a),0x44C,0); return 5;
    }
    return phase;
}
VERIFY(0x022A07F8,daNpcMt_phase2);
s32 daNpcMt_create(daNpcMt_c* a) { WWHD_FUNC(0x022A0880,s32,a); return gabi::call<s32>(0x02525FE4,gabi::ea(a)+0x7E4,0x101C24E8,a); }
VERIFY(0x022A0880,daNpcMt_create);
s32 daNpc_MtCreate(daNpcMt_c* a) { WWHD_FUNC(0x022A0894,s32,a); return daNpcMt_create(a); }
VERIFY(0x022A0894,daNpc_MtCreate);
u32 daNpcMt_getPhoto(daNpcMt_c* a,u32 index) { WWHD_FUNC(0x022A0898,u32,a,index); return gabi::call<u32>(0x0271F978,photoBuffer(),index); }
VERIFY(0x022A0898,daNpcMt_getPhoto);
BOOL daNpc_MtDelete(daNpcMt_c* a) { WWHD_FUNC(0x022A0B1C,BOOL,a); return gabi::call<BOOL>(0x022A08D8,a); }
VERIFY(0x022A0B1C,daNpc_MtDelete);
void daNpcMt_executeSetMode(daNpcMt_c* a,u8 mode) {
    WWHD_FUNC(0x022A0DFC,void,a,mode);
    u32 p=gabi::ea(a), entry=0x101C2410+mode*8;
    writeAt<f32>(p,0x88C,0.0f);
    s16 virtualIndex=readAt<s16>(entry,2);
    u32 self=p+readAt<s16>(entry,0), target;
    if(virtualIndex<0) target=readAt<u32>(entry,4);
    else { u32 vt=readAt<u32>(self,readAt<s16>(entry,6)); target=readAt<u32>(vt+virtualIndex*8,4); }
    writeAt<u8>(p,0x8BA,gabi::call_ptr<u32>(target,self));
}
VERIFY(0x022A0DFC,daNpcMt_executeSetMode);
BOOL daNpcMt_chkEndEvent(daNpcMt_c* a) {
    WWHD_FUNC(0x022A0F34,BOOL,a);
    s16 event=readAt<s16>(gabi::ea(a),0x89E);
    if(!gabi::call<s32>(0x025440C8,playObject()+0x52C4,event)) return FALSE;
    u32 play=playObject(); writeAt<u16>(play,0x52B8,readAt<u16>(play,0x52B8)|8); return TRUE;
}
VERIFY(0x022A0F34,daNpcMt_chkEndEvent);
void daNpcMt_setMessage(daNpcMt_c* a,u32 message) { WWHD_FUNC(0x022A0FA0,void,a,message); writeAt<u32>(gabi::ea(a),0x7C0,message); }
VERIFY(0x022A0FA0,daNpcMt_setMessage);
void daNpcMt_eventGetItemInit(daNpcMt_c* a) {
    WWHD_FUNC(0x022A1064,void,a);
    u32 p=gabi::ea(a);
    u32 item=gabi::call<u32>(0x025D7DEC,p+0x314,readAt<u32>(p,0x898),0,-1,-1,0,0);
    if(item!=0xFFFFFFFF) writeAt<u32>(playObject(),0x52A0,item);
}
VERIFY(0x022A1064,daNpcMt_eventGetItemInit);
BOOL daNpcMt_eventMesSet(daNpcMt_c* a) {
    WWHD_FUNC(0x022A1270,BOOL,a);
    u32 result=gabi::call<u16>(0x022A10BC,a,0),p=gabi::ea(a);
    if(result==0x12) {
        u8 flags=readAt<u8>(p,0x8BD);
        if(flags&1) { writeAt<u8>(p,0x8BD,flags&0xFE); writeAt<u32>(p,0x898,7); writeAt<u8>(p,0x8B8,3); }
        else if(flags&2) { writeAt<u8>(p,0x8BD,flags&0xFD); writeAt<u32>(p,0x898,5); writeAt<u8>(p,0x8B8,3); }
    }
    return result==0x12;
}
VERIFY(0x022A1270,daNpcMt_eventMesSet);
void daNpcMt_setAnmFromMsgTag(daNpcMt_c* a) {
    WWHD_FUNC(0x022A141C,void,a);
    u32 tag=readAt<u8>(playObject(),0x5BC5);
    if(tag<=6) gabi::call(0x022A02B0,a,gabi::load<u32>(0x1001EAD0+tag*4));
    writeAt<u8>(playObject(),0x5BC5,0xFF);
}
VERIFY(0x022A141C,daNpcMt_setAnmFromMsgTag);
void daNpcMt_eventMove(daNpcMt_c* a) {
    WWHD_FUNC(0x022A1478,void,a);
    if(daNpcMt_chkEndEvent(a)) return;
    u32 p=gabi::ea(a);u8 saved=readAt<u8>(p,0x440);
    if(gabi::call<s32>(0x0259F858,p+0x3E0)) { if(!readAt<u8>(p,0x440)) writeAt<u8>(p,0x440,saved); }
    else { gabi::call(0x022A12FC,a); daNpcMt_setAnmFromMsgTag(a); }
}
VERIFY(0x022A1478,daNpcMt_eventMove);
BOOL daNpc_MtExecute(daNpcMt_c* a) { WWHD_FUNC(0x022A1B9C,BOOL,a); return gabi::call<BOOL>(0x022A1914,a); }
VERIFY(0x022A1B9C,daNpc_MtExecute);
BOOL daNpc_MtDraw(daNpcMt_c* a) { WWHD_FUNC(0x022A1C48,BOOL,a); return gabi::call<BOOL>(0x022A1BA0,a); }
VERIFY(0x022A1C48,daNpc_MtDraw);
u32 daNpcMt_executeCommon(daNpcMt_c* a) {
    WWHD_FUNC(0x022A1C4C,u32,a);
    u32 p=gabi::ea(a);writeAt<u8>(p,0x8B8,readAt<u8>(p,0x8B7)!=0);
    u8 result=readAt<u8>(p,0x8B6);
    if(result==1 && readAt<u8>(p,0x8BA)!=1) { daNpcMt_executeSetMode(a,1); result=readAt<u8>(p,0x8B6); }
    return result;
}
VERIFY(0x022A1C4C,daNpcMt_executeCommon);
s32 daNpcMt_executeWaitInit(daNpcMt_c* a) {
    WWHD_FUNC(0x022A1CAC,s32,a);
    writeAt<f32>(gabi::ea(a),0x370,0.0f);gabi::call(0x022A02B0,a,0x101C2330);
    s16 params[9];for(u32 i=0;i<9;++i) params[i]=gabi::load<s16>(0x101C249C+i*2);
    gabi::call(0x0259E08C,gabi::ea(a)+0x3AC,params[2],params[3],params[6],params[7],params[0],params[1],params[4],params[5],params[8]);
    return 0;
}
VERIFY(0x022A1CAC,daNpcMt_executeWaitInit);
void daNpcMt_executeWait(daNpcMt_c* a) {
    WWHD_FUNC(0x022A1D2C,void,a);daNpcMt_executeCommon(a);
    if(readAt<u8>(gabi::ea(a),0x8BE)) gabi::call(0x022A02B0,a,0x101C2330);
}
VERIFY(0x022A1D2C,daNpcMt_executeWait);
s32 daNpcMt_executeTalkInit(daNpcMt_c* a) { WWHD_FUNC(0x022A1D74,s32,a); return 1; }
VERIFY(0x022A1D74,daNpcMt_executeTalkInit);
void daNpcMt_executeTalk(daNpcMt_c* a) {
    WWHD_FUNC(0x022A1D7C,void,a);daNpcMt_executeCommon(a);
    if(gabi::call<u16>(0x022A10BC,a,1)==0x12) {
        writeAt<u8>(gabi::ea(a),0x8B6,0);daNpcMt_executeSetMode(a,0);
        u32 play=playObject();writeAt<u16>(play,0x52B8,readAt<u16>(play,0x52B8)|8);
    } else daNpcMt_setAnmFromMsgTag(a);
}
VERIFY(0x022A1D7C,daNpcMt_executeTalk);
u32 daNpcMt_changePhotoNo(u32 photo) {
    WWHD_FUNC(0x022A1F1C,u32,photo);
    switch(photo) { case 1:return 0x6A;case 2:return 0x69;case 4:return 0x67;case 6:return 0x6D;default:return photo; }
}
VERIFY(0x022A1F1C,daNpcMt_changePhotoNo);
void daNpcMt_submitPhoto(daNpcMt_c* a,u32 photo) {
    WWHD_FUNC(0x022A2008,void,a,photo);
    u32 selection=daNpcMt_getPhotoSelection(a);
    gabi::call(0x0271F994,photoBuffer(),selection,photo);
    gabi::call(0x0271F9A8,photoBuffer(),1);
}
VERIFY(0x022A2008,daNpcMt_submitPhoto);
void daNpcMt_textureDtor(void* p,u32 flags) { WWHD_FUNC(0x022A277C,void,p,flags); if(p&&(flags&1)) gabi::call(0x0273AF40,p); }
VERIFY(0x022A277C,daNpcMt_textureDtor);
BOOL daNpc_MtIsDelete(daNpcMt_c* a) { WWHD_FUNC(0x022A2790,BOOL,a); return TRUE; }
VERIFY(0x022A2790,daNpc_MtIsDelete);
void daNpcMt_destruct(daNpcMt_c* a,u32 flags) {
    WWHD_FUNC(0x022A2798,void,a,flags);
    if(!a) return;u32 p=gabi::ea(a);
    gabi::call(0x02515A70,p+0x690,2);gabi::call(0x02515860,p+0x654,2);gabi::call(0x02018034,p+0x628,2);
    writeAt<u32>(p,0x470,0x1001E9B0);writeAt<u32>(p,0x464,0x1001E9C0);
    gabi::call(0x024EFD9C,p+0x450,0);gabi::call(0x025D50BC,a,0);
    if(flags&1) gabi::call(0x0273AF40,a);
}
VERIFY(0x022A2798,daNpcMt_destruct);
void daNpcMt_textureEmpty(void* p) { WWHD_FUNC(0x022A2834,void,p); }
VERIFY(0x022A2834,daNpcMt_textureEmpty);

daNpcMt_c* daNpcMt_construct(daNpcMt_c* a) {
    WWHD_FUNC(0x0229FD7C,daNpcMt_c*,a);
    if(!a) a=gabi::call<daNpcMt_c*>(0x0273AD10,0x8CC);
    if(!a) return a;
    u32 p=gabi::ea(a);gabi::call(0x025A1458,a);writeAt<u32>(p,0xB4,0x1001EB40);
    gabi::call(0x025E7820,p+0x7F0);writeAt<u8>(p,0x8BC,daNpcMt_getPrmNpcNo(a));
    writeAt<s16>(p,0x8A6,readAt<s16>(p,0x2FA));writeAt<f32>(p,0x890,-1.0f);
    writeAt<u8>(p,0x8BB,0);writeAt<f32>(p,0x88C,0.0f);writeAt<u8>(p,0x8BE,0);
    writeAt<u16>(p,0x8AA,0);writeAt<u8>(p,0x89C,1);writeAt<u16>(p,0x8A0,0);
    writeAt<u8>(p,0x8C3,0);writeAt<u8>(p,0x8BA,0);return a;
}
VERIFY(0x0229FD7C,daNpcMt_construct);
BOOL daNpcMt_setAnmTbl(daNpcMt_c* a,void* table) {
    WWHD_FUNC(0x022A02B0,BOOL,a,table);
    u32 p=gabi::ea(a),t=gabi::ea(table);
    if(readAt<u8>(t,0)==0xFF) { writeAt<u32>(p,0x884,0);return TRUE; }
    writeAt<u32>(p,0x884,t);s8 count=readAt<s8>(t,2);writeAt<s8>(p,0x8C0,count);
    u8 animation=readAt<u8>(t,0);
    if(count>0 || readAt<u8>(p,0x8BE)!=animation)
        gabi::call(0x022A01A4,a,animation,count>0?0:2,(f32)readAt<u8>(t,1));
    return FALSE;
}
VERIFY(0x022A02B0,daNpcMt_setAnmTbl);
s16 daNpcMt_XyCheckCB(daNpcMt_c* a,s32 index) {
    WWHD_FUNC(0x022A03AC,s16,a,index);
    if(!gabi::call<s32>(0x025B8B94,saveObject()+0x644,0x2F02)) return 0;
    if(gabi::call<s32>(0x025B8B94,saveObject()+0x644,0x2F01)) {
        if(gabi::call<s32>(0x025B8B94,saveObject()+0x644,0x3080)) return 0;
        if(daNpcMt_getPhotoSelection(a)>=0xC) return 0;
    }
    u8 item=readAt<u8>(playObject()+index,0x5BBB);
    if(item!=0x23&&item!=0x26) return 0;
    u32 photos=gabi::call<u32>(0x02720144,saveObject()+0x12C0);
    return readAt<u8>(photos,0x3C030C)!=0;
}
VERIFY(0x022A03AC,daNpcMt_XyCheckCB);
void daNpcMt_setMtx(daNpcMt_c* a) {
    WWHD_FUNC(0x022A04A8,void,a);
    u32 p=gabi::ea(a),model=readAt<u32>(readAt<u32>(p,0x44C),0x90);
    f32 x=readAt<f32>(p,0x330),y=readAt<f32>(p,0x334),z=readAt<f32>(p,0x338);
    writeAt<f32>(model,0xBC,x);writeAt<f32>(model,0xC0,y);writeAt<f32>(model,0xC4,z);
    gabi::call(0x028E93CC,0x1048D0CC,readAt<f32>(p,0x314),readAt<f32>(p,0x318),readAt<f32>(p,0x31C));
    gabi::call(0x025F1C28,0x1048D0CC,readAt<s16>(p,0x322));
    model=readAt<u32>(readAt<u32>(p,0x44C),0x90);f32 matrix[12];
    for(u32 i=0;i<12;++i)matrix[i]=gabi::load<f32>(0x1048D0CC+i*4);
    for(u32 i=0;i<12;++i)writeAt<f32>(model,0xC8+i*4,matrix[i]);
}
VERIFY(0x022A04A8,daNpcMt_setMtx);
void daNpcMt_checkOrder(daNpcMt_c* a) {
    WWHD_FUNC(0x022A0E84,void,a);u32 p=gabi::ea(a);u16 status=readAt<u16>(p,0xF8);
    if(status==2) {
        s16 event=readAt<s16>(p,0x89E);
        if(gabi::call<s32>(0x0254407C,playObject()+0x52C4,event)&&readAt<u8>(p,0x8B8)==3)writeAt<u8>(p,0x8B8,0);
    } else if(status==1&&(readAt<u8>(p,0x8B8)==1||readAt<u8>(p,0x8B8)==2)) {
        writeAt<u8>(p,0x8B6,1);daNpcMt_executeSetMode(a,1);
    }
}
VERIFY(0x022A0E84,daNpcMt_checkOrder);
void daNpcMt_eventMesSetInit(daNpcMt_c* a,s32 staff) {
    WWHD_FUNC(0x022A0FA8,void,a,staff);u32 p=gabi::ea(a);
    u32 substance=gabi::call<u32>(0x0254487C,playObject()+0x52C4,staff,0x1001EAB4,3);
    if(substance) {
        writeAt<u32>(p,0x888,0);u32 message=readAt<u32>(substance,0);
        if(message==1)return;
        if(message==0)message=gabi::call_ptr<u32>(readAt<u32>(readAt<u32>(p,0xB4),0x1C),a);
        daNpcMt_setMessage(a,message);
        u32 table=readAt<u32>(p,0x888);if(table)daNpcMt_setMessage(a,readAt<u32>(table,0));
    } else {
        u32 table=readAt<u32>(p,0x888)+4;writeAt<u32>(p,0x888,table);daNpcMt_setMessage(a,readAt<u32>(table,0));
    }
}
VERIFY(0x022A0FA8,daNpcMt_eventMesSetInit);
void daNpcMt_eventOrder(daNpcMt_c* a) {
    WWHD_FUNC(0x022A1500,void,a);u32 p=gabi::ea(a);u8 mode=readAt<u8>(p,0x8B8);
    if(mode==1||mode==2) {
        u16 flags=readAt<u16>(p,0xFA);mode=readAt<u8>(p,0x8B8);writeAt<u16>(p,0xFA,flags|0x21);
        if(mode==2)gabi::call(0x025D76A8,a);
    } else if(mode==3) {
        u32 link=readAt<u32>(playObject(),0x5B2C);
        gabi::call(0x025D7970,link,a,readAt<s16>(p,0x89E),0,0xFFFF);
    }
}
VERIFY(0x022A1500,daNpcMt_eventOrder);
void daNpcMt_playTexPatternAnm(daNpcMt_c* a) {
    WWHD_FUNC(0x022A1598,void,a);u32 p=gabi::ea(a);
    if(gabi::call<s16>(0x02055B64,p+0x8AE))return;
    u32 animation=readAt<u32>(p,0x7EC);s32 max=gabi::call_ptr<s32>(readAt<u32>(readAt<u32>(animation,4),0x14),animation);
    if((s32)readAt<u8>(p,0x8B9)>=max) {
        animation=readAt<u32>(p,0x7EC);max=gabi::call_ptr<s32>(readAt<u32>(readAt<u32>(animation,4),0x14),animation);
        u8 frame=readAt<u8>(p,0x8B9);writeAt<u16>(p,0x8AE,120);writeAt<u8>(p,0x8B9,frame-(u32)max);
    } else writeAt<u8>(p,0x8B9,readAt<u8>(p,0x8B9)+1);
}
VERIFY(0x022A1598,daNpcMt_playTexPatternAnm);
void daNpcMt_playAnm(daNpcMt_c* a) {
    WWHD_FUNC(0x022A1638,void,a);u32 p=gabi::ea(a);
    u8 flags=readAt<u8>(p,0x8BF);u32 morph=readAt<u32>(p,0x44C);writeAt<u8>(p,0x8BF,flags&0xFE);
    if(!gabi::call<s32>(0x025E535C,morph,0,0,0)||!readAt<u32>(p,0x884))return;
    s8 count=readAt<s8>(p,0x8C0);if(count<=0)return;--count;writeAt<s8>(p,0x8C0,count);
    if(count==0) {
        u32 table=readAt<u32>(p,0x884)+3;writeAt<u32>(p,0x884,table);
        if(daNpcMt_setAnmTbl(a,gabi::at<void>(table)))writeAt<u8>(p,0x8BF,readAt<u8>(p,0x8BF)|1);
    } else gabi::call(0x022A01A4,a,readAt<u8>(readAt<u32>(p,0x884),0),0,0.0f);
}
VERIFY(0x022A1638,daNpcMt_playAnm);
BOOL daNpcMt_hasPhoto(daNpcMt_c* a,u32 figure) {
    WWHD_FUNC(0x022A1F64,BOOL,a,figure);u32 count=daNpcMt_getPhotoSelection(a);
    for(u32 i=0;i<count;++i)if(daNpcMt_getPhoto(a,(u8)i)==figure)return TRUE;
    return FALSE;
}
VERIFY(0x022A1F64,daNpcMt_hasPhoto);
void daNpcMt_staticInit() {
    WWHD_FUNC(0x022A26E8,void);
    gabi::store<u32>(0x10467CD4,0);gabi::store<u32>(0x10467CCC,0);gabi::store<u32>(0x10467CD8,0);gabi::store<u32>(0x10467CD0,0);
    gabi::call(0x028F026C,0x101C24FC);
    gabi::store<f32>(0x10467CC0,gabi::load<f32>(0x1001EB38));gabi::store<f32>(0x10467CC4,gabi::load<f32>(0x1001EB3C));
    gabi::call(0x028ED6F8,0x10467CC8);gabi::call(0x028F026C,0x101C2508);
    gabi::call(0x028EAB2C,0x10467CC9);gabi::call(0x028F026C,0x101C2514);
}
VERIFY(0x022A26E8,daNpcMt_staticInit);

static u32 objectResource(u32 resource) {
    gabi::Local<SafeString> name;
    name->mStringTop=gabi::load<u32>(0x101C231C);name->__vtbl=0x1001E998;
    return gabi::call<u32>(0x026067F4,gabi::load<u32>(0x101F4F28),(SafeString*)name,resource);
}
BOOL daNpcMt_initTexPatternAnm(daNpcMt_c* a,u32 modify) {
    WWHD_FUNC(0x0229F9B8,BOOL,a,modify);u32 p=gabi::ea(a);
    u32 data=readAt<u32>(readAt<u32>(readAt<u32>(p,0x44C),0x90),0xAC);
    u32 pattern=objectResource(gabi::load<u32>(0x1001E988));writeAt<u32>(p,0x7EC,pattern);
    if(!pattern) { gabi::call(0x0273AA24,0x1001EA08,0x85E,0x1001E9EC);pattern=readAt<u32>(p,0x7EC); }
    if(!gabi::call<s32>(0x025E789C,p+0x7F0,data,pattern,1,2,1.0f,0,-1,modify))return FALSE;
    writeAt<u8>(p,0x8B9,0);writeAt<u16>(p,0x8AE,0);return TRUE;
}
VERIFY(0x0229F9B8,daNpcMt_initTexPatternAnm);
s32 daNpcMt_phase1(daNpcMt_c* a) {
    WWHD_FUNC(0x022A0018,s32,a);u32 p=gabi::ea(a),flags=readAt<u32>(p,0x2E4);
    if(!(flags&8)) { if(a) { daNpcMt_construct(a);flags=readAt<u32>(p,0x2E4); }writeAt<u32>(p,0x2E4,flags|8); }
    if(gabi::call<s32>(0x025B8B94,saveObject()+0x644,0x3D08)) { daNpcMt_setFigure(a,0x40);return 3; }
    if(readAt<s16>(playObject(),0x513C)==0) {
        BOOL complete=daNpcMt_isComp(a);u32 save=saveObject();
        if(complete) { gabi::call(0x025B8B68,save+0x644,0x3D08);daNpcMt_setFigure(a,0x40);return 3; }
        gabi::call(0x025B8B7C,save+0x1178,0x202);
        if(gabi::call<s32>(0x025B8B94,saveObject()+0x644,0x4080)) {
            for(u32 event:{0x3080u,0x2F01u,0x4080u,0x3F01u,0x4040u})gabi::call(0x025B8B7C,saveObject()+0x644,event);
            daNpcMt_clearPhotoSelection(a);
        }
    }
    writeAt<u8>(p,0x8BB,1);return 2;
}
VERIFY(0x022A0018,daNpcMt_phase1);
void daNpcMt_setAnm(daNpcMt_c* a,u8 animation,s32 attr,f32 blend) {
    WWHD_FUNC(0x022A01A4,void,a,animation,attr,blend);u32 p=gabi::ea(a);
    f32 override=readAt<f32>(p,0x890);
    if(!(override<0.0f)) { blend=override;writeAt<f32>(p,0x890,-1.0f); }
    u32 resource=objectResource(gabi::load<u32>(0x1001E9D0+animation*4));
    gabi::call(0x025E4A98,readAt<u32>(p,0x44C),resource,attr,blend,1.0f,0.0f,-1.0f,0);
    writeAt<u8>(p,0x8BE,animation);
}
VERIFY(0x022A01A4,daNpcMt_setAnm);
u16 daNpcMt_talk2(daNpcMt_c* a,s32 init) {
    WWHD_FUNC(0x022A10BC,u16,a,init);u32 p=gabi::ea(a);s32 message=readAt<s32>(p,0x7C8);
    u32 actor=gabi::load<u32>(0x101F4B5C);u16 status=0xFF;
    if(message==-1) {
        u32 number;
        if(init==1) { number=gabi::call_ptr<u32>(readAt<u32>(readAt<u32>(p,0xB4),0x1C),a);writeAt<u32>(p,0x7C0,number); }
        else number=readAt<u32>(p,0x7C0);
        s32 id=gabi::call<s32>(0x025F7DB0,actor,number,p+0x37C);writeAt<s32>(p,0x7C8,id);
        if(id!=-1) { writeAt<u8>(p,0x7CC,0);writeAt<u16>(p,0x8A8,0xFFFF); }
    } else if(!readAt<u8>(p,0x7CC)) writeAt<u8>(p,0x7CC,1);
    else {
        status=gabi::call<u16>(0x025F795C,actor);
        if(status==0xE) {
            u32 next=gabi::call_ptr<u32>(readAt<u32>(readAt<u32>(p,0xB4),0x14),a,p+0x7C0);
            gabi::call(0x025F74D0,actor,next);
            if(gabi::call<u32>(0x025F795C,actor)==0xF)gabi::call(0x025F7DB0,actor,readAt<u32>(p,0x7C0),0);
        } else if(status==0x12) { gabi::call(0x025F74D0,actor,0x13);writeAt<s32>(p,0x7C8,-1); }
        writeAt<u16>(p,0x8A8,status);gabi::call_ptr<void>(readAt<u32>(readAt<u32>(p,0xB4),0x24),a,status);
    }
    return status;
}
VERIFY(0x022A10BC,daNpcMt_talk2);
void daNpcMt_privateCut(daNpcMt_c* a) {
    WWHD_FUNC(0x022A12FC,void,a);u32 p=gabi::ea(a),staffName=gabi::load<u32>(0x101C2318);
    s32 staff=gabi::call<s32>(0x02542D88,playObject()+0x52C4,staffName,0,0);if(staff==-1)return;
    s8 action=gabi::call<s8>(0x02542EDC,playObject()+0x52C4,staff,0x101C24F4,2,1,0);writeAt<s8>(p,0x8C1,action);
    u32 event=playObject()+0x52C4;
    if(action==-1) { gabi::call(0x02543280,event,staff);return; }
    if(gabi::call<s32>(0x025447C8,event,staff)) {
        action=readAt<s8>(p,0x8C1);
        if(action==0)daNpcMt_eventMesSetInit(a,staff);
        else if(action==1)daNpcMt_eventGetItemInit(a);
        else { gabi::call(0x02543280,playObject()+0x52C4,staff);return; }
    }
    action=readAt<s8>(p,0x8C1);
    if(action!=0||daNpcMt_eventMesSet(a))gabi::call(0x02543280,playObject()+0x52C4,staff);
}
VERIFY(0x022A12FC,daNpcMt_privateCut);
BOOL daNpcMt_draw(daNpcMt_c* a) {
    WWHD_FUNC(0x022A1BA0,BOOL,a);u32 p=gabi::ea(a),model=readAt<u32>(readAt<u32>(p,0x44C),0x90),data=readAt<u32>(model,0xAC);
    u32 environment=gabi::call<u32>(0x02555D0C);gabi::call(0x025626A4,environment,0,p+0x314,p+0x110);
    environment=gabi::call<u32>(0x02555D0C);gabi::call(0x02562F5C,environment,model,p+0x110);
    gabi::call(0x025E7B3C,p+0x7F0,data,readAt<u8>(p,0x8B9));gabi::call(0x025E54D8,readAt<u32>(p,0x44C));
    writeAt<u32>(data,0x38,0);gabi::call(0x025BED80,0xA7,a,1.0f,1.0f,1.0f);return TRUE;
}
VERIFY(0x022A1BA0,daNpcMt_draw);

u16 daNpcMt_next_msgStatus(daNpcMt_c* a,void* messageOut) {
    WWHD_FUNC(0x022A1DF4,u16,a,messageOut);u32 p=gabi::ea(a),table=readAt<u32>(p,0x888);
    if(!table)return 0x10;
    table+=4;writeAt<u32>(p,0x888,table);u32 message=readAt<u32>(table,0);
    if(message==2) {
        u8 selected=readAt<u8>(playObject(),0x5BEB);
        u32 album=gabi::call<u32>(0x02720144,saveObject()+0x12C0);gabi::call(0x02725E0C,album,selected);
        u32 play=playObject();writeAt<u16>(play,0x5B66,(u16)(readAt<s16>(play,0x5B66)-1));
    }
    if(message==0||message==2) { writeAt<u32>(p,0x888,0);return 0x10; }
    if(message==1) {
        if(daNpcMt_getPhotoSelection(a)>1)message=0x3676;
        else {
            u32 room=gabi::call<u32>(0x025BD64C,daNpcMt_getPhoto(a,0));
            if(room>=8)gabi::call(0x0273AA24,0x1001EB28,0x69E,0x1001EAEC);
            message=gabi::load<u32>(0x101C2370+room*4);
        }
    }
    writeAt<u32>(gabi::ea(messageOut),0,message);return 0xF;
}
VERIFY(0x022A1DF4,daNpcMt_next_msgStatus);
void daNpcMt_makeFigures(daNpcMt_c* a) {
    WWHD_FUNC(0x022A2078,void,a);u32 count=daNpcMt_getPhotoSelection(a);
    for(u32 i=0;i<count;++i) {
        u32 figure=daNpcMt_getPhoto(a,(u8)i);daNpcMt_setFigure(a,figure);
        if(figure==gabi::call<u8>(0x025BD600,0x4A))daNpcMt_setFigure(a,gabi::call<u8>(0x025BD600,0x49));
        else if(figure==gabi::call<u8>(0x025BD600,0x72)) {
            for(u32 photo=0x73;photo<=0x78;++photo)daNpcMt_setFigure(a,gabi::call<u8>(0x025BD600,photo));
        } else if(figure==gabi::call<u8>(0x025BD600,0x8C))daNpcMt_setFigure(a,gabi::call<u8>(0x025BD600,0x8D));
        else if(figure==gabi::call<u8>(0x025BD600,0x88))daNpcMt_setFigure(a,gabi::call<u8>(0x025BD600,0x87));
    }
}
VERIFY(0x022A2078,daNpcMt_makeFigures);
static void jointParameters(u32 joint) {
    s16 params[9];for(u32 i=0;i<9;++i)params[i]=gabi::load<s16>(0x101C249C+i*2);
    gabi::call(0x0259E08C,joint,params[2],params[3],params[6],params[7],params[0],params[1],params[4],params[5],params[8]);
}
s32 daNpcMt_createInit(daNpcMt_c* a) {
    WWHD_FUNC(0x022A0618,s32,a);u32 p=gabi::ea(a);writeAt<f32>(p,0x374,-9.0f);daNpcMt_setAnmTbl(a,gabi::at<void>(0x101C2330));
    s16 event=gabi::call<s16>(0x02543F10,playObject()+0x52C4,0x1001EAA4,0xFF);writeAt<s16>(p,0x89E,event);
    writeAt<u32>(p,0x104,0x022A04A4);gabi::call(0x0259F814,p+0x3E0,gabi::load<u32>(0x101C2318),a);
    u32 morph=readAt<u32>(p,0x44C);
    for(u32 off:{0x8B6u,0x8C6u,0x8C7u,0x8B7u})writeAt<u8>(p,off,0);writeAt<u16>(p,0x8B2,0);
    u32 model=readAt<u32>(morph,0x90);writeAt<u32>(p,0x348,model?model+0xC8:0);
    gabi::call(0x025D674C,a,-70.0f,0.0f,-70.0f,70.0f,200.0f,70.0f);
    writeAt<u8>(p,0x389,0x6E);writeAt<u8>(p,0x38B,0x6E);writeAt<u32>(p,0x39C,0x0100000A);jointParameters(p+0x3AC);
    writeAt<u8>(p,0x8C4,gabi::load<u8>(0x101C24E6));writeAt<u8>(p,0x8C5,gabi::load<u8>(0x101C24E7));
    writeAt<f32>(p,0x894,gabi::load<f32>(0x101C24BC));writeAt<s16>(p,0x8A4,gabi::load<s16>(0x101C24C4));
    gabi::call(0x024F08A8,p+0x450,playObject()+0x12A0);daNpcMt_setMtx(a);
    gabi::call(0x027F4D5C,readAt<u32>(readAt<u32>(p,0x44C),0x90));gabi::call(0x02515F14,p+0x654,0xFF,0xFF,a);
    gabi::call(0x02516518,p+0x690,0x101EA190);
    gabi::Local<cXyz> center;center->x=readAt<f32>(p,0x314);center->z=readAt<f32>(p,0x31C);writeAt<u32>(p,0x6D4,p+0x654);center->y=readAt<f32>(p,0x318);
    daNpcMt_setCollision(a,gabi::at<void>(p+0x690),center,gabi::load<f32>(0x101C24CC),150.0f);return 4;
}
VERIFY(0x022A0618,daNpcMt_createInit);
void daNpcMt_lookBack(daNpcMt_c* a) {
    WWHD_FUNC(0x022A1708,void,a);u32 p=gabi::ea(a);
    s8 mode=readAt<s8>(p,0x8C3);f32 x=readAt<f32>(p,0x37C),y=readAt<f32>(p,0x380),z=readAt<f32>(p,0x384);
    s16 yaw=readAt<s16>(p,0x322),targetAngle=readAt<s16>(p,0x8B0);u32 enabled=readAt<u8>(p,0x89C);
    gabi::Local<cXyz> target,eye;cXyz* targetPtr=nullptr;
    if(mode==1) { for(u32 i=0;i<3;++i)gabi::store<u32>(gabi::ea((cXyz*)target)+4*i,readAt<u32>(p,0x878+4*i));targetPtr=target; }
    else if(mode==2)yaw=readAt<s16>(p,0x8B4);
    bool forced=readAt<u8>(p,0x8B6)&&readAt<u8>(p,0x8C4);
    if(forced) { writeAt<u8>(p,0x3B6,1);enabled=0; }
    s16 turn=0;
    if(forced||readAt<u8>(p,0x3B6)) {
        s16 eventAngle=readAt<s16>(p,0x442);if(eventAngle)targetAngle=eventAngle;
        gabi::call(0x0200F428,p+0x8B2,targetAngle,4,0x800);turn=readAt<s16>(p,0x8B2);
    } else writeAt<u16>(p,0x8B2,0);
    eye->x=x;eye->y=y;eye->z=z;
    gabi::call(0x0259DED0,p+0x3AC,p+0x322,targetPtr,(cXyz*)eye,yaw,turn,enabled);
    u16 ax=readAt<u16>(p,0x320),ay=readAt<u16>(p,0x322),az=readAt<u16>(p,0x324);
    writeAt<u16>(p,0x328,ax);writeAt<u16>(p,0x32C,az);writeAt<u16>(p,0x32A,ay);
}
VERIFY(0x022A1708,daNpcMt_lookBack);
BOOL daNpcMt_execute(daNpcMt_c* a) {
    WWHD_FUNC(0x022A1914,BOOL,a);u32 p=gabi::ea(a);gabi::call(0x022A0B20,a);daNpcMt_checkOrder(a);
    if(readAt<u8>(playObject(),0x5292)&&readAt<u16>(p,0xF8)!=1)daNpcMt_eventMove(a);
    else {
        u32 entry=0x101C2420+readAt<u8>(p,0x8BA)*8,self=p+readAt<s16>(entry,0);s16 index=readAt<s16>(entry,2);u32 target;
        if(index<0)target=readAt<u32>(entry,4);
        else target=readAt<u32>(readAt<u32>(self,readAt<s16>(entry,6))+index*8,4);
        gabi::call_ptr<void>(target,self);
    }
    daNpcMt_eventOrder(a);daNpcMt_playTexPatternAnm(a);daNpcMt_playAnm(a);
    gabi::Local<cXyz> center;center->x=readAt<f32>(p,0x314);center->y=readAt<f32>(p,0x318);center->z=readAt<f32>(p,0x31C);
    daNpcMt_setCollision(a,gabi::at<void>(p+0x690),center,gabi::load<f32>(0x101C24CC),150.0f);
    f32 x=readAt<f32>(p,0x314),y=readAt<f32>(p,0x318),z=readAt<f32>(p,0x31C);
    f32 offset=gabi::load<f32>(0x101C24B4);writeAt<f32>(p,0x398,z);writeAt<f32>(p,0x390,x);writeAt<f32>(p,0x394,y+offset);
    offset=gabi::load<f32>(0x101C24B8);writeAt<f32>(p,0x384,z);writeAt<f32>(p,0x37C,x);writeAt<f32>(p,0x380,y+offset);
    daNpcMt_lookBack(a);daNpcMt_setMtx(a);return FALSE;
}
VERIFY(0x022A1914,daNpcMt_execute);

BOOL daNpcMt_nodeCallback(void* node,s32 phase) {
    WWHD_FUNC(0x0229F85C,BOOL,node,phase);
    if(phase)return TRUE;
    u32 system=gabi::load<u32>(0x104B462C),actor=readAt<u32>(system,0xB8);
    u16 joint=readAt<u16>(gabi::call<u32>(0x027F7878,node),4);
    u32 matrices=readAt<u32>(system,0x2C);writeAt<u16>(matrices,4,readAt<u16>(matrices,4)|0x10);
    gabi::call(0x028E90D4,readAt<u32>(matrices,0x10)+joint*0x30,gabi::load<u32>(0x1018C7B0));
    u32 turn=0;
    if(joint==(u32)readAt<s8>(actor,0x3B4))turn=0x3AC;
    else if(joint==(u32)readAt<s8>(actor,0x3B5))turn=0x3B0;
    if(turn) {
        gabi::call(0x025F1BF4,gabi::load<u32>(0x1018C7B0),readAt<s16>(actor,turn+2));
        gabi::call(0x025F1C5C,gabi::load<u32>(0x1018C7B0),(s16)-readAt<s16>(actor,turn));
    }
    matrices=readAt<u32>(system,0x2C);u32 source=gabi::load<u32>(0x1018C7B0),destination=readAt<u32>(matrices,0x10)+joint*0x30;
    writeAt<u16>(matrices,4,readAt<u16>(matrices,4)|0x10);
    f32 values[12];for(u32 i=0;i<12;++i)values[i]=readAt<f32>(source,i*4);
    for(u32 i=0;i<12;++i)writeAt<f32>(destination,i*4,values[i]);
    gabi::call(0x028E90D4,gabi::load<u32>(0x1018C7B0),0x104B4868);return TRUE;
}
VERIFY(0x0229F85C,daNpcMt_nodeCallback);
BOOL daNpcMt_createHeap(daNpcMt_c* a) {
    WWHD_FUNC(0x0229FABC,BOOL,a);u32 p=gabi::ea(a);
    u32 data=objectResource(gabi::load<u32>(0x1001E984));
    u32 animation=objectResource(gabi::load<u32>(0x1001E9D0+readAt<u8>(p,0x8BE)*4));
    u32 morph=gabi::call<u32>(0x025E4F64,0,data,0,0,animation,2,1.0f,0,-1,1,0,0x80000,0x11020022);writeAt<u32>(p,0x44C,morph);
    for(u32 part=0;part<2;++part) {
        u32 names=gabi::call<u32>(0x027F68FC,data),offset=readAt<u32>(names,0x10);
        s8 joint=gabi::call<s8>(0x027DF9B0,offset?names+0x10+offset:0,part?0x1001EA50:0x1001EA1C);
        writeAt<s8>(p,part?0x3B5:0x3B4,joint);
        if(joint<0)gabi::call(0x0273AA24,0x1001EA24,part?0x3B8:0x3B4,part?0x1001EA5C:0x1001EA34);
    }
    if(!daNpcMt_initTexPatternAnm(a,0))return FALSE;
    u16 i=0,count=readAt<u16>(gabi::call<u32>(0x027F3F94,data),8);
    while(i<count) {
        if((u32)i==(u32)readAt<s8>(p,0x3B4)||(u32)i==(u32)readAt<s8>(p,0x3B5)) {
            u32 entry=readAt<u32>(data,8);if(i<readAt<u32>(data,4))entry+=i*0x1C;
            writeAt<u32>(entry,8,0x0229F85C);
        }
        ++i;count=readAt<u16>(gabi::call<u32>(0x027F3F94,data),8);
    }
    morph=readAt<u32>(p,0x44C);if(readAt<u32>(morph,0x90))writeAt<u32>(readAt<u32>(morph,0x90),0xB8,p);
    gabi::call(0x024EFF44,p+0x614,30.0f,30.0f);
    gabi::call(0x024F06B4,p+0x450,p+0x314,p+0x300,a,1,p+0x614,p+0x33C,p+0x320,p+0x328);return TRUE;
}
VERIFY(0x0229FABC,daNpcMt_createHeap);
static bool sameStage(SafeString* first,SafeString* second) {
    gabi::call_ptr<void>(readAt<u32>(first->__vtbl.get(),0x14),first);
    gabi::call_ptr<void>(readAt<u32>(first->__vtbl.get(),0x14),first);
    u32 string=first->mStringTop.get();
    gabi::call_ptr<void>(readAt<u32>(second->__vtbl.get(),0x14),second);
    if(string==second->mStringTop.get())return true;
    string=first->mStringTop.get();u32 other=second->mStringTop.get();
    for(u32 i=0;i<0x40001;++i) { u8 c=readAt<u8>(string,i);if(c!=readAt<u8>(other,i))return false;if(!c)return true; }
    return false;
}
BOOL daNpcMt_delete(daNpcMt_c* a) {
    WWHD_FUNC(0x022A08D8,BOOL,a);u32 p=gabi::ea(a);gabi::call(0x025204C8,p+0x7DC,gabi::load<u32>(0x101C231C));
    if(readAt<u32>(p,0xF4)) { u32 morph=readAt<u32>(p,0x44C);if(morph)gabi::call(0x025E563C,morph); }
    if(!readAt<s8>(playObject(),0x514C))return TRUE;
    for(u32 room=1;room<8;++room) {
        gabi::Local<SafeString> desired,next;desired->mStringTop=gabi::load<u32>(0x101C23F0+room*4);desired->__vtbl=0x1001E998;
        next->mStringTop=playObject()+0x5140;next->__vtbl=0x1001E998;
        if(sameStage(desired,next)) {
            if(!gabi::call<s32>(0x025B8B94,saveObject()+0x644,0x4080))return TRUE;
            u8 i=0;while(i<daNpcMt_getPhotoSelection(a)) {
                if(gabi::call<u32>(0x025BD64C,daNpcMt_getPhoto(a,i))==room)gabi::call(0x025B8B68,saveObject()+0x644,0x3F01);
                ++i;
            }
            return TRUE;
        }
    }
    gabi::call(0x025B8B7C,saveObject()+0x1178,0x202);
    if(gabi::call<s32>(0x025B8B94,saveObject()+0x644,0x4080)) {
        for(u32 event:{0x3080u,0x2F01u,0x4080u,0x3F01u,0x4040u})gabi::call(0x025B8B7C,saveObject()+0x644,event);
        daNpcMt_clearPhotoSelection(a);
    }
    return TRUE;
}
VERIFY(0x022A08D8,daNpcMt_delete);

static void faceHome(u32 p) {
    writeAt<u8>(p,0x89C,0);writeAt<s16>(p,0x8B4,readAt<s16>(p,0x8A6));writeAt<u8>(p,0x8C3,2);writeAt<u8>(p,0x3B6,1);
}
static void playerGaze(u32 p,cXyz* eye) {
    u32 e=gabi::ea(eye);u8 turn=readAt<u8>(p,0x8C4),look=readAt<u8>(p,0x8C5);
    u32 x=readAt<u32>(e,0),y=readAt<u32>(e,4),z=readAt<u32>(e,8);
    writeAt<u8>(p,0x89C,turn==0);writeAt<u32>(p,0x880,z);writeAt<u32>(p,0x87C,y);writeAt<u8>(p,0x8C3,1);writeAt<u32>(p,0x878,x);
    if(!look)faceHome(p);
}
void daNpcMt_chkAttention(daNpcMt_c* a) {
    WWHD_FUNC(0x022A0B20,void,a);u32 p=gabi::ea(a);u8 event=readAt<u8>(p,0x440);writeAt<u8>(p,0x8C6,0);
    bool close=false;
    if(event) {
        f32 z=readAt<f32>(p,0x43C),y=readAt<f32>(p,0x438);u8 turn=readAt<u8>(p,0x8C4);
        writeAt<f32>(p,0x880,z);writeAt<u8>(p,0x8C3,1);writeAt<f32>(p,0x87C,y);writeAt<f32>(p,0x878,readAt<f32>(p,0x434));
        if(turn) { writeAt<u8>(p,0x89C,0);writeAt<u8>(p,0x3B6,1); }else writeAt<u8>(p,0x89C,1);
        close=true;
    } else {
        u32 player=readAt<u32>(playObject(),0x5B34);gabi::Local<cXyz> origin,target;gabi::Local<be<f32>> distance;gabi::Local<be<s16>> angle;
        origin->x=readAt<f32>(p,0x314);origin->y=readAt<f32>(p,0x318);origin->z=readAt<f32>(p,0x31C);
        target->x=readAt<f32>(player,0x314);target->y=readAt<f32>(player,0x318);target->z=readAt<f32>(player,0x31C);
        f32 radius=readAt<f32>(p,0x894);s32 cone=readAt<s16>(p,0x8A4);
        gabi::call(0x0259D624,(cXyz*)origin,(cXyz*)target,(be<f32>*)distance,(be<s16>*)angle);
        u8 wasNear=readAt<u8>(p,0x8B7);s16 difference=(s16)(angle.get()->get()-readAt<s16>(p,0x32A));
        if(wasNear) { radius+=40.0f;cone+=0x71C; }*angle=difference;
        f32 separation=distance.get()->get();s32 absAngle=difference<0?-(s32)difference:difference;
        if(radius>separation&&cone>absAngle) {
            gabi::Local<cXyz> eye;gabi::call(0x0259D54C,(cXyz*)eye,gabi::load<f32>(0x101C24B0));playerGaze(p,eye);close=true;
        } else {
            if(wasNear==1) { writeAt<u8>(p,0x8B7,0);writeAt<s16>(p,0x8A2,gabi::load<s16>(0x101C24E4)); }
            if(gabi::load<f32>(0x101C24C0)>separation||readAt<u8>(p,0x8C7)) {
                gabi::Local<cXyz> eye;gabi::call(0x0259D54C,(cXyz*)eye,gabi::load<f32>(0x101C24B0));playerGaze(p,eye);writeAt<u8>(p,0x8C6,1);
            } else {
                u32 message=readAt<u32>(p,0x864);writeAt<u8>(p,0x8C3,0);
                if(!message) {
                    s16 timer=readAt<s16>(p,0x8A2);
                    if(timer)writeAt<u16>(p,0x8A2,(u16)(timer-1));else faceHome(p);
                }
            }
        }
    }
    if(close&&!readAt<u8>(p,0x8B7))writeAt<u8>(p,0x8B7,1);
    writeAt<s16>(p,0x8B0,gabi::load<s16>(0x101C24C6));
}
VERIFY(0x022A0B20,daNpcMt_chkAttention);

u32 daNpcMt_getMsg(daNpcMt_c* a) {
    WWHD_FUNC(0x022A2208,u32,a);u32 p=gabi::ea(a);writeAt<u32>(p,0x888,0);
    u8 showingPhoto=readAt<u8>(playObject(),0x52B2);u32 inventory=playObject()+0x12A0;
    auto table=[&](u32 t) { writeAt<u32>(p,0x888,t);return t?gabi::load<u32>(t):0; };
    auto event=[&](u32 bit) { return gabi::call<s32>(0x025B8B94,saveObject()+0x644,bit)!=0; };
    auto on=[&](u32 bit) { gabi::call(0x025B8B68,saveObject()+0x644,bit); };
    auto off=[&](u32 bit) { gabi::call(0x025B8B7C,saveObject()+0x644,bit); };
    if(showingPhoto) {
        u32 photo=daNpcMt_changePhotoNo(readAt<u8>(inventory,0x4946)),figure=0xFF;
        if(photo-0x49<0x86)figure=gabi::call<u8>(0x025BD600,photo);
        if(!readAt<u8>(playObject(),0x5BEA))return table(0x101C22D0);
        if(figure==0xFF)return table(0x101C22D8);
        if(daNpcMt_isFigureGet(a,figure))return table(0x101C22E0);
        if(daNpcMt_hasPhoto(a,figure))return table(0x101C22E8);
        if(readAt<u8>(playObject(),0x5BE7)&1)return table(0x101C22F0);
        if(readAt<u8>(playObject(),0x5BE7)&2)return table(0x101C22F8);
        if(event(0x4080)) { for(u32 bit:{0x3080u,0x4080u,0x3F01u,0x4040u})off(bit);daNpcMt_clearPhotoSelection(a); }
        daNpcMt_submitPhoto(a,figure);
        u32 experienced=event(0x3A01)||event(0x3401);
        u32 choices=figure==0x5E?0x101C2308:figure==0x5F?0x101C2310:0x101C2300;
        writeAt<u32>(p,0x888,gabi::load<u32>(choices+experienced*4));on(0x2F01);on(0x3401);
    } else {
        u32 equipped=readAt<u8>(inventory,0x4010);
        if(equipped-1<=3) { u32 t=readAt<u32>(p,0x888);return t?gabi::load<u32>(t):0; }
        if(event(0x3080)) {
            if(!event(0x4080)) {
                writeAt<u32>(p,0x888,event(0x3A01)?0x101C2464:0x101C2450);daNpcMt_makeFigures(a);on(0x4080);off(0x2F01);
            } else {
                if(!event(0x3F01))return table(0x101C22B8);
                if(!event(0x4040)) { on(0x4040);return table(0x101C22C0); }
                writeAt<u32>(p,0x888,0x101C22C8);
                for(u32 bit:{0x3080u,0x4080u,0x3F01u,0x4040u})off(bit);
                gabi::call(0x025B8B68,saveObject()+0x1178,0x202);daNpcMt_clearPhotoSelection(a);
            }
        } else if(event(0x2F01))return table(daNpcMt_getPhotoSelection(a)>=0xC?0x101C22A8:0x101C22B0);
        else if(!event(0x2F02)) { on(0x2F02);return table(0x101C2340); }
        else if(!event(0x3A01))return table(0x101C2360);
        else if(!gabi::call<s32>(0x025B8B94,saveObject()+0x1178,0x202)) {
            gabi::call(0x025B8B68,saveObject()+0x1178,0x202);return table(0x101C2298);
        } else return table(0x101C22A0);
    }
    u32 selected=readAt<u32>(p,0x888);return selected?gabi::load<u32>(selected):0;
}
VERIFY(0x022A2208,daNpcMt_getMsg);
