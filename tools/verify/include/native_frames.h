/* The original's stack frame of a function (build/verify/frames.tsv, tools/verify/frames_all.py):
 * frame size and the prologue stores gabi replays (back chain, LR, callee-saved registers with
 * their entry values). Shared by gabi.h (consumer) and native_frames.cpp (loader). */
#pragma once
#include <cstdint>

struct GabiFrameItem {
    uint8_t kind; /* GABI_FI_* */
    uint8_t reg;
    uint16_t off; /* from the original's sp (after stwu) */
};
enum { GABI_FF_TAIL = 1 }; /* size 0: no frame, only tail branches (its calls run at the caller's sp) */
enum { GABI_FI_BACKCHAIN, GABI_FI_LR, GABI_FI_GPR, GABI_FI_STFD, GABI_FI_STFS0, GABI_FI_STFS1, GABI_FI_PSQ };
struct GabiFrameInfo {
    uint32_t size;  /* stwu r1,-size(r1) */
    uint32_t n;
    const GabiFrameItem* items;
    uint32_t flags; /* GABI_FF_* */
    uint32_t objects_lo; /* offset of the original's first stack object (0: unknown) */
};

extern "C" {
/* the frame of the original at addr, or null (no frame, unknown, or automatic frames disabled) */
const GabiFrameInfo* gmem_frame_info(uint32_t addr);
}
