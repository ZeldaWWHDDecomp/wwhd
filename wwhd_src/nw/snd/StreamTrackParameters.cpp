// Stream track parameter propagation, reconstructed solely from WWHD instructions.
#include "nw/snd/StreamPlayback.h"
using namespace nw::snd;

/* 0289B0F8 */
void StreamPlayback::setTrackVolume(u32 mask,f64 value) {
    u32 count=trackCount;
    for(u32 index=0;index<count && mask;++index,mask>>=1) {
        if(mask&1) { tracks[index].volume=gabi::f32_from_bits(ppc_stfs_bits(value)); count=trackCount; }
    }
}

/* 0289B13C */
void StreamPlayback::setTrackVolumeByte(u32 mask,u8 value) {
    u32 count=trackCount;
    for(u32 index=0;index<count && mask;++index,mask>>=1) {
        if(mask&1) { tracks[index].volumeByte=value; count=trackCount; }
    }
}

/* 0289B180 */
void StreamPlayback::setTrackOutputLine(u32 mask,u32 value) {
    u32 count=trackCount;
    for(u32 index=0;index<count && mask;++index,mask>>=1) {
        if(mask&1) { tracks[index].outputLine=value; count=trackCount; }
    }
}

/* 0289B20C */
void StreamPlayback::setTrackTvVolume(u32 mask,f64 value) {
    u32 count=trackCount;
    for(u32 index=0;index<count && mask;++index,mask>>=1) {
        if(mask&1) { tracks[index].tvVolume=gabi::f32_from_bits(ppc_stfs_bits(value)); count=trackCount; }
    }
}

/* 0289B2BC */
void StreamPlayback::setTrackPan(u32 mask,f64 value) {
    u32 count=trackCount;
    for(u32 index=0;index<count && mask;++index,mask>>=1) {
        if(mask&1) { tracks[index].pan=gabi::f32_from_bits(ppc_stfs_bits(value)); count=trackCount; }
    }
}

/* 0289B300 */
void StreamPlayback::setTrackSurroundPan(u32 mask,f64 value) {
    u32 count=trackCount;
    for(u32 index=0;index<count && mask;++index,mask>>=1) {
        if(mask&1) { tracks[index].surroundPan=gabi::f32_from_bits(ppc_stfs_bits(value)); count=trackCount; }
    }
}

/* 0289B344 */
void StreamPlayback::setTrackMainSend(u32 mask,f64 value) {
    u32 count=trackCount;
    for(u32 index=0;index<count && mask;++index,mask>>=1) {
        if(mask&1) { tracks[index].mainSend=gabi::f32_from_bits(ppc_stfs_bits(value)); count=trackCount; }
    }
}

/* 0289B1C4 */
void StreamPlayback::resetTrackOutputLine(u32 mask) {
    u32 count=trackCount;
    for(u32 index=0;index<count && mask;++index,mask>>=1) {
        if(mask&1) { tracks[index].outputLine=0xFFFFFFFF; count=trackCount; }
    }
}
/* 0289B250 */
void StreamPlayback::setTrackMix(u32 mask,u32 set,const ChannelMixState* value) {
    u32 count=trackCount;
    for(u32 index=0;index<count && mask;++index,mask>>=1) {
        if(mask&1) {
            be<f32>* out=reinterpret_cast<be<f32>*>(&tracks[index].mix[set]);
            const be<f32>* input=reinterpret_cast<const be<f32>*>(value);
            for(u32 component=0;component<6;++component) out[component]=static_cast<f32>(input[component]);
            count=trackCount;
        }
    }
}
/* 0289B388 */
void StreamPlayback::setTrackFxSend(u32 mask,u32 bus,f64 value) {
    u32 count=trackCount;
    for(u32 index=0;index<count && mask;++index,mask>>=1) {
        if(mask&1) { tracks[index].fxSend[bus]=gabi::f32_from_bits(ppc_stfs_bits(value)); count=trackCount; }
    }
}

/* 0289B3D8 */
void StreamPlayback::setTrackOutputVolume(u32 output,u32 mask,f64 value) {
    u32 count=trackCount;
    for(u32 index=0;index<count && mask;++index,mask>>=1) {
        if(mask&1) { tracks[index].outputs[output].gain=gabi::f32_from_bits(ppc_stfs_bits(value)); count=trackCount; }
    }
}

/* 0289B4B0 */
void StreamPlayback::setTrackOutputPan(u32 output,u32 mask,f64 value) {
    u32 count=trackCount;
    for(u32 index=0;index<count && mask;++index,mask>>=1) {
        if(mask&1) { tracks[index].outputs[output].pan=gabi::f32_from_bits(ppc_stfs_bits(value)); count=trackCount; }
    }
}

/* 0289B500 */
void StreamPlayback::setTrackOutputSurroundPan(u32 output,u32 mask,f64 value) {
    u32 count=trackCount;
    for(u32 index=0;index<count && mask;++index,mask>>=1) {
        if(mask&1) { tracks[index].outputs[output].surroundPan=gabi::f32_from_bits(ppc_stfs_bits(value)); count=trackCount; }
    }
}

/* 0289B550 */
void StreamPlayback::setTrackOutputMainSend(u32 output,u32 mask,f64 value) {
    u32 count=trackCount;
    for(u32 index=0;index<count && mask;++index,mask>>=1) {
        if(mask&1) { tracks[index].outputs[output].mainSend=gabi::f32_from_bits(ppc_stfs_bits(value)); count=trackCount; }
    }
}

/* 0289B428 */
void StreamPlayback::setTrackOutputMix(u32 output,u32 mask,u32 set,const ChannelMixState* value) {
    u32 count=trackCount;
    for(u32 index=0;index<count && mask;++index,mask>>=1) {
        if(mask&1) {
            be<f32>* out=reinterpret_cast<be<f32>*>(&tracks[index].outputs[output].channels[set]);
            const be<f32>* input=reinterpret_cast<const be<f32>*>(value);
            for(u32 component=0;component<6;++component) out[component]=static_cast<f32>(input[component]);
            count=trackCount;
        }
    }
}
/* 0289B5A0 */
void StreamPlayback::setTrackOutputFxSend(u32 output,u32 mask,u32 bus,f64 value) {
    u32 count=trackCount;
    for(u32 index=0;index<count && mask;++index,mask>>=1) {
        if(mask&1) { tracks[index].outputs[output].auxiliarySend[bus]=gabi::f32_from_bits(ppc_stfs_bits(value)); count=trackCount; }
    }
}
/* 0289BF04 */
void StreamPlayback::forwardHook() { gabi::call_ptr<void>(vtable->hook,this); }
/* WWHD glue
*/
