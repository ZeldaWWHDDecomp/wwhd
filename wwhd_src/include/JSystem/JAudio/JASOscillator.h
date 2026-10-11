#ifndef WWHD_JASOSCILLATOR_H
#define WWHD_JASOSCILLATOR_H
#include "wwhd.h"
using namespace gabi;
namespace JASystem {
// HD: rate/phase fields precede the index and state; the configuration keeps the GC layout.
class TOscillator {
  public:
    struct Osc_ {
        /* 0x00 */ be<u8> field_0x0;
        u8 padding[3];
        /* 0x04 */ be<f32> field_0x4;
        /* 0x08 */ gptr<be<s16>> table;
        /* 0x0C */ gptr<be<s16>> rel_table;
        /* 0x10 */ be<f32> field_0x10;
        /* 0x14 */ be<f32> field_0x14;
    };
    // HD: initialized state is zero; attack through direct release are GC states minus one.
    enum State { STOP, ATTACK, HOLD, RELEASE, FORCE_STOP, DIRECT_RELEASE };
    enum EnvelopeCommand { JUMP = 13, SUSTAIN = 14, END = 15 };
    void init();
    f32 calc(be<s16>* i_table);
    void getOffset();                // HD: updates only; the value is read separately.
    void initStart(const Osc_* osc); // HD: configuration is supplied here.
    f32 getValue();
    u32 release(); // HD: keep the full integer result used by callers.
    /* 0x00 */ gptr<const Osc_> mOsc;
    /* 0x04 */ be<f32> mReleaseRate;
    /* 0x08 */ be<f32> mPhase;
    /* 0x0C */ be<f32> mTargetPhase;
    /* 0x10 */ be<f32> mPhaseChangeRate;
    /* 0x14 */ be<f32> mInitialReleaseRate;
    /* 0x18 */ be<u16> field_0x6;
    /* 0x1A */ be<u16> mDirectRelease;
    /* 0x1C */ be<u8> mState;
    /* 0x1D */ be<u8> field_0x5;
};
} // namespace JASystem
WWHD_OFFSET(JASystem::TOscillator::Osc_, field_0x4, 0x04);
WWHD_OFFSET(JASystem::TOscillator::Osc_, table, 0x08);
WWHD_OFFSET(JASystem::TOscillator::Osc_, rel_table, 0x0C);
WWHD_OFFSET(JASystem::TOscillator::Osc_, field_0x10, 0x10);
WWHD_OFFSET(JASystem::TOscillator::Osc_, field_0x14, 0x14);
WWHD_SIZE(JASystem::TOscillator::Osc_, 0x18);
WWHD_OFFSET(JASystem::TOscillator, mOsc, 0x00);
WWHD_OFFSET(JASystem::TOscillator, mReleaseRate, 0x04);
WWHD_OFFSET(JASystem::TOscillator, mPhase, 0x08);
WWHD_OFFSET(JASystem::TOscillator, mTargetPhase, 0x0C);
WWHD_OFFSET(JASystem::TOscillator, mPhaseChangeRate, 0x10);
WWHD_OFFSET(JASystem::TOscillator, mInitialReleaseRate, 0x14);
WWHD_OFFSET(JASystem::TOscillator, field_0x6, 0x18);
WWHD_OFFSET(JASystem::TOscillator, mDirectRelease, 0x1A);
WWHD_OFFSET(JASystem::TOscillator, mState, 0x1C);
WWHD_OFFSET(JASystem::TOscillator, field_0x5, 0x1D);
#endif
