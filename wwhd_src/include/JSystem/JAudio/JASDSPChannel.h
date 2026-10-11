/* DSP channel scheduling, WWHD layout.  HD: the GameCube 0x14-byte channel becomes 0x1C bytes:
 * word-sized status and flags, signed 16-bit priority, 32-bit age, and separate
 * callback data. The callback takes a reason, DSP buffer, and data instead of a
 * channel and reason. */
#pragma once
#include "bindings.h"
namespace JASystem {
namespace DSPInterface {
class DSPBuffer;
}
class TDSPChannel {
public:
  enum Status_e { STATUS_ACTIVE_e, STATUS_FREE_e, STATUS_STOPPING_e };
  enum Flag_e { FLAG_PLAY_e = 1, FLAG_STOP_e = 2 };
  enum Callback_e {
    CALLBACK_UPDATE_e,
    CALLBACK_PLAY_e,
    CALLBACK_FINISH_e,
    CALLBACK_FORCE_STOP_e
  };
  enum { CHANNEL_COUNT = 64, UNSTEALABLE_PRIORITY = 127, FREE_PRIORITY = -1 };
  void forceStop();
  void detachCallback();
  void setPriority(u32 priority);
  void play();
  void markWaveReady();
  void update();
  /* 0x00 */ be<u32> mStatus;
  /* 0x04 */ be<s16> mPriority;
  /* 0x06 */ u8 mPadding[2];
  /* 0x08 */ be<u32> mFlags;
  /* 0x0C */ be<u32> mPriorityTime;
  /* 0x10 */ be<u32> mCallback;
  /* 0x14 */ be<u32> mCallbackArg;
  /* 0x18 */ gptr<DSPInterface::DSPBuffer> mpDSPBuffer;
};
static_assert(sizeof(TDSPChannel) == 0x1C);
} // namespace JASystem

struct GlobalRegistrationHD {
  u8 bytes[12];
};

using ChannelConstructorHD =
    JASystem::TDSPChannel *(*)(JASystem::TDSPChannel *);
