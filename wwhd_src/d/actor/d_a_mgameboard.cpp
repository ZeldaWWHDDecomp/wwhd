// Squid Hunt board. Actual HD TU021C41B0..021C5CF7.
#include "d/actor/d_a_mgameboard.h"
#include "bindings.h"
namespace {
template<class T> T read(u32 a) { return *gabi::at<be<T>>(a); }
template<class T> void write(u32 a,T v) { *gabi::at<be<T>>(a)=v; }
void* ptr(u32 a) { return gabi::at<void>(a); }
struct Archive { be<u32> name, vtable; };
u32 resource(s32 index,s32 assertionLine) {
    gabi::Local<Archive> name;
    name->name=0x101BA5E8; name->vtable=0x10014C14;
    u32 data=gabi::call<u32>(0x026066C4,ptr(read<u32>(0x101F4F28)),name.get(),index);
    if(!data) gabi::call(0x0273AA24,ptr(0x10014C48),assertionLine,ptr(0x10014C5C));
    return data;
}
void copyMatrix(u32 model) {
    // All twelve loads precede the stores, including for aliased model storage.
    f32 values[12];
    for(u32 i=0;i<12;++i) values[i]=read<f32>(0x1048D0CC+i*4);
    for(u32 i=0;i<12;++i) write<f32>(model+0xC8+i*4,values[i]);
}
void transform(daMgBoard_c* board,u32 model,u32 grid,s32 rotation=0,bool rotate=false) {
    f32 x=board->current.pos.x, y=board->current.pos.y,z=board->current.pos.z;
    if(grid) { x=x+read<f32>(grid); y=y+read<f32>(grid+4); z=z+read<f32>(grid+8); }
    gabi::call(0x028E93CC,ptr(0x1048D0CC),x,y,z);
    gabi::call(0x025F1C28,ptr(0x1048D0CC),(s16)board->current.angle.y);
    if(rotate) gabi::call(0x025F1C5C,ptr(0x1048D0CC),rotation);
    copyMatrix(model);
}
void markersAndShips(daMgBoard_c* board) {
    u32 a=gabi::ea(board);
    board->missCount=0; board->hitCount=0;
    for(u32 x=0;x<8;++x) for(u32 y=0;y<8;++y) {
        u8 cell=read<u8>(a+0x598+x*8+y); u32 model=0;
        if(cell==3) { s32 i=board->hitCount; model=read<u32>(a+0x3BC+(u32)i*4); board->hitCount=i+1; }
        else if(cell==1) { s32 i=board->missCount; model=read<u32>(a+0x40C+(u32)i*4); board->missCount=i+1; }
        if(model) transform(board,model,0x1046500C+x*12+y*0x60);
    }
    u32 count=read<u8>(a+0x614);
    for(u32 i=0;i<count;++i) {
        if(i>=4) continue;
        u32 ship=a+0x5D8+i*15;
        u8 length=read<u8>(ship+8);
        u32 model=length==2?(u32)board->shipModels[0]:length==3?(u32)board->shipModels[2]:length==4?(u32)board->shipModels[4]:0;
        if(!model) continue;
        u8 y=read<u8>(ship+12),x=read<u8>(ship+11);
        if(y>=8||x>=8) continue;
        f32 px=board->current.pos.x+read<f32>(0x1046500C+y*0x60+x*12);
        f32 py=board->current.pos.y+read<f32>(0x10465010+y*0x60+x*12);
        f32 pz=board->current.pos.z+read<f32>(0x10465014+y*0x60+x*12);
        gabi::call(0x028E93CC,ptr(0x1048D0CC),px,py,pz);
        gabi::call(0x025F1C28,ptr(0x1048D0CC),(s16)board->current.angle.y);
        s32 rotation=read<s8>(ship+14)?-0x8000:0x4000;
        gabi::call(0x025F1C5C,ptr(0x1048D0CC),rotation);
        copyMatrix(model);
    }
}
void matrices(daMgBoard_c* board) {
    u32 model=board->boardModel;
    transform(board,model,0);
    u32 grid=0x1046500C+(u32)(s32)board->cursorY*0x60+(u32)(s32)board->cursorX*12;
    model=board->cursorModel;
    transform(board,model,grid);
    markersAndShips(board);
}
void setDrawBuffer(u32 off) {
    u32 play=gabi::call<u32>(0x025200D4);
    write<u32>(0x104B4634,read<u32>(play+off));
    play=gabi::call<u32>(0x025200D4);
    write<u32>(0x104B4638,read<u32>(play+off+4));
}
void lightModel(daMgBoard_c* board,u32 model) {
    auto light=dKy_getEnvlight();
    setLightTevColorType(light,gabi::at<J3DModel>(model),&board->tevStr);
}
}
BOOL mgboard_CreateHeap(daMgBoard_c* board) {
    WWHD_FUNC(0x021C41B0,BOOL,board);
    u32 data=resource(8,0xC6);
    board->boardModel=gabi::call<u32>(0x025E38E0,ptr(data),0x80000,0x11000022);
    if(!board->boardModel) return 0;
    data=resource(9,0xD6);
    board->cursorModel=gabi::call<u32>(0x025E38E0,ptr(data),0x80000,0x11000022);
    if(!board->cursorModel) return 0;
    data=resource(7,0xE6);
    for(u32 i=0;i<20;++i) { u32 model=gabi::call<u32>(0x025E38E0,ptr(data),0x80000,0x11000022); board->hitModels[i]=model; if(!model)return 0; }
    data=resource(10,0xF8);
    for(u32 i=0;i<32;++i) { u32 model=gabi::call<u32>(0x025E38E0,ptr(data),0x80000,0x11000022); board->missModels[i]=model; if(!model)return 0; }
    for(u32 group=0;group<3;++group) {
        data=resource((s32)group+4,group==0?0x10A:group==1?0x11F:0x134);
        for(u32 i=0;i<2;++i) {
            u32 model=gabi::call<u32>(0x025E38E0,ptr(data),0x80000,0x11000022);
            board->shipModels[group*2+i]=model;
            if(!model)return 0;
            gabi::call(0x027F58E0,ptr(model),1);
        }
    }
    return 1;
}
VERIFY(0x021C41B0,mgboard_CreateHeap);
BOOL mgboard_CheckCreateHeap(daMgBoard_c* board) { WWHD_FUNC(0x021C44E0,BOOL,board); return mgboard_CreateHeap(board); }
VERIFY(0x021C44E0,mgboard_CheckCreateHeap);
void mgboard_MiniGameInit(daMgBoard_c* board) {
    WWHD_FUNC(0x021C44E4,void,board);
    board->cursorY=0; board->lastFireX=-100; board->lastFireY=-100; board->cursorX=0;
    u32 a=gabi::ea(board); write<u16>(a+0x590,0);write<u32>(a+0x594,0);
    gabi::call(0x025BB358,ptr(a+0x598),24,3);
    gabi::Local<be<s16>> npcType; *npcType=0x16A;
    u32 npc=gabi::call<u32>(0x025D5218,ptr(0x025E121C),npcType.get());
    if(npc) { write<u32>(a+0x6D0,read<u32>(npc+0x314));write<u32>(a+0x6D4,read<u32>(npc+0x318));write<u32>(a+0x6D8,read<u32>(npc+0x31C)); }
    matrices(board);
}
VERIFY(0x021C44E4,mgboard_MiniGameInit);
void mgboard_CreateInit(daMgBoard_c* board) {
    WWHD_FUNC(0x021C4958,void,board);
    u32 model=board->boardModel;board->cullMtx=model?model+0xC8:0;
    gabi::call(0x025D674C,board,-600.f,-300.f,-500.f,600.f,300.f,100.f);
    board->state=0;
    u32 play=gabi::call<u32>(0x025200D4);
    board->startEvent=gabi::call<s32>(0x02543F10,ptr(play+0x52C4),ptr(0x10014C90),255);
    play=gabi::call<u32>(0x025200D4);
    board->endEvent=gabi::call<s32>(0x02543F10,ptr(play+0x52C4),ptr(0x10014CA0),255);
    gabi::call(0x025885C4,board->stickControl,5,2,3,2,0,0x800,0.8999999761581421f,0.5f);
    mgboard_MiniGameInit(board);
}
VERIFY(0x021C4958,mgboard_CreateInit);
s32 mgboard_Create(daMgBoard_c* board) {
    WWHD_FUNC(0x021C4A4C,s32,board);
    u32 condition=board->actor_condition;
    if(!(condition&8)) {
        if(board) {
            fopAc_ac_c_ct(board);
            write<u32>(gabi::ea(board)+0x6CC,0x10050788);
            board->__vtbl=0x10014C2C;
            gabi::call(0x025885C4,board->stickControl,15,15,0,0,0,0x2000,0.8999999761581421f,0.5f);
            gabi::call(0x025885E8,board->stickControl);
            condition=board->actor_condition;
        }
        board->actor_condition=condition|8;
    }
    write<u32>(0x101BA5E8,read<u32>(0x10014C3C));write<u32>(0x101BA5EC,read<u32>(0x10014C40));write<u8>(0x101BA5F0,read<u8>(0x10014C44));
    s32 phase=gabi::call<s32>(0x02520460,&board->phase,ptr(0x101BA5E8));
    if(phase==4) { if(!gabi::call<s32>(0x025D63E8,board,ptr(0x021C44E0),0x4E000))return 5; mgboard_CreateInit(board); }
    return phase;
}
VERIFY(0x021C4A4C,mgboard_Create);
BOOL mgboard_Delete(daMgBoard_c* board) {
    WWHD_FUNC(0x021C4B70,BOOL,board);
    gabi::call(0x025204C8,&board->phase,ptr(0x101BA5E8));
    gabi::call(0x025E1B34,&board->npcPosition);
    return 1;
}
VERIFY(0x021C4B70,mgboard_Delete);
BOOL mgboard_Draw(daMgBoard_c* board) {
    WWHD_FUNC(0x021C4BB4,BOOL,board);
    auto light=dKy_getEnvlight();settingTevStruct(light,0,&board->current.pos,&board->tevStr);
    light=dKy_getEnvlight();setLightTevColorType(light,gabi::at<J3DModel>((u32)board->boardModel),&board->tevStr);
    mDoExt_modelUpdateDL(gabi::at<J3DModel>((u32)board->boardModel),0);
    if(!board->drawInfo)return 1;
    light=dKy_getEnvlight();settingTevStruct(light,0,&board->current.pos,&board->tevStr);
    light=dKy_getEnvlight();setLightTevColorType(light,gabi::at<J3DModel>((u32)board->cursorModel),&board->tevStr);
    setDrawBuffer(0x5D84);mDoExt_modelUpdateDL(gabi::at<J3DModel>((u32)board->cursorModel),0);setDrawBuffer(0x5D78);
    setDrawBuffer(0x5D84);
    u32 a=gabi::ea(board);
    for(s32 i=0;i<(s32)board->hitCount;++i) {
        light=dKy_getEnvlight();u32 model=read<u32>(a+0x3BC+(u32)i*4);
        setLightTevColorType(light,gabi::at<J3DModel>(model),&board->tevStr);
        model=read<u32>(a+0x3BC+(u32)i*4);mDoExt_modelUpdateDL(gabi::at<J3DModel>(model),0);
    }
    for(s32 i=0;i<(s32)board->missCount;++i) {
        light=dKy_getEnvlight();u32 model=read<u32>(a+0x40C+(u32)i*4);
        setLightTevColorType(light,gabi::at<J3DModel>(model),&board->tevStr);
        model=read<u32>(a+0x40C+(u32)i*4);mDoExt_modelUpdateDL(gabi::at<J3DModel>(model),0);
    }
    setDrawBuffer(0x5D78);
    for(u32 i=0;i<3;++i) {
        u8 remaining=read<u8>(a+0x614),bombs=read<u8>(a+0x615),length=read<u8>(a+0x5E0+i*15);
        if(remaining&&bombs)continue;
        u32 model=length==2?(u32)board->shipModels[0]:length==3?(u32)board->shipModels[2]:length==4?(u32)board->shipModels[4]:0;
        if(!model)continue;
        lightModel(board,model);setDrawBuffer(0x5D84);
        mDoExt_modelUpdateDL(gabi::at<J3DModel>(model),0);setDrawBuffer(0x5D78);
    }
    return 1;
}
VERIFY(0x021C4BB4,mgboard_Draw);
void mgboard_ResetGame(daMgBoard_c* board) { WWHD_FUNC(0x021C4DFC,void,board);mgboard_MiniGameInit(board); }
VERIFY(0x021C4DFC,mgboard_ResetGame);
void mgboard_StartInput(daMgBoard_c* board) { WWHD_FUNC(0x021C4E00,void,board);gabi::call(0x02618760,ptr(read<u32>(0x101F5088)),5,5); }
VERIFY(0x021C4E00,mgboard_StartInput);
u32 mgboard_CursorMove(daMgBoard_c* board) {
    WWHD_FUNC(0x021C4E14,u32,board);
    u32 pad=read<u32>(0x101F5088);s8 x=board->cursorX,y=board->cursorY,oldX=x,oldY=y;
    u32 buttons=read<u32>(pad+0x18)|read<u32>(pad+0x20);
    if(read<u32>(pad+0x124)&0x00F00000)buttons&=0x00F00000;
    if(buttons&0x00440000) { x=(s8)(x-1);board->cursorX=x; }
    else if(buttons&0x00880000) { x=(s8)(x+1);board->cursorX=x; }
    if(buttons&0x00110000) { y=(s8)((s8)board->cursorY+1);board->cursorY=y;x=board->cursorX; }
    else if(buttons&0x00220000) { y=(s8)((s8)board->cursorY-1);board->cursorY=y;x=board->cursorX; }
    if(x>7) { x=7;board->cursorX=x; } else if(x<0) {x=0;board->cursorX=x;}
    y=board->cursorY;
    if(y>7) {y=7;board->cursorY=y;x=board->cursorX;}else if(y<0){y=0;x=board->cursorX;board->cursorY=y;}
    if(x!=oldX||y!=oldY)return gabi::call<u32>(0x025E1988,0x8A9);
    return (u32)gabi::ea(board); /* original: r3 still the board */
}
VERIFY(0x021C4E14,mgboard_CursorMove);
BOOL mgboard_MinigameMain(daMgBoard_c* board) {
    WWHD_FUNC(0x021C4F34,BOOL,board);
    if(gabi::call<s32>(0x025E1B24,0x8A8))return 1;
    mgboard_CursorMove(board);
    u32 a=gabi::ea(board);u8 bombs=read<u8>(a+0x615),oldRemaining=read<u8>(a+0x614);
    s32 fire=gabi::call<s32>(0x02007898,0);
    if(fire&&bombs>0) {
        u8 x=read<u8>(a+0x584),y=read<u8>(a+0x585);
        s32 result=gabi::call<s32>(0x025BB470,ptr(a+0x598),x,y);
        s8 fireY=board->cursorY,fireX=board->cursorX;u8 remaining=read<u8>(a+0x614);
        board->lastFireY=fireY;
        u32 score=read<u32>(a+0x618);u8 total=read<u8>(a+0x616);
        board->lastFireX=fireX;
        if(result>=0) {
            gabi::call(0x025E1A04,0x69A2,&board->npcPosition,0);
            if(oldRemaining!=remaining&&remaining)gabi::call(0x025E1A04,0x8AA,&board->npcPosition,0);
            u32 play=gabi::call<u32>(0x025200D4);
            gabi::Local<cXyz> direction;direction->x=0.f;direction->y=1.f;direction->z=0.f;
            gabi::call(0x025CB374,ptr(play+0x599C),7,-33,direction.get());
        } else if(result==-1)gabi::call(0x025E1A04,0x69A3,&board->npcPosition,0);
        u32 screen=read<u32>(read<u32>(0x101F8344)+0x1F8);
        gabi::call(0x02624134,ptr(screen),total,score);
    }
    matrices(board);
    return 1;
}
VERIFY(0x021C4F34,mgboard_MinigameMain);
BOOL mgboard_execGameMain(daMgBoard_c* board) {
    WWHD_FUNC(0x021C546C,BOOL,board);
    mgboard_MinigameMain(board);
    u32 a=gabi::ea(board);
    if(!read<u8>(a+0x614)||!read<u8>(a+0x615)) {board->endGame=1;return 1;}
    return 0;
}
VERIFY(0x021C546C,mgboard_execGameMain);
void mgboard_EndInput(daMgBoard_c* board) { WWHD_FUNC(0x021C54D4,void,board);gabi::call(0x02618774,ptr(read<u32>(0x101F5088))); }
VERIFY(0x021C54D4,mgboard_EndInput);
BOOL mgboard_Execute(daMgBoard_c* board) {
    WWHD_FUNC(0x021C54E0,BOOL,board);
    gabi::call(0x025B8BB0,ptr(read<u32>(0x101F84DC)+0x644),0xBEFF);
    if(board->forceEnd) {mgboard_ResetGame(board);board->forceEnd=0;board->endGame=0;}
    switch((u32)(s32)board->state) {
    case 0:board->startGame=0;board->state=1;break;
    case 1:if(board->startGame)board->state=2;break;
    case 2:mgboard_StartInput(board);board->state=3;[[fallthrough]];
    case 3:
        mgboard_execGameMain(board);
        if(board->endGame) {mgboard_EndInput(board);board->timer=30;board->state=4;}
        break;
    case 4:
        if(!gabi::call<u8>(0x0207A9A0,&board->timer)) {
            u8 remaining=read<u8>(gabi::ea(board)+0x614);board->state=0;
            gabi::call(0x025E1A04,remaining?0x8AC:0x8AB,&board->npcPosition,0);
        }
        break;
    }
    return 1;
}
VERIFY(0x021C54E0,mgboard_Execute);
BOOL mgboard_ExecuteWrapper(daMgBoard_c* board) { WWHD_FUNC(0x021C5634,BOOL,board);return mgboard_Execute(board); }
VERIFY(0x021C5634,mgboard_ExecuteWrapper);
void mgboard_staticInit() {
    WWHD_FUNC(0x021C5638,void);
    for(u32 i=0;i<4;++i)write<u32>(0x10464FFC+i*4,0);
    gabi::call(0x028F026C,ptr(0x101BA594));
    write<f32>(0x10464FE0,-3.1415927410125732f);write<f32>(0x10464FE4,3.1415927410125732f);
    gabi::call(0x028ED6F8,ptr(0x10464FF8));gabi::call(0x028F026C,ptr(0x101BA5A0));
    gabi::call(0x028EAB2C,ptr(0x10464FF9));gabi::call(0x028F026C,ptr(0x101BA5AC));
    write<f32>(0x10464FF4,10000.f);
    for(u32 y=0;y<8;++y)for(u32 x=0;x<8;++x) {
        u32 cell=0x1046500C+y*0x60+x*12;
        write<f32>(cell,-87.5f+(f32)x*25.f);write<f32>(cell+4,-87.5f+(f32)y*25.f);write<f32>(cell+8,0.f);
    }
    write<f32>(0x10464FE8,50000.f);write<f32>(0x10464FF0,10000.f);write<f32>(0x10464FEC,50000.f);
    for(u32 column=0;column<3;++column)for(u32 row=0;row<8;++row) {
        u32 cell=0x1046530C+column*0x60+row*12;
        write<f32>(cell,-150.f-(f32)column*20.f);write<f32>(cell+4,90.f-(f32)row*20.f);write<f32>(cell+8,0.f);
    }
    for(u32 i=0;i<3;++i){write<f32>(0x1046542C+i*12,162.5f);write<f32>(0x10465430+i*12,87.5f-(f32)i*25.f);write<f32>(0x10465434+i*12,0.f);}
}
VERIFY(0x021C5638,mgboard_staticInit);
void mgboard_staticDtor(void* object,u32 flags) { WWHD_FUNC(0x021C5BF4,void,object,flags);if(object&&(flags&1))gabi::call(0x0273AF40,object); }
VERIFY(0x021C5BF4,mgboard_staticDtor);
BOOL mgboard_IsDelete(daMgBoard_c* board) { WWHD_FUNC(0x021C5C08,BOOL,board);return 1; }
VERIFY(0x021C5C08,mgboard_IsDelete);
void mgboard_destructor(daMgBoard_c* board,u32 flags) {
    WWHD_FUNC(0x021C5C10,void,board,flags);
    if(board){gabi::call(0x025886D8,board->stickControl,2);gabi::call(0x025D50BC,board,0);if(flags&1)gabi::call(0x0273AF40,board);}
}
VERIFY(0x021C5C10,mgboard_destructor);
void mgboard_emptyVirtual(void* object) { WWHD_FUNC(0x021C5C70,void,object); }
VERIFY(0x021C5C70,mgboard_emptyVirtual);
bool mgboard_checkClearGame(daMgBoard_c* board) { WWHD_FUNC(0x021C5C74,bool,board);return read<u8>(gabi::ea(board)+0x614)==0; }
VERIFY(0x021C5C74,mgboard_checkClearGame);
u8 mgboard_getScore(daMgBoard_c* board) { WWHD_FUNC(0x021C5C84,u8,board);return read<u8>(gabi::ea(board)+0x616); }
VERIFY(0x021C5C84,mgboard_getScore);
void mgboard_reqStartGame(daMgBoard_c* board) {
    WWHD_FUNC(0x021C5C8C,void,board);
    board->endGame=0;board->startGame=1;
    u32 screen=read<u32>(read<u32>(0x101F8344)+0x1F8);
    gabi::call(0x020063C0,ptr(screen+0x18),ptr(0x1048ED50));
}
VERIFY(0x021C5C8C,mgboard_reqStartGame);
bool mgboard_checkEndGame(daMgBoard_c* board) { WWHD_FUNC(0x021C5CB8,bool,board);return board->endGame!=0; }
VERIFY(0x021C5CB8,mgboard_checkEndGame);
void mgboard_setGInfoDraw(daMgBoard_c* board) { WWHD_FUNC(0x021C5CC8,void,board);board->drawInfo=1; }
VERIFY(0x021C5CC8,mgboard_setGInfoDraw);
void mgboard_clrGInfoDraw(daMgBoard_c* board) {
    WWHD_FUNC(0x021C5CD4,void,board);
    board->drawInfo=0;
    u32 screen=read<u32>(read<u32>(0x101F8344)+0x1F8);
    gabi::call(0x020063C0,ptr(screen+0x18),ptr(0x1048ED80));
}
VERIFY(0x021C5CD4,mgboard_clrGInfoDraw);

/* ---- leftover functions of the translation unit ---- */

/* 021C5CF8 __sinit_d_a_mgameboard_static_cpp (HD: the d_a_mgameboard_static.cpp unit's own static
 * initializer): the header statics with the zeroed object at +0x1C and the calls at +0x18/+0x19, then
 * two pairs of static floats (50000, 10000) at 10465458..10465464. */
static void __sinit_d_a_mgameboard_static_cpp() {
    WWHD_FUNC(0x021C5CF8, void);
    for (int i = 3; i >= 0; i--) gabi::store<u32>(0x1046546C + 4 * i, 0);
    __register_global_object(0x101BA5F4);
    gabi::store<f32>(0x10465450, -3.1415927f);
    gabi::store<f32>(0x10465454, 3.1415927f);
    gabi::call(0x028ED6F8, 0x10465468);
    __register_global_object(0x101BA600);
    gabi::call(0x028EAB2C, 0x10465469);
    __register_global_object(0x101BA60C);
    gabi::store<f32>(0x10465458, 50000.0f);
    gabi::store<f32>(0x10465460, 10000.0f);
    gabi::store<f32>(0x1046545C, 50000.0f);
    gabi::store<f32>(0x10465464, 10000.0f);
}
VERIFY(0x021C5CF8, __sinit_d_a_mgameboard_static_cpp);
