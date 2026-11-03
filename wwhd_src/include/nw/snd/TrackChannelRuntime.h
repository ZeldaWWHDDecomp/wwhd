#pragma once
#include "nw/snd/SequenceRuntimeSupport.h"
namespace nw { namespace snd {
struct ChannelModulation_l { be<u32> state[4]; u8 reserved[16]; };
struct ChannelRuntime_l {
    u8 channelState[0x2C];
    ChannelModulation_l modulation[4];
    be<u8> modulationType[4];
    u8 flags[6];
    be<s8> biquadType;
    u8 biquadPadding;
    be<f32> volume,pitch,lpfOffset,biquadValue;
    be<u32> outputLine;
    TrackOutputControl tvOutput,output[2];
    be<u32> remoteOutput[4][3];
    be<f32> pitchBend;
    u8 voiceState[0x48];
    gptr<ChannelRuntime_l> next;
    void detach(u32,TrackRuntimeControl*);
};
WWHD_OFFSET(ChannelRuntime_l,modulation,0x2C);
WWHD_OFFSET(ChannelRuntime_l,modulationType,0xAC);
WWHD_OFFSET(ChannelRuntime_l,volume,0xB8);
WWHD_OFFSET(ChannelRuntime_l,tvOutput,0xCC);
WWHD_OFFSET(ChannelRuntime_l,remoteOutput,0x1BC);
WWHD_OFFSET(ChannelRuntime_l,pitchBend,0x1EC);
WWHD_OFFSET(ChannelRuntime_l,next,0x238);
WWHD_SIZE(ChannelRuntime_l,0x23C);
} }
void runtimeChannelBiquad(nw::snd::ChannelRuntime_l*,s32,f32);
