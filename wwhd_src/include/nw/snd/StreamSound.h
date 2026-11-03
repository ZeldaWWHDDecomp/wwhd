// Game-thread stream sound extension, reconstructed from original WWHD accesses.
#pragma once
#include "nw/snd/BasicSound.h"
#include "nw/snd/ArchivePlayback.h"
#include "nw/snd/StreamPlayback.h"
namespace nw { namespace snd {
struct StreamSound;
struct StreamSoundHandle { gptr<StreamSound> sound; void detach(); };
struct StreamTrackFade {
    be<f32> origin,target;
    be<s32> frames,counter;
    static StreamTrackFade* construct(StreamTrackFade*);
    f32 value() const {
        s32 elapsed=counter, length=frames;
        f32 goal=target;
        if(elapsed>=length) return goal;
        f32 start=origin;
        f32 distance=gabi::fsubs_ppc(goal,start);
        f32 step=gabi::fmuls_ppc(distance,static_cast<f32>(elapsed));
        return gabi::fadds_ppc(start,gabi::fdivs_ppc(step,static_cast<f32>(length)));
    }
};
struct StreamSetupBlock { be<u32> words[35]; };
struct StreamMetadataBlock { be<u32> words[171]; };
struct StreamSound {
    BasicSound basic;
    gptr<StreamSoundHandle> handle;
    gptr<PlaybackPool> pool;
    StreamTrackFade fades[8];
    u8 metadataStorage[0x200];
    be<u16> trackMask;
    be<u8> active;
    u8 reserved523;
    be<u32> firstSource,secondSource;
    StreamPlayback player;
    static StreamSound* construct(StreamSound*,PlaybackPool*);
    void sendSetup(const StreamSetupBlock*);
    void sendMetadata(const StreamMetadataBlock*);
    void initialize();
    void finalize();
    u32 isPrepared() const;
    u32 hasHandle() const;
    void detachHandle();
    void updateFades();
    void updatePriority();
    void sendTrackVolumes();
    StreamPlayback* driverPlayer();
    void destroy(u32 flags);
};
WWHD_OFFSET(StreamSound,player,0x52C); WWHD_SIZE(StreamSound,0x7670);
}}
