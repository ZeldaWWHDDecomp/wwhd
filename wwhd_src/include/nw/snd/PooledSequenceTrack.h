/* Pooled track lifecycle wrappers, using the canonical NW-SND#12 track layout. */
#pragma once
#include "nw/snd/SequenceTrackPool.h"
nw::snd::SequenceTrack_l* constructSequenceTrack(void* memory);
void releasePooledSequenceTrack(nw::snd::SequenceTrack_l* track,u32 argument);
void destroyPooledSequenceTrack(nw::snd::SequenceTrack_l* track,u32 flags);
