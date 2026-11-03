/* Channel mixing state, assembled from WWHD constructors and their array strides. */
#pragma once
#include "wwhd.h"
namespace nw { namespace snd {
struct ChannelMixState {
    be<f32> gain, pitch, pan, surroundPan, mainSend, auxiliarySend;
    static ChannelMixState* construct(ChannelMixState* state);
    static ChannelMixState* constructForWave(ChannelMixState* state);
};
WWHD_OFFSET(ChannelMixState, auxiliarySend, 0x14);
WWHD_SIZE(ChannelMixState, 0x18);
struct ChannelMixGroup {
    be<f32> gain;
    be<u32> mode;
    ChannelMixState channels[2];
    be<f32> pan;
    be<f32> surroundPan;
    be<f32> mainSend;
    be<f32> auxiliarySend[3];
    static ChannelMixGroup* construct(ChannelMixGroup* group);
    static ChannelMixGroup* constructForWave(ChannelMixGroup* group);
};
WWHD_OFFSET(ChannelMixGroup, channels, 8);
WWHD_OFFSET(ChannelMixGroup, pan, 0x38);
WWHD_OFFSET(ChannelMixGroup, mainSend, 0x40);
WWHD_SIZE(ChannelMixGroup, 0x50);
struct ChannelOutputGain { be<f32> gain, pan, surroundPan; };
WWHD_SIZE(ChannelOutputGain, 12);
struct ChannelMixDefaults {
    be<f32> gain, pitch, pan, surroundPan;
    be<u8> panMode, filterKind, filterEnabled;
    u8 reserved;
    be<u32> filterState;
    be<u32> flags;
    be<u32> outputMode;
    u8 padding[4];
    ChannelMixGroup groups[3];
    ChannelOutputGain outputs[4];
    void initialize();
};
WWHD_OFFSET(ChannelMixDefaults, panMode, 0x10);
WWHD_OFFSET(ChannelMixDefaults, groups, 0x24);
WWHD_OFFSET(ChannelMixDefaults, outputs, 0x114);
WWHD_SIZE(ChannelMixDefaults, 0x144);
struct ChannelParameterTable { be<u32> words[2]; };
struct ChannelParameters {
    u8 synchronization[0x24];
    be<u8> flags[5];
    u8 padding[3];
    ChannelMixDefaults mix;
    be<u32> channelMask;
    gptr<ChannelParameterTable> table;
    static ChannelParameters* construct(ChannelParameters* parameters);
    void reset();
    void finish();
    void setSurroundPan(u32 mode, f64 value);
};
WWHD_OFFSET(ChannelParameters, flags, 0x24);
WWHD_OFFSET(ChannelParameters, mix, 0x2C);
WWHD_OFFSET(ChannelParameters, channelMask, 0x170);
WWHD_OFFSET(ChannelParameters, table, 0x174);
WWHD_SIZE(ChannelParameters, 0x178);
} }
