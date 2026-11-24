/* InputState_t60.h (WWHD, true 60 build; 2026-11-24: a press landing on an
 * in-between step is lost by the message flow). Decomp build only. Included by
 * sead/InputState.cpp only (host state of that unit).
 *
 * The frame-level work (the first-step frame-level rule, WWHD_T60_SCENE_FIRST: the play scene's execute, which runs the HD in-game UI's
 * screens, the message windows among them) runs on the frame's FIRST step and reads the controller's press word as the
 * frame's first read left it. A press that arrives on a later step of frame f (per-step input: the GamePad is read on every
 * step; WWHD_T60_TEST_INPUT_STEP=0.5 puts every scripted press there) is added to frame f's press union for the
 * tick30-only code, but the frame-level work of frame f has already run, and frame f + 1's first read starts a new word
 * (the button is held by then: no edge). The message window never saw it: an event route with the aid, the presses from
 * 8 s on did not advance the message-only window (hdScreenMsgOnly2_c::continueExecute's decide, its 0x80B advance sound),
 * the event never gave Link DEMO_TOOL (A frame 477), link 52 / camera 81 off; without the aid strict yes.
 * Form (the mixed-pair hit-flag rule (2): the press acts, at the latest at the next step that reads it): the frame's first read adds
 * the presses that arrived on the previous frame's later steps (its union less its own first read's presses) to its
 * press word, so the frame-level work on the first step sees them once; the frame's first later read takes them out of
 * the union again before adding its own edges, so the tick30-only code (which saw them in the previous frame's union)
 * does not see them twice, and converted code keeps its own step edges (t60_in_edges gets the first read's own presses).
 * N = 1 and the verification harness (no later reads): the carried set is always empty, the original. Negative control
 * WWHD_T60_LATEPRESS_NEGCTRL=1 (game build). Host memory, a few controller states. */
#pragma once
#include "true60.h"
#include <cstdlib>

namespace sead_t60 {
struct LatePress {
    u32 ea;        /* the InputState's guest address */
    u32 firstPress; /* the presses of its last first read (without the carried set) */
    u32 carried;    /* the carried set added at the last first read, until the frame's first later read */
    bool valid;
};
static LatePress s_late[16];

static inline bool latePressNegctrl() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool nc = [] { const char* e = getenv("WWHD_T60_LATEPRESS_NEGCTRL"); return e && e[0] == '1'; }();
    return nc;
#else
    return false;
#endif
}
static inline LatePress* latePressRec(u32 ea) {
    LatePress* free = nullptr;
    for (LatePress& r : s_late) {
        if (r.valid && r.ea == ea)
            return &r;
        if (!r.valid && !free)
            free = &r;
    }
    if (free) {
        free->ea = ea;
        free->firstPress = 0;
        free->carried = 0;
        free->valid = true;
        free->carried = 0;
        return free;
    }
    return nullptr;
}
/* the frame's first read, before its press word is written: union = the press word the previous frame left (its first
 * read's presses + its later steps' presses); press = this read's own presses. Returns the presses to carry (0: none). */
static inline u32 latePressFirst(u32 ea, u32 unionWord, u32 press) {
    if (latePressNegctrl() || t60::steps() <= 1)
        return 0;
    LatePress* r = latePressRec(ea);
    if (!r)
        return 0;
    const u32 carry = unionWord & ~(r->firstPress | r->carried);
    r->firstPress = press;
    r->carried = carry & ~press;
    return r->carried;
}
/* the frame's later read, before it adds its edges: the carried set leaves the union (once per frame) */
static inline u32 latePressLater(u32 ea) {
    for (LatePress& r : s_late) {
        if (r.valid && r.ea == ea) {
            const u32 c = r.carried;
            r.carried = 0;
            return c;
        }
    }
    return 0;
}
} // namespace sead_t60
