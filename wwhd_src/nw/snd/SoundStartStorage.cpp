/* NW-SND#6: per-TU constructors of the archive player's small records (stream track summaries,
 * resource choices, priority buckets). Written from WWHD's instructions; verified by tools/verify. */
#include "nw/snd/SoundStartStorage.h"
using namespace nw::snd;

/* 02895ED4 */
ArchiveStreamTrackSummary* constructStreamTrack(ArchiveStreamTrackSummary* track) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (track == NULL) track = (ArchiveStreamTrackSummary*)utAllocate(sizeof(ArchiveStreamTrackSummary));
    if (track != NULL) {
        track->filterValue = 0;
        track->routing[0] = 127;
        track->channelCount = 0;
        track->pan = 0;
        track->surroundPan = 0;
        track->volume = 0;
        track->priority = 64;
        track->filter = 0;
        fillBytes(track->channels, -1, 2);
        fillBytes(&track->routing[1], 0, 3);
    }
    return track;
}

/* 02895F68 */
SoundResourceChoice* constructResourceChoice(SoundResourceChoice* choice) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (choice == NULL) choice = (SoundResourceChoice*)utAllocate(sizeof(SoundResourceChoice));
    if (choice != NULL) {
        choice->type = -1;
        choice->value = 0;
    }
    return choice;
}

/* The three priority-bucket constructors are separate copies of one inline constructor. */
/* 02895FAC */
PlaybackPriorityBucket* constructSequencePoolBucket(PlaybackPriorityBucket* bucket) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (bucket == NULL) bucket = (PlaybackPriorityBucket*)utAllocate(sizeof(PlaybackPriorityBucket));
    if (bucket != NULL) {
        bucket->count = 0;
        bucket->sentinel.previous = &bucket->sentinel;
        bucket->sentinel.next = &bucket->sentinel;
    }
    return bucket;
}

/* 02895FF4 */
PlaybackPriorityBucket* constructWavePoolBucket(PlaybackPriorityBucket* bucket) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (bucket == NULL) bucket = (PlaybackPriorityBucket*)utAllocate(sizeof(PlaybackPriorityBucket));
    if (bucket != NULL) {
        bucket->count = 0;
        bucket->sentinel.previous = &bucket->sentinel;
        bucket->sentinel.next = &bucket->sentinel;
    }
    return bucket;
}

/* 0289603C */
PlaybackPriorityBucket* constructStreamPoolBucket(PlaybackPriorityBucket* bucket) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (bucket == NULL) bucket = (PlaybackPriorityBucket*)utAllocate(sizeof(PlaybackPriorityBucket));
    if (bucket != NULL) {
        bucket->count = 0;
        bucket->sentinel.previous = &bucket->sentinel;
        bucket->sentinel.next = &bucket->sentinel;
    }
    return bucket;
}

/* WWHD glue
 ext C000A858 void fillBytes(void*,s32,u32)
*/
