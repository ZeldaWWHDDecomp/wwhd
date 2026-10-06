#include "d_kankyo_rain_private.h"
namespace d_kankyo_rain_cpp {
static void drawWave(u32 ignored,u32 unused,u32 draw) {
    WWHD_FUNC(0x02574D38,void,ignored,unused,draw);
    if((s32)ld(draw+12)!=2)return;
    u32 packet=ld(environment()+0xAA0),camera=ld(gameInfo()+0x5AF8);
    f32 one=lf(0x1004F550);
    if(!(lf(environment()+0x9F4)<one))return;
    if(!ld(gameInfo()+0x5FA4))return;
    gabi::Local<Matrix_l> view,rotation;
    gabi::Local<SafeString_l> stage,current;
    gabi::Local<be<u32>> color,secondary;
    gabi::Local<Vec_l> input,output;
    gabi::Local<Quad_l> quad;
    gabi::call(0x028E91EC,ld(gameInfo()+0x5FA4)+0x1E4,view.get());
    f32 angle=gabi::fmuls_ppc((f32)gabi::load<s16>(camera+0x100),lf(0x1004F5F8));
    stage->text=0x1004FCC0;stage->vtable=0x1004F3AC;
    current->text=gameInfo()+0x5134;current->vtable=0x1004F3AC;
    safe_string_callback(stage.get());safe_string_callback(stage.get());
    u32 expected=(u32)stage->text;
    safe_string_callback(current.get());
    equal_terminated_strings(expected,(u32)current->text);
    gabi::call(0x025602F0,color.get(),secondary.get());
    gabi::call(0x0255F8A0);
    gabi::call(0x028E98C0,rotation.get(),90,gabi::fmuls_ppc(angle,lf(0x1004FB7C)));
    gabi::call(0x028E9108,view.get(),rotation.get(),view.get());
    for(u32 i=0;i<300;i++)gabi::store<u8>(packet+0x9DB90+i,0);
    f32 zero=lf(0x1004F528),uvScale=lf(0x1004F6D0),fadeScale=lf(0x1004FCBC),byteScale=lf(0x1004F5AC);
    f32 scalarOne=lf(0x1004F5D0),uvWide=lf(0x1004F8D8),uvTwo=lf(0x1004F52C),widthScale=lf(0x1004F8D4);
    s32 i=0;
    while(i<gabi::load<s16>(environment()+0x9F8)) {
        u32 particle=packet+0xA0+(u32)i*56;
        f32 centerX=gabi::fadds_ppc(lf(particle+12),lf(particle));
        f32 centerY=gabi::fadds_ppc(lf(particle+16),lf(particle+4));
        f32 centerZ=gabi::fadds_ppc(lf(particle+20),lf(particle+8));
        f64 sine=gabi::call<f64>(0x028F43F8,lf(particle+36));
        if(sine>zero) {
            f32 height=wave_multiply_double(gabi::fmuls_ppc(lf(environment()+0x9E4),lf(particle+28)),sine);
            f32 width=gabi::fmuls_ppc(lf(environment()+0x9E4),lf(particle+28));
            u32 env=environment();f32 fade=lf(particle+44),stretch=lf(env+0x9F0);
            height=gabi::fmuls_ppc(height,fade);
            f32 step=gabi::fmuls_ppc((f32)((u32)i*31),fadeScale);
            f32 widthFade=gabi::fnmsubs(step,height,fade);
            width=gabi::fmuls_ppc(gabi::fmuls_ppc(width,stretch),widthFade);
            if(height>zero) {
                gabi::store<u8>(packet+0x9DB90+(u32)i,1);
                gabi::store<u8>(gabi::ea(color.get())+3,(u8)gabi::ftoi(gabi::fmuls_ppc(byteScale,lf(particle+40))));
                gabi::call(0x02582518,packet,i,secondary.get(),color.get());
                f32 skewY=lf(packet+0x4244),skew=lf(packet+0x4240);
                f32 offset=gabi::fmuls_ppc(width,gabi::fmuls_ppc(widthScale,lf(particle+24)));
                f32 first=skewY<zero?gabi::fsubs_ppc(-gabi::fmuls_ppc(offset,skew),width):gabi::fmadds(offset,skew,-width);
                input->x=first;input->y=height;input->z=zero;
                gabi::call(0x028E8F64,view.get(),input.get(),output.get());
                f32 x0=gabi::fadds_ppc(centerX,(f32)output->x),y0=gabi::fadds_ppc(centerY,(f32)output->y),z0=gabi::fadds_ppc(centerZ,(f32)output->z);
                quad->vertices[0].x=x0;quad->vertices[0].y=y0;quad->vertices[0].z=z0;
                offset=gabi::fmuls_ppc(width,gabi::fmuls_ppc(widthScale,lf(particle+24)));
                skewY=lf(packet+0x4244);skew=lf(packet+0x4240);
                input->x=skewY<zero?gabi::fnmsubs(offset,skew,width):gabi::fmadds(offset,skew,width);
                input->y=height;input->z=zero;
                gabi::call(0x028E8F64,view.get(),input.get(),output.get());
                quad->vertices[1].x=gabi::fadds_ppc(centerX,(f32)output->x);
                quad->vertices[1].y=gabi::fadds_ppc(centerY,(f32)output->y);
                quad->vertices[1].z=gabi::fadds_ppc(centerZ,(f32)output->z);
                input->x=width;input->y=zero;input->z=zero;
                gabi::call(0x028E8F64,view.get(),input.get(),output.get());
                quad->vertices[2].x=gabi::fadds_ppc(centerX,(f32)output->x);
                quad->vertices[2].y=gabi::fadds_ppc(centerY,(f32)output->y);
                quad->vertices[2].z=gabi::fadds_ppc(centerZ,(f32)output->z);
                input->x=-width;input->y=zero;input->z=zero;
                gabi::call(0x028E8F64,view.get(),input.get(),output.get());
                quad->vertices[3].x=gabi::fadds_ppc(centerX,(f32)output->x);
                quad->vertices[3].y=gabi::fadds_ppc(centerY,(f32)output->y);
                quad->vertices[3].z=gabi::fadds_ppc(centerZ,(f32)output->z);
                if(!ld(0x1047758C)) {
                    f32 values[12]={zero,zero,zero,uvScale,zero,uvWide,scalarOne,zero,uvTwo,uvTwo,zero,uvScale};
                    for(u32 k=0;k<12;k++)sf(0x10477480+k*4,values[k]);
                    gabi::store<u32>(0x1047758C,1);
                }
                u32 geometry=packet+0x424C+(u32)i*0x4C0;
                u32 selected=ld(geometry+0x4A8),buffer=ld(geometry+selected*596),end=buffer+64;
                if(buffer<end)for(u32 cursor=buffer;cursor<end;cursor+=32)for(u32 k=0;k<32;k+=4)gabi::store<u32>((cursor&~31u)+k,0);
                selected=ld(geometry+0x4A8);buffer=ld(geometry+selected*596);
                f32 texture[4][2]={{zero,zero},{one,zero},{one,one},{zero,one}};
                for(u32 corner=0;corner<4;corner++) {
                    for(u32 axis=0;axis<3;axis++)sf(buffer+corner*20+axis*4,gabi::fadds_ppc(lf(gabi::ea(quad.get())+corner*12+axis*4),lf(0x10477480+axis*4)));
                    sf(buffer+corner*20+12,texture[corner][0]);sf(buffer+corner*20+16,texture[corner][1]);
                }
                selected=ld(geometry+0x4A8);u32 slot=geometry+selected*596;
                gabi::call(0x027B5E94,slot+4,0,ld(slot+0x150));
                gabi::store<u32>(geometry+0x4A8,ld(geometry+0x4A8)==0?1:0);
            }
        }
        i++;
    }
    gabi::call(0x02582650,packet,draw);
}
VERIFY(0x02574D38,drawWave);
}
