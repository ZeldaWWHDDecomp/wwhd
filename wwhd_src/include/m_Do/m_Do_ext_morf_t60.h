/* m_Do_ext_morf_t60.h (WWHD, true 60 build, unit option (b)):
 * the morf blend on the 30 Hz grid, for mDoExt_McaMorf (m_Do_ext_morf_calc.cpp) and mDoExt_McaMorf2
 * (m_Do_ext_morf2_calc.cpp).
 *
 * The original calc lerps the STORED blended pose (mpQuat / mpTransformInfo) toward the current animation pose by
 * (cur - prev) / (1 - prev), once per frame. Per step (dt = 1/N) each step blends partly toward the in-between animation
 * pose, so while a morf runs the blended pose lags A's (kmoH2_2: the Moblin's oshi shaft ~0.25 frame).
 *
 * The exact form: the frame's first calc keeps the stored pose (the frame-start pose, A's stored pose) and the
 * frame-start morf value; every calc of the frame restores the stored pose of the joint from that snapshot and blends
 * with the frame's cumulative ratio (cur - prev0) / (1 - prev0), so the tick30 calc gives A's pose exactly.
 *   - the snapshot is taken on every frame's first calc, also when no morf runs then: a morf set later in the frame
 *     blends from the frame-start pose, as A's does;
 *   - a morf set again within the frame (setMorf: prev back below the snapshot's) keeps the frame-start pose and takes
 *     the new start value;
 *   - dt = 1 (N = 1, the 30 Hz fallback, every unconverted process): nothing here runs, the original code; except the
 *     draw's calc of a morf with a snapshot of this frame (morf_t60_draw_snap below): the frame's prev, as A's draw.
 * Frame tag = g_Counter.mCounter0 (counts 30 Hz frames, advanced after the tick30 step's execute, f_ap_game.cpp).
 * Host map keyed by the morf's guest address; the entry also checks the pose arrays and joint count. The pose words go
 * through gabi::load / store (no direct host access to guest memory; verify's guard). */
#pragma once
#include "true60.h"
#include <unordered_map>
#include <vector>

struct MorfT60Snap {
    u32 tag = 0xFFFFFFFFu, quats = 0, transforms = 0, count = 0;
    f32 prev = 0.0f;
    std::vector<u32> quat, info; /* the words of mpQuat (4 per joint) and mpTransformInfo (8 per joint) */
};

/* the snapshots, keyed by the morf's guest address */
static inline std::unordered_map<u32, MorfT60Snap>& morf_t60_snaps() {
    static std::unordered_map<u32, MorfT60Snap> s_snaps;
    return s_snaps;
}

/* Call before the joint loop. Returns the snapshot to blend from (the morf blends this calc), or nullptr: the original
 * path (dt = 1, no pose arrays, or no morf running). */
static inline MorfT60Snap* morf_t60_snap(u32 morf, u32 count, u32 quats, u32 transforms, f32 cur, f32 prev, f32 one) {
    if (t60::dt() == 1.0f || !quats || !transforms)
        return nullptr;
    MorfT60Snap& s = morf_t60_snaps()[morf];
    u32 tag = gabi::load<u32>(0x101FF558); /* g_Counter.mCounter0 */
    if (s.tag != tag || s.quats != quats || s.transforms != transforms || s.count != count) {
        /* the frame's first calc: the frame-start pose and morf value */
        s.tag = tag;
        s.quats = quats;
        s.transforms = transforms;
        s.count = count;
        s.prev = prev;
        s.quat.resize(count * 4u);
        s.info.resize(count * 8u);
        for (u32 w = 0; w < count * 4u; w++)
            s.quat[w] = gabi::load<u32>(quats + w * 4u);
        for (u32 w = 0; w < count * 8u; w++)
            s.info[w] = gabi::load<u32>(transforms + w * 4u);
    } else if (prev < s.prev) {
        /* a morf set within the frame (setMorf: prev 0): blend from the frame-start pose with the new start value */
        s.prev = prev;
    }
    return cur < one ? &s : nullptr;
}
/* The draw's calc (mDoExt_McaMorf::updateDL, or any calc at dt = 1 after this frame's converted steps: ReDead pair
 * rdS1_2, 2026-11-19). A's draw calc blends its stored pose once more with the frame's ratio (cur - prev) / (1 - prev),
 * prev set once by the frame's play. A converted morf's play sets prev on every step (and updateDL after each step's draw),
 * so the original code there blended B's exact frame-end pose with a smaller ratio (rdS1_2 frame 781: prev 0.1875 for
 * A's 0.125): the draw-time pose and what the joint callbacks write from it (the ReDead's mRdEyePos) lagged A's, and the
 * next frame's snapshot took that pose. At dt = 1 with N > 1 and a snapshot of this frame (same tag, arrays, joint count)
 * the blend uses the snapshot's frame-start prev on the stored pose: no restore, no snapshot or morf field written;
 * not after a morf set since the frame's last calc (prev below the snapshot's).
 * Returns the snapshot, or nullptr: the original code (N = 1, any other dt = 1 calc). */
static inline const MorfT60Snap* morf_t60_draw_snap(u32 morf, u32 count, u32 quats, u32 transforms, f32 cur, f32 prev,
                                                    f32 one) {
    if (t60::steps() == 1 || t60::dt() != 1.0f || !quats || !transforms || !(cur < one))
        return nullptr;
    auto& snaps = morf_t60_snaps();
    auto it = snaps.find(morf);
    if (it == snaps.end())
        return nullptr;
    const MorfT60Snap& s = it->second;
    u32 tag = gabi::load<u32>(0x101FF558); /* g_Counter.mCounter0 */
    /* a morf set after the frame's last calc (setMorf: prev back below the snapshot's, e.g. 0 after a frame-start 1)
     * blends with its own new start value, as A's draw does (rdT1_2 frames 122-206: the snapshot's prev 1 gave a 0 / 0
     * ratio, the head pose NaN) */
    return s.tag == tag && s.quats == quats && s.transforms == transforms && s.count == count && !(prev < s.prev) &&
                   s.prev < one
               ? &s
               : nullptr;
}
/* before joint i's blend: its stored pose back to the frame start */
static inline void morf_t60_restore(MorfT60Snap* s, u32 i, u32 quat, u32 info) {
    for (u32 w = 0; w < 4u; w++)
        gabi::store<u32>(quat + w * 4u, s->quat[i * 4u + w]);
    for (u32 w = 0; w < 8u; w++)
        gabi::store<u32>(info + w * 4u, s->info[i * 8u + w]);
}
