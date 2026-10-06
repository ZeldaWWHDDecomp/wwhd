/* Native HD frame controller;16-byte layout without a vptr. */
#include "wwhd.h"
enum { EMode_NONE, EMode_RESET, EMode_LOOP, EMode_REVERSE, EMode_LOOP_REVERSE };
void frame_init(void* self,s16 end) {
 WWHD_FUNC(0x027F2BC0,void,self,end);
 u32 p=gabi::ea(self);
 gabi::store<s16>(p+10,end);
 gabi::store<u8>(p+15,0);gabi::store<u8>(p+14,2);
 f32 initialFrame=gabi::load<f32>(0x1016E48C);
 gabi::store<s16>(p+8,0);gabi::store<f32>(p+4,initialFrame);
 f32 initialRate=gabi::load<f32>(0x1016E488);
 gabi::store<s16>(p+12,0);gabi::store<f32>(p,initialRate);
}
VERIFY(0x027F2BC0,frame_init);
s32 frame_checkPass(void* self,f32 pass_frame) {
 WWHD_FUNC(0x027F2BF8,s32,self,pass_frame);
 u32 p=gabi::ea(self);
 f32 mFrame=gabi::load<f32>(p+4),mRate=gabi::load<f32>(p);
 s16 mStart=gabi::load<s16>(p+8),mEnd=gabi::load<s16>(p+10),mLoop=gabi::load<s16>(p+12);
 u8 mAttribute=gabi::load<u8>(p+14);

    f32 cur_frame = mFrame;
    f32 next_frame = cur_frame + mRate;

    switch (mAttribute) {
    case EMode_NONE:
    case EMode_RESET:
        if (next_frame < mStart) {
            next_frame = mStart;
        }

        if (next_frame >= mEnd) {
            next_frame = mEnd - gabi::load<f32>(0x1016E490);
        }

        if (cur_frame <= next_frame) {
            if (cur_frame <= pass_frame && pass_frame < next_frame) {
                return true;
            } else {
                return false;
            }
        }

        if (next_frame <= pass_frame && pass_frame < cur_frame) {
            return true;
        }
        return false;
    case EMode_LOOP:
        if (cur_frame < mStart) {
            while (next_frame < mStart) {
                if (mLoop - mStart <= 0.0f) {
                    break;
                }
                next_frame += mLoop - mStart;
            }

            if (next_frame <= pass_frame && pass_frame < mLoop) {
                return true;
            } else {
                return false;
            }
        } else if (mEnd <= cur_frame) {
            while (next_frame >= mEnd) {
                if (mEnd - mLoop <= 0.0f) {
                    break;
                }
                next_frame -= mEnd - mLoop;
            }

            if (mLoop <= pass_frame && pass_frame < next_frame) {
                return true;
            } else {
                return false;
            }
        } else if (next_frame < mStart) {
            while (next_frame < mStart) {
                if (mLoop - mStart <= 0.0f) {
                    break;
                }
                next_frame += mLoop - mStart;
            }

            if ((mStart <= pass_frame && pass_frame < cur_frame) || (next_frame <= pass_frame && pass_frame < mLoop)) {
                return true;
            } else {
                return false;
            }
        } else if (mEnd <= next_frame) {
            while (next_frame >= mEnd) {
                if (mEnd - mLoop <= 0.0f) {
                    break;
                }

                next_frame -= mEnd - mLoop;
            }

            if ((cur_frame <= pass_frame && pass_frame < mEnd) || (mLoop <= pass_frame && pass_frame < next_frame)) {
                return true;
            } else {
                return false;
            }
        } else if (cur_frame <= next_frame) {
            if (cur_frame <= pass_frame && pass_frame < next_frame) {
                return true;
            } else {
                return false;
            }
        } else if (next_frame <= pass_frame && pass_frame < cur_frame) {
            return true;
        }
        return false;
    case EMode_REVERSE:
    case EMode_LOOP_REVERSE:
        if (next_frame >= mEnd) {
            next_frame = mEnd - gabi::load<f32>(0x1016E490);
        }

        if (next_frame < mStart) {
            next_frame = mStart;
        }

        if (cur_frame <= next_frame) {
            if (cur_frame <= pass_frame && pass_frame < next_frame) {
                return true;
            } else {
                return false;
            }
        }

        if (next_frame <= pass_frame && pass_frame < cur_frame) {
            return true;
        }
        return false;
    default:
        return false;
    }

}
VERIFY(0x027F2BF8,frame_checkPass);

void frame_update(void* self) {
 WWHD_FUNC(0x027F2FC4,void,self);
 u32 p=gabi::ea(self);
 f32 mFrame=gabi::load<f32>(p+4),mRate=gabi::load<f32>(p);
 s16 mStart=gabi::load<s16>(p+8),mEnd=gabi::load<s16>(p+10),mLoop=gabi::load<s16>(p+12);
 u8 mAttribute=gabi::load<u8>(p+14),mState=0;

    { mState = 0; gabi::store<u8>(p+15,mState); }
    { mFrame += mRate; gabi::store<f32>(p+4,mFrame); }
    switch (mAttribute) {
    case EMode_NONE:
        if (mFrame < mStart) {
            { mFrame = mStart; gabi::store<f32>(p+4,mFrame); }
            { mRate = 0.0f; gabi::store<f32>(p+0,mRate); }
            { mState |= 1; gabi::store<u8>(p+15,mState); }
        }
        if (mFrame >= mEnd) {
            { mFrame = mEnd - gabi::load<f32>(0x1016E490); gabi::store<f32>(p+4,mFrame); }
            { mRate = 0.0f; gabi::store<f32>(p+0,mRate); }
            { mState |= 1; gabi::store<u8>(p+15,mState); }
        }
        break;
    case EMode_RESET:
        if (mFrame < mStart) {
            { mFrame = mStart; gabi::store<f32>(p+4,mFrame); }
            { mRate = 0.0f; gabi::store<f32>(p+0,mRate); }
            { mState |= 1; gabi::store<u8>(p+15,mState); }
        }
        if (mFrame >= mEnd) {
            { mFrame = mStart; gabi::store<f32>(p+4,mFrame); }
            { mRate = 0.0f; gabi::store<f32>(p+0,mRate); }
            { mState |= 1; gabi::store<u8>(p+15,mState); }
        }
        break;
    case EMode_LOOP: {
        // HD uses one remainder operation per crossed boundary and leaves state clear.
        if (mFrame < mStart) {
            f32 span=(f32)(mLoop-mStart);
            if (span > 0.0f) {
                f64 rem=gabi::call<f64>(0x028F4034,(f64)(f32)(mFrame-mStart),(f64)span);
                mFrame=(f32)(rem+(f64)span);
                gabi::store<f32>(p+4,mFrame);
            }
        }
        mEnd=gabi::load<s16>(p+10);
        if (!(mFrame < mEnd)) {
            mLoop=gabi::load<s16>(p+12);
            f32 span=(f32)(mEnd-mLoop);
            if (span > 0.0f) {
                f64 rem=gabi::call<f64>(0x028F4034,(f64)(f32)(mFrame-mEnd),(f64)span);
                gabi::store<f32>(p+4,(f32)rem);
            }
        }
        break;
    }
    case EMode_REVERSE:
        if (mFrame >= mEnd) {
            { mFrame = mEnd - gabi::load<f32>(0x1016E490); gabi::store<f32>(p+4,mFrame); }
            { mRate = -mRate; gabi::store<f32>(p+0,mRate); }
        }
        if (mFrame < mStart) {
            { mFrame = mStart; gabi::store<f32>(p+4,mFrame); }
            { mRate = 0.0f; gabi::store<f32>(p+0,mRate); }
            { mState |= 1; gabi::store<u8>(p+15,mState); }
        }
        break;
    case EMode_LOOP_REVERSE:
        if (mFrame >= mEnd) {
            { mFrame = mEnd - gabi::load<f32>(0x1016E490); gabi::store<f32>(p+4,mFrame); }
            { mRate = -mRate; gabi::store<f32>(p+0,mRate); }
        }
        if (mFrame < mStart) {
            { mFrame = mStart; gabi::store<f32>(p+4,mFrame); }
            { mRate = -mRate; gabi::store<f32>(p+0,mRate); }
            { mState |= 2; gabi::store<u8>(p+15,mState); }
        }
        break;
    }

}
VERIFY(0x027F2FC4,frame_update);
