/* Channel parameter construction and defaults. From WWHD's instructions only. */
#include "nw/snd/ChannelParameters.h"
using namespace nw::snd;

void* allocateChannelParameters(u32 size);
void initializeChannelSynchronization(void* storage, u32 first, u32 second);
void finishChannelSynchronization(void* storage);
void constructChannelArray(void* storage, u32 count, u32 stride, u32 callback);
static ChannelParameterTable sChannelParameterTable;

/* 028A3534 */
void ChannelMixDefaults::initialize() {
    flags = 0;
    pan = 0.0f;
    groups[0].mainSend = 0.0f;
    groups[0].surroundPan = 0.0f;
    filterState = 0;
    surroundPan = 0.0f;
    gain = 1.0f;
    outputMode = 1;
    groups[0].pan = 0.0f;
    filterKind = 0;
    groups[0].gain = 1.0f;
    pitch = 1.0f;
    groups[0].mode = 0;
    filterEnabled = 0;
    panMode = 0xFF;
    for (u32 i = 0; i < 3; ++i) groups[0].auxiliarySend[i] = 0.0f;
    for (u32 group = 1; group < 3; ++group) {
        groups[group].gain = 1.0f;
        groups[group].mainSend = 0.0f;
        groups[group].mode = 0;
        groups[group].pan = 0.0f;
        groups[group].surroundPan = 0.0f;
        for (u32 i = 0; i < 3; ++i) groups[group].auxiliarySend[i] = 0.0f;
    }
    for (u32 i = 0; i < 4; ++i) {
        outputs[i].pan = 0.0f;
        outputs[i].surroundPan = 0.0f;
        outputs[i].gain = 1.0f;
    }
}

/* 028A3610 */
ChannelParameters* ChannelParameters::construct(ChannelParameters* parameters) {
    gabi::AutoFrame frame{0x028A3610};
    if (!parameters) parameters = (ChannelParameters*)allocateChannelParameters(sizeof(ChannelParameters));
    if (parameters) {
        parameters->table = &sChannelParameterTable;
        initializeChannelSynchronization(parameters, 1, 1);
        parameters->flags[3] = 0;
        parameters->flags[4] = 0;
        parameters->flags[1] = 0;
        parameters->flags[0] = 0;
        parameters->flags[2] = 0;
        constructChannelArray(parameters->mix.groups[0].channels, 2, sizeof(ChannelMixState),
            gabi::guest_fn((const void*)&ChannelMixState::construct));
        constructChannelArray(&parameters->mix.groups[1], 2, sizeof(ChannelMixGroup),
            gabi::guest_fn((const void*)&ChannelMixGroup::construct));
        parameters->mix.initialize();
    }
    return parameters;
}

/* 028A36C8 */
void ChannelParameters::reset() {
    gabi::AutoFrame frame{0x028A36C8};
    flags[1] = 0;
    flags[4] = 0;
    flags[0] = 0;
    flags[2] = 0;
    flags[3] = 0;
    mix.initialize();
    channelMask = 0;
}

/* 028A370C */
void ChannelParameters::finish() {
    gabi::AutoFrame frame{0x028A370C};
    finishChannelSynchronization(this);
}

/* 028A3710 */
void ChannelParameters::setSurroundPan(u32 mode, f64 value) {
    mix.panMode = mode;
    mix.surroundPan = gabi::f32_from_bits(ppc_stfs_bits(value));
}

/* 028A371C */
ChannelMixState* ChannelMixState::construct(ChannelMixState* state) {
    gabi::AutoFrame frame{0x028A371C};
    if (!state) state = (ChannelMixState*)allocateChannelParameters(sizeof(ChannelMixState));
    if (state) {
        state->pitch = 1.0f;
        state->auxiliarySend = 0.0f;
        state->pan = 0.0f;
        state->mainSend = 0.0f;
        state->gain = 1.0f;
        state->surroundPan = 0.0f;
    }
    return state;
}

/* 028A3778 */
ChannelMixGroup* ChannelMixGroup::construct(ChannelMixGroup* group) {
    gabi::AutoFrame frame{0x028A3778};
    if (!group) group = (ChannelMixGroup*)allocateChannelParameters(sizeof(ChannelMixGroup));
    if (group) constructChannelArray(group->channels, 2, sizeof(ChannelMixState),
        gabi::guest_fn((const void*)&ChannelMixState::construct));
    return group;
}

/* WWHD glue
 data sChannelParameterTable 101857C8
 ext 0273AD10 void* allocateChannelParameters(u32)
 ext 028F69A8 void initializeChannelSynchronization(void*,u32,u32)
 ext 028F6A24 void finishChannelSynchronization(void*)
 ext 028EFFD0 void constructChannelArray(void*,u32,u32,u32)
*/
