/* HD message states. */
#include "d/hd_msg_window_base.h"
using namespace gabi;
static f32 l_windowPartOne;
static f32 l_windowPartZero;
extern void windowUpdateItem(WindowPart *, u32);
extern f32 windowRandomDelay(f32);
extern void windowFinishPart(WindowPart *, WindowPartConfig *);

static s32 divided(s32 a, s32 b) {
    if (!b || (a == s32(0x80000000u) && b == -1))
        return a < 0 ? -1 : 0;
    return a / b;
}
static s32 sub(s32 a, s32 b) { return s32(u32(a) - u32(b)); }
static void item(WindowPart *self, s32 index) { windowUpdateItem(self, u32(index)); }
/* 026F48A4 */
u32 hdMsgPart_updateItems(WindowPart *self, f32 before, f32 now) {
    s32 count = self->mItemCount;
    if (count == 0)
        return 1;
    f32 one = l_windowPartOne;
    f32 zero = l_windowPartZero;
    f32 progress = zero;
    WindowPartConfig *config = self->mpConfig.get();
    if (count == 1) {
        WindowAnimator *animator = nullptr;
        if (config->mAnimatorCount)
            animator = config->mpAnimators.get()->get();
        if (animator->mProgress == zero)
            call_ptr<void>(animator->mpVTable[43].get(), animator, one);
        return 1;
    }
    s32 first = 0;
    s32 last = 0;
    if (config->mDuration <= 0) {
        progress = one;
        first = 0;
        last = sub(count, 1);
    } else {
        u32 mode = config->mMode;
        if (mode == WINDOW_SCHEDULE_TIMELINE || mode == WINDOW_SCHEDULE_EXTENDED_TIMELINE) {
            s32 duration = config->mDuration;
            s32 end = sub(count, 1);
            if (mode == WINDOW_SCHEDULE_EXTENDED_TIMELINE)
                end = s32(u32(end) + config->mExtraItems);
            // HD: GHS first divides time by duration, then by 1/(count-1 or extended end).
            f32 divisor = one / f32(end);
            progress = before / f32(duration);
            first = ftoi(progress / divisor);
            last = ftoi((now / f32(duration)) / divisor);
            if (mode == WINDOW_SCHEDULE_EXTENDED_TIMELINE) {
                s32 edge = sub(count, 1);
                if (first >= end) {
                    first = edge;
                    if (last >= end)
                        last = edge;
                    else if (last >= edge)
                        last = sub(edge, 1);
                } else {
                    if (first >= edge)
                        first = sub(edge, 1);
                    if (last >= end)
                        last = edge;
                    else if (last >= edge)
                        last = sub(edge, 1);
                }
            }
        } else if (mode == WINDOW_SCHEDULE_REMAINING_SQUARED ||
                   mode == WINDOW_SCHEDULE_ELAPSED_SQUARED ||
                   mode == WINDOW_SCHEDULE_RANDOM_DELAY) {
            s32 delay = self->mDelay;
            if (delay != 0) {
                self->mDelay = u32(delay) - 1;
                first = 0;
                last = 0;
            } else {
                s32 remaining = self->mRemaining;
                last = sub(count, remaining);
                first = last;
                self->mRemaining = u32(remaining) - 1;
                s32 next;
                if (mode == WINDOW_SCHEDULE_REMAINING_SQUARED) {
                    u32 r = u32(remaining) - 1;
                    next = divided(s32(r * r), self->mIntervalDivisor);
                } else if (mode == WINDOW_SCHEDULE_ELAPSED_SQUARED) {
                    // HD: Count is reloaded here before the countdown store in the original.
                    u32 r = self->mItemCount - (u32(remaining) - 1);
                    next = divided(s32(r * r), self->mIntervalDivisor);
                } else {
                    config = self->mpConfig.get();
                    s32 maximum = config->mDuration;
                    next = s32(u32(ftoi(windowRandomDelay(f32(maximum)))) + 1);
                    if (next > maximum)
                        next = maximum;
                }
                self->mDelay = u32(next);
                s32 remainingNow = self->mRemaining;
                config = self->mpConfig.get();
                if (remainingNow <= 0)
                    progress = one;
            }
        }
    }
    u32 order = config->mOrder;
    if (order == WINDOW_ITEMS_FORWARD) {
        for (s32 i = first; i <= last; ++i) {
            if (i >= self->mItemCount)
                break;
            item(self, i);
        }
    } else if (order == WINDOW_ITEMS_REVERSE) {
        for (s32 i = first; i <= last; ++i) {
            s32 n = self->mItemCount;
            if (i >= n)
                break;
            item(self, sub(sub(n, i), 1));
        }
    } else if (order == WINDOW_ITEMS_CENTER_OUT) {
        for (s32 i = first; i <= last; ++i) {
            s32 n = self->mItemCount;
            if (n % 2 == 1) {
                s32 half = i / 2;
                s32 center = sub(n, 1) / 2;
                item(self, sub(center, half));
                item(self, s32(u32(center) + u32(half)));
            } else if (i != 0) {
                s32 half = sub(i, 1) / 2;
                s32 center = n / 2;
                item(self, sub(sub(center, half), 1));
                item(self, s32(u32(center) + u32(half)));
            }
        }
    } else if (order == WINDOW_ITEMS_EDGES_IN) {
        for (s32 i = first; i <= last; ++i) {
            s32 half = i / 2;
            item(self, half);
            s32 n = self->mItemCount;
            item(self, sub(sub(n, half), 1));
        }
    } else if (order == WINDOW_ITEMS_CONFIGURED) {
        for (s32 i = first; i <= last; ++i) {
            if (i >= self->mItemCount)
                break;
            s32 index = *gabi::at<be<s32>>(gabi::ea(self->mpOrder.get()) + u32(i) * 4);
            if (index >= 0)
                item(self, index);
        }
    }
    return !(progress < one);
}

/* 026F4FD8 */
void hdMsgPart_update(WindowPart *self) {
    if (!self->mpConfig.get())
        return;
    if (self->mScheduling) {
        f32 previous = self->mTime;
        f32 current = fadds_ppc(previous, l_windowPartOne);
        self->mTime = current;
        if (hdMsgPart_updateItems(self, previous, current))
            self->mScheduling = 0;
    } else {
        WindowAnimator *animator = self->mpCurrentAnimator.get();
        if (!animator)
            return;
        if (animator->mProgress != l_windowPartZero && !(animator->mFlags & 2))
            return;
        WindowPartConfig *config = self->mpConfig.get();
        self->mpCurrentAnimator = 0;
        if (config->mFinishEnabled)
            windowFinishPart(self, config);
    }
}
/* WWHD glue (tools/verify/autoglue.py)
data l_windowPartOne 10109A98
data l_windowPartZero 10109AA4
ext 026F47D0 void windowUpdateItem(WindowPart*,u32)
ext 020198D8 f32 windowRandomDelay(f32)
ext 026F4EA8 void windowFinishPart(WindowPart*,WindowPartConfig*)
*/
