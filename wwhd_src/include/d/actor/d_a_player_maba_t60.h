#pragma once
/* d_a_player_maba_t60.h (WWHD, true 60 build, decomp build only): the blink timer's random draw on the 30 Hz grid
 * (dt = 1/N the 30 Hz visual-motion rule, to frame-75 RNG item). .
 *
 * daPy_lk_c::setTextureScrollResource (main_01, 023DC43C) ends with setMabaTimer: the guest byte 0x101CEF19 (main_01's
 * `m_morf_frame` binding) = cM_rndF(30) + 75, the frames until the next blink, read only by setTextureAnime's face
 * texture choice: visual only, no collision, no decision of play or of another actor. It runs on every face texture
 * change, i.e. inside animation setters (procedure inits on the frame's FIRST step, first-step input / first-step AI-timer). There the draw came
 * before the unconverted processes' tick30 draws of the same frame (frame 75: Link's procCutL_init draw before
 * the ship 0247AE78's; A: the ship first): one seed apart from there on, by order not count.
 *
 * Form (N > 1 only; N = 1 is the original, nothing here is consulted): on a step before the tick30 step the draw is not
 * made (the byte keeps the 1 the function stores just before it: non-zero, so setTextureAnime decides the same until the
 * frame's draw) and is noted in the port-owned set latch (keys kKey + k, one per deferred draw, tagged with the 30 Hz
 * frame counter g_Counter.mCounter0); on Link's tick30 execute the frame's deferred draws are made in order (the last
 * one is the timer, as in the original): at the matching tick30 setTextureAnime call (positional flush below), at the
 * end of the procedure body, or at the end of the execute; at its start when Link made no first-step execute. The draw count per frame is unchanged (the per-step chance rule);
 * Link's draws keep their place after the processes that execute before him.
 *
 * NOTE: main_01 and main_04 bind the name m_morf_frame to DIFFERENT guest bytes (main_01 0x101CEF19, the
 * byte setTextureScrollResource draws into; main_04 0x101CEF1B, the byte decMorfFrame counts down, main_01's
 * `m_maba_flg`). The flush therefore writes 0x101CEF19 by address: attempt passed main_04's
 * m_morf_frame and wrote the deferred draw into 0x101CEF1B, leaving 0x101CEF19 at 1 (pmaba5: B's setTextureAnime then
 * made a second draw on the tick30 step where A made one). */
#include "true60.h"

namespace lkT60Maba {
static constexpr uint32_t kTimer = 0x101CEF19u; /* setMabaTimer's byte (main_01 `m_morf_frame`) */
static constexpr uint32_t kKey = 0x901CEF19u;   /* latch keys: the byte's address with the top bit (no guest key) */
static constexpr uint32_t kMax = 48;            /* queue slots in one frame (entries and markers) */
static inline uint32_t frameTag() { return *gabi::at<be<uint32_t>>(0x101FF558); /* g_Counter.mCounter0 */ }
/* the draw waits for the frame's tick30 step (true on an earlier step at N > 1: a converted execute, or Link's draw on an
 * in-between pass before his conversion, frame 11) */
static inline bool defer() { return t60::steps() != 1 && !t60::tick30(); }
/* setTextureAnime's per-call draws (rule 1 2026-11-17, pmaba10 frame 75): the upfront dVar9 draw
 * and the 2.5% eye-glance chance are made once per CALL in the original. At N > 1 a converted Link calls setTextureAnime
 * on the frame's first step as the original frame does (procedure inits, first-step AI-timer; the every-step move callers too), and
 * again on later steps (the every-step callers only). The first-step calls count: each notes its dVar9 draw (and its
 * glance chance where the call reaches it) in the same ordered queue as the blink-timer notes, and the flush makes them
 * in note order (A's order inside one call: dVar9, then the glance chance or setTextureScrollResource's timer draw).
 * Later-step calls draw nothing (their first-step call drew), except the eye-move pick's own value when the event
 * really starts on a later step. A first-step call that reaches the eye-move pick draws dVar9 at once (the pick needs
 * it) and notes nothing. */
static constexpr uint32_t kKeyD = 0x911CEF19u; /* queue slot k holds setTextureAnime's dVar9 draw */
static constexpr uint32_t kKeyG = 0x921CEF19u; /* queue slot k holds setTextureAnime's glance chance */
/* positional flush (after pmaba13 frame 45): the draws noted on the first step are
 * made where A makes them, inside the procedure: the k-th tick30-step setTextureAnime call of a segment makes the
 * entries of the k-th matchable first-step call of that segment (its dVar9, then its glance or timer draw, up to the
 * next marker). Markers (queue slots, no draw): kKeyC a first-step setTextureAnime call starts; kKeyI a procedure init
 * starts (commonProcInit): the calls after it up to the segment end are first-step-only (no tick30 counterpart); kKeyE
 * the first step's procedure body returned (segment 0 = execute up to and with the body, segment 1 = after it); kKeyX
 * Link's first-step execute started (the queue was wiped there). Unmatched entries are made before the next matched
 * flush (A's order: the queue is in A's call order), the rest of segment 0 right after the tick30 body (the point the
 * procedure would have called it), everything left at the end of the tick30 execute. kKeyF + k: slot k made. */
static constexpr uint32_t kKeyC = 0x931CEF19u;
static constexpr uint32_t kKeyE = 0x941CEF19u;
static constexpr uint32_t kKeyI = 0x951CEF19u;
static constexpr uint32_t kKeyX = 0x961CEF19u;
static constexpr uint32_t kKeyF = 0x971CEF19u;
static constexpr uint32_t kKeyM = 0x981CEF19u;  /* + s * 0x40 + j: the tick30 step's j-th call of segment s seen */
static constexpr uint32_t kKeyTE = 0x991CEF19u; /* the tick30 step's body returned (segment 1) */
/* the guarded knockback's stun chance (gd64c frame 368: the per-step chance rule / the procWait
 * idle-timer form below): procGuardSlip_init / procCrouchDefenseSlip_init (main_03) draw mProcVar6 = cM_rnd < 0.2 right
 * after their setSingleMoveAnime, whose setTextureAnime dVar9 draw is noted here on a converted first step. Drawn at
 * once, the chance came BEFORE that dVar9 in Link's cM_rnd sequence (A: after it): the two values swapped (gd64c: A
 * NOCK_BACK_END at 379, B not). Now noted in the same queue after the dVar9 and made in A's order on the tick30 step;
 * the value is stored while Link is still in the slide (mProcVar6 is read only once its speed has died down, frames
 * later; a procedure left within the frame re-inits it, as the original's later init overwrote it). */
static constexpr uint32_t kKeyK = 0x9A1CEF19u;
/* procWait's idle-timer draw (2026-11-19: the criterion is the REAL RNG's per-frame draw sequence,
 * caller and order, equal to A's): mProcVar1 = cM_rndF(150) + 300, drawn in procWait's first-step countdown block (rule
 * the first-step AI-timer rule). deferred it to the START of Link's tick30 execute (lkT60WaitRnd), i.e. before the draws
 * of his tick30 execute that A makes before it (the body's queued setTextureAnime draws, ...). Now noted in this queue
 * at its place in the first-step body and made in A's order on the tick30 step (the rest of segment 0 is made right
 * after the tick30 body, where the original body ended); stored while Link is still in procWait. */
static constexpr uint32_t kKeyW = 0x9B1CEF19u;
/* (kf-rt70q frame 339) the blink timer was set by an entry made earlier in this frame's queue (a timer
 * entry, or a glance that hit): a later glance entry's chance is then not reached (the original's condition at its call,
 * m_morf_frame == 0, was true when it was noted). form read the live byte, which a LATER first-step
 * setTextureScrollResource (a procedure init's face change, deferred: byte 1) had already set: kf-rt70q frame 339 (procWait
 * -> procFreeWait_init) lost the glance chance of the frame's first setTextureAnime call (B one draw short). Not a slot. */
static constexpr uint32_t kKeyH = 0x9C1CEF19u;
/* checkNormalFace's 1% face pick (km74 / kf-rt70q frame 339; 2026-11-19 WAITRND rule: the per-frame
 * draw outside the queue goes through it): checkNormalFace (main_01, 023DD36C, called only from setTextureAnime with face
 * 0) draws cM_rnd() for the rare face while Link is in procWait without an event (guest 023DD67C), right BEFORE the call's
 * dVar9.'s form drew it on the tick30 step only, outside the queue: on a normal procWait frame the count and
 * order matched A (tick30 call: pick, then the flush's dVar9), but on a frame whose first step left procWait
 * (procFreeWait_init, frame 339) the tick30 call was no longer in WAIT: no pick, B one draw short before the idle choice
 * (the choice took A's dVar9 value). Now a converted first-step call notes the pick here (in its call group, before its
 * dVar9; the first step keeps the common outcome as before), the flush makes it in note order, and the matching tick30
 * call takes the made value for its face (pickValue). Later steps take the common outcome; an unmatched tick30 call draws its own pick (unmatched_tick30). */
static constexpr uint32_t kKeyP = 0x9D1CEF19u;
inline float g_pickValue[kMax];               /* host: the made pick value of slot k (valid while the slot's made flag is set) */
/* setTextureAnime's eye-move event (rt77m frames 121 / 208; form (1): Link's
 * shared-RNG draws are made on the tick30 step at his place in the process order). At the blink timer's end
 * (m_morf_frame 0, the scroll animation not TMABA) the call draws dVar9 up front and picks the eye move with it
 * (TMABA or one of eight directions), sets the scroll animation (setTextureScrollResource: the timer draw) and the
 * maba flag. A converted first-step call drew dVar9 at once (now): in the real RNG before the unconverted processes
 * that execute before Link on the tick30 step (Outset palms daObjLpalm 0236CB54, 10 draws: rt77m B two seeds ahead at
 * frame 121). Now such a call notes the event in its call group and holds it: the first step keeps the scroll animation,
 * the timer byte reads 1 (as a deferred setTextureScrollResource leaves it: the frame's later first-step calls decide as
 * after the event: no second event, no glance), and the flush makes it in note order on the tick30 step (dVar9, the
 * choice, the scroll animation and its timer draw at once). Consumers: the face texture only (scroll heap mIdx, the scroll
 * animation, the timer and maba bytes; read by later setTextureAnime calls), no decision of Link's logic: the face's
 * eye move shows one 30 Hz frame later. g_evIdx / g_evP2: the scroll mIdx and the frame argument at the note. */
static constexpr uint32_t kKeyV = 0x9E1CEF19u;
inline u16 g_evIdx[kMax];
inline u32 g_evP2[kMax];
inline uint32_t g_matchLo = 1u, g_matchHi = 0u; /* host: the slot range of the tick30 call's matched first-step call (empty: none) */
/* procFreeWait_init's random part (rt77e frame 339; plan 2026-11-20, t60once form): entered on a
 * converted first step (procWait's idle countdown, main_04b), the init drew the frame's noted draws (now()) and its
 * animation choice (cM_rnd) at once: in the real RNG before the unconverted processes that execute before Link on the
 * tick30 step (rt77e: B's first step three seeds ahead at frame 339). Now the init notes kKeyR in the queue (after its
 * kKeyI marker) and the first step keeps the pre-init animation (WAITS) and mProcVar6; commonProcInit, mNormalSpeed,
 * mDirection and the angle stay at the init. The flush makes it in note order (the noted entries before it first): the
 * choice, mProcVar6 and setSingleMoveAnime(anm, 1, 0, -1, 5) as the tick30 step's call at dt 1 (its setTextureAnime /
 * checkNormalFace / setMabaTimer draws at once, at that place: A's order inside the init). kKeyRB (no slot) marks the
 * make in progress: a flush reached from inside it (setTextureAnime's now()) does nothing (re-entry guard). Readers of
 * the held fields (all wait one 30 Hz frame part): procFreeWait (mProcVar6: the voices at anim frames 105 / 168; the body
 * is held on the frame's later steps), procWait_init (mCurProc and the rate only), setSingleMoveAnime's morph / hand /
 * SE / face (picture and sound). The new animation plays from the next frame either way (lkT60_markAnmSet). */
static constexpr uint32_t kKeyR = 0x9F1CEF19u;
static constexpr uint32_t kKeyRB = 0xA01CEF19u;
static constexpr uint32_t kTypes[] = {kKey, kKeyD, kKeyG, kKeyC, kKeyE, kKeyI, kKeyX, kKeyK, kKeyW, kKeyP, kKeyV, kKeyR};
/* slot k's entry type (its base key), 0 when free */
static inline uint32_t slot(uint32_t k, uint32_t tag) {
    for (uint32_t t : kTypes)
        if (wwhd_t60_latch_pending(t + k, tag))
            return t;
    return 0;
}
static inline bool conv() { return t60::steps() != 1 && t60::dt() != 1.0f; }
/* note one deferred entry of this frame (base: kKey timer, kKeyD dVar9, kKeyG glance, or a marker) in the next free slot */
static inline void note_as(uint32_t base) {
    const uint32_t tag = frameTag();
    for (uint32_t k = 0; k < kMax; k++) {
        if (!slot(k, tag)) {
            wwhd_t60_latch_set(base + k, tag);
            return;
        }
    }
}
static inline void note() { note_as(kKey); }
/* note the eye-move event (kKeyV) with its scroll mIdx and frame argument; the first step's timer byte then reads 1 */
static inline void note_event(u16 idx, u32 p2) {
    const uint32_t tag = frameTag();
    for (uint32_t k = 0; k < kMax; k++) {
        if (!slot(k, tag)) {
            g_evIdx[k] = idx;
            g_evP2[k] = p2;
            wwhd_t60_latch_set(kKeyV + k, tag);
            break;
        }
    }
    gabi::store<u8>(kTimer, 1);
}
/* the start of Link's FIRST-step execute (N > 1): the queue starts empty. pmaba11: the port replays the
 * frame's steps on hold / record passes (executes run, the actors and the RNG are put back at pass end), but the host
 * latch is not put back: a first-step execute replayed there noted its draws a second time (B's Link one draw more per
 * frame). Wiping the queue here makes the replay re-note the same slots (procWait's single key was idempotent already).
 * Erasing = asking for a tag the entry does not carry (wwhd_t60_latch_pending drops it). The made flags (kKeyF + k)
 * and the timer-set flag (kKeyH) go too (kf-rt71q frame 339): since now() entries are
 * also MADE on the first step, and a replayed first step's draws are put back with the RNG at its pass end; flags
 * kept from it made the next pass's now() skip those slots (B one draw short before procFreeWait_init's choice).
 * The tick30 call counts (kKeyM) and kKeyTE are set on the tick30 step only (after every first step of the frame). */
static inline void begin_first_step() {
    if (t60::steps() == 1 || !t60::first_step())
        return;
    const uint32_t other = frameTag() ^ 0x80000000u;
    for (uint32_t k = 0; k < kMax; k++) {
        for (uint32_t t : kTypes)
            wwhd_t60_latch_pending(t + k, other);
        wwhd_t60_latch_pending(kKeyF + k, other);
    }
    wwhd_t60_latch_pending(kKeyH, other);
    wwhd_t60_latch_pending(kKeyRB, other);
    if (conv())
        note_as(kKeyX);
}
static inline bool flushing() { return t60::steps() != 1 && t60::tick30(); }
/* kKeyR (above): procFreeWait_init's choice, mProcVar6 and its setSingleMoveAnime, made by the flush */
static inline void make_freewait(uint32_t link, uint32_t tag) {
    const f32 choice = cM_rnd(); /* procFreeWait_init's animation choice (cM_rnd 02019788) */
    if (gabi::load<s32>(link + 0x65F0) != 5 /* mCurProc == daPyProc_FREE_WAIT_e */)
        return; /* Link left FREE_WAIT before the flush (the tick30 step's execute start): the draw only */
    int anm = 0xE3; /* ANM_FREED */
    s32 v6 = 0;
    if (choice < 0.3333f) {
        anm = 0xE1; /* ANM_FREEA */
    } else if (choice < 0.6666f) {
        anm = 0xE2; /* ANM_FREEB */
        v6 = 1;
    }
    gabi::store<s32>(link + 0x69C0, v6); /* mProcVar6 */
    const u8 timer = gabi::load<u8>(kTimer);
    wwhd_t60_latch_set(kKeyRB, tag);
    const bool tick = flushing();
    const float pdt = tick ? t60_set_dt(1.0f) : 0.0f; /* the tick30 step's call at dt 1: its draws at once, no queue step */
    gabi::call<BOOL>(0x023E0A04 /* daPy_lk_c::setSingleMoveAnime */, link, anm, 1.0f, 0.0f, -1, 5.0f);
    if (tick)
        t60_set_dt(pdt);
    wwhd_t60_latch_pending(kKeyRB, tag ^ 0x80000000u);
    /* the init's face change set the blink timer (its setTextureScrollResource or a glance hit): a later glance entry of
     * this frame is not reached, as in the original (kKeyH) */
    if (gabi::load<u8>(kTimer) != timer)
        wwhd_t60_latch_set(kKeyH, tag);
}
/* make the unmade entries of slots [0, last] in order (link: Link's guest address; the glance applies to his scroll heap) */
static inline void flush_upto(uint32_t link, uint32_t last) {
    const uint32_t tag = frameTag();
    if (wwhd_t60_latch_pending(kKeyRB, tag))
        return; /* inside make_freewait's setSingleMoveAnime (re-entry guard) */
    f32 d = 0.0f; /* the last dVar9 drawn: the glance entry after it picks with it, as in its call */
    for (uint32_t k = 0; k <= last && k < kMax; k++) {
        const uint32_t t = slot(k, tag);
        if (!t)
            break;
        if (wwhd_t60_latch_pending(kKeyF + k, tag))
            continue;
        wwhd_t60_latch_set(kKeyF + k, tag);
        if (t == kKey) {
            gabi::store<u8>(kTimer, (u8)gabi::ftoi(cM_rndF(30.0f) + 75.0f)); /* setMabaTimer */
            wwhd_t60_latch_set(kKeyH, tag);
        } else if (t == kKeyP) {
            g_pickValue[k] = cM_rnd(); /* checkNormalFace's pick (Player01_random, cM_rnd 02019788) */
        } else if (t == kKeyD) {
            d = cM_rnd(); /* Player01_random (cM_rnd 02019788) */
        } else if (t == kKeyK) {
            const bool stun = cM_rnd() < 0.2f; /* LinkMotionApi::randomWide (cM_rnd 02019788) */
            const s32 proc = gabi::load<s32>(link + 0x65F0); /* mCurProc */
            if (proc == 0x6D /* GUARD_SLIP */ || proc == 0xD /* CROUCH_DEFENSE_SLIP */)
                gabi::store<s32>(link + 0x69C0, stun ? 1 : 0); /* mProcVar6 */
        } else if (t == kKeyR) {
            make_freewait(link, tag);
        } else if (t == kKeyW) {
            const s16 v = (s16)gabi::ftoi(cM_rndF(150.0f) + 300.0f); /* procWait's idle time */
            if (gabi::load<s32>(link + 0x65F0) == 4 /* mCurProc == daPyProc_WAIT_e */)
                gabi::store<s16>(link + 0x6918, v); /* mProcVar1 */
        } else if (t == kKeyV) {
            /* the eye-move event (note_event above): dVar9, the choice (setTextureAnime's table), the scroll animation */
            d = cM_rnd(); /* Player01_random (cM_rnd 02019788) */
            u16 btk = 0x1A7; /* dRes_INDEX_LKANM_BTK_TMABA_e */
            u32 p2 = g_evP2[k];
            if (!(d < 0.44f)) {
                p2 = 0;
                if (d < 0.51f) btk = 0x187;
                else if (d < 0.58f) btk = 0x183;
                else if (d < 0.65f) btk = 0x184;
                else if (d < 0.72f) btk = 0x185;
                else if (d < 0.79f) btk = 0x181;
                else if (d < 0.86f) btk = 0x182;
                else if (d < 0.93f) btk = 0x188;
                else btk = 0x186;
            }
            const uint32_t h = link + 0x65E0; /* m_tex_scroll_heap: mIdx, mPriIdx, mDemoIdx */
            const bool free = gabi::load<u16>(h + 2) == 0xFFFF && gabi::load<u16>(h + 4) == 0xFFFF;
            if (gabi::load<u16>(h) == g_evIdx[k]) {
                gabi::store<u16>(h, btk);
                if (free) {
                    /* loadTextureScrollResource 023DC4DC, setTextureScrollResource 023DC43C (here: the timer draw at once) */
                    gabi::call<void>(0x023DC43C, link, gabi::call<u32>(0x023DC4DC, link, (u32)btk, 0u), p2);
                    wwhd_t60_latch_set(kKeyH, tag);
                }
            } else if (free) {
                /* a later first-step call of the frame changed the scroll animation (its own timer entry follows): the
                 * event's timer draw only */
                gabi::store<u8>(kTimer, (u8)gabi::ftoi(cM_rndF(30.0f) + 75.0f));
                wwhd_t60_latch_set(kKeyH, tag);
            }
            if (btk != 0x1A7)
                gabi::store<u8>(0x101CEF18, 1); /* m_maba_timer = 1 (onMabaFlg) */
        } else if (t == kKeyG) {
            /* the chance needs the timer byte still 0 (the original's condition at the call, true when noted): an
             * entry made before it in this frame's queue that set the timer (kKeyH) means the original's call did not
             * reach the chance; the live byte is not asked (a later deferred face change holds 1 there) */
            if (wwhd_t60_latch_pending(kKeyH, tag))
                continue;
            if (cM_rnd() < 0.025f) {
                wwhd_t60_latch_set(kKeyH, tag);
                const uint32_t h = link + 0x65E0; /* m_tex_scroll_heap: mIdx, mPriIdx, mDemoIdx */
                u16 idx;
                if (d < 0.125f) idx = 0x187;
                else if (d < 0.25f) idx = 0x183;
                else if (d < 0.375f) idx = 0x184;
                else if (d < 0.5f) idx = 0x185;
                else if (d < 0.625f) idx = 0x181;
                else if (d < 0.75f) idx = 0x182;
                else if (d < 0.875f) idx = 0x186;
                else idx = 0x188;
                if (gabi::load<u16>(h) == 0x1A7 && gabi::load<u16>(h + 2) == 0xFFFF &&
                    gabi::load<u16>(h + 4) == 0xFFFF) {
                    gabi::store<u16>(h, idx);
                    /* loadTextureScrollResource 023DC4DC, setTextureScrollResource 023DC43C (tick30: draws the timer) */
                    gabi::call<void>(0x023DC43C, link, gabi::call<u32>(0x023DC4DC, link, (u32)idx, 0u), 0);
                    gabi::store<u8>(0x101CEF18, 1); /* m_maba_timer = 1 (onMabaFlg) */
                } else {
                    /* a later first-step call already changed the scroll animation (its own timer note follows): the
                     * original's glance timer draw only */
                    gabi::store<u8>(kTimer, (u8)gabi::ftoi(cM_rndF(30.0f) + 75.0f));
                }
            }
        }
    }
}
/* (kf-rt70q frame 339) a draw Link must make at once on a converted first step (a decision: procFreeWait_init's
 * animation choice, setTextureAnime's eye-move pick): the entries noted before it this frame are made first, so Link's
 * draws keep A's order among themselves (A: the frame's earlier setTextureAnime dVar9 / glance draws, then the choice;
 * form drew the choice first: kf-rt70q B FREEB with A's FREEA value one place early, the face anim
 * diverged at 539). Made here = marked made (kKeyF): the tick30 flushes skip them. Steps 1 / dt 1 / a later step: nothing. */
static inline void now(uint32_t link) {
    if (!conv() || !t60::first_step())
        return;
    flush_upto(link, kMax - 1);
    flush_upto(link, kMax - 1); /* a glance hit made above notes its timer draw (first step: deferred) at the queue end */
}
/* a first-step setTextureAnime call starts (converted) */
static inline void call_first() { note_as(kKeyC); }
/* commonProcInit on a converted first step: the setTextureAnime calls after it are first-step-only */
static inline void proc_init() {
    if (conv() && t60::first_step())
        note_as(kKeyI);
}
/* a tick30-step setTextureAnime call of a converted Link, before its own draws: makes the matching first-step call's
 * entries (and the unmade ones before them) */
static inline void call_tick30(uint32_t link) {
    g_matchLo = 1u;
    g_matchHi = 0u;
    if (!flushing())
        return;
    const uint32_t tag = frameTag();
    const uint32_t seg = wwhd_t60_latch_pending(kKeyTE, tag) ? 1u : 0u;
    uint32_t j = 0;
    while (j < 0x40 && wwhd_t60_latch_pending(kKeyM + seg * 0x40 + j, tag))
        j++;
    if (j >= 0x40)
        return;
    wwhd_t60_latch_set(kKeyM + seg * 0x40 + j, tag);
    uint32_t s = 0, n = 0;
    bool init = false;
    for (uint32_t k = 0; k < kMax; k++) {
        const uint32_t t = slot(k, tag);
        if (!t)
            return; /* no k-th matchable call: nothing to make here */
        if (t == kKeyE) {
            s = 1;
            init = false;
        } else if (t == kKeyI) {
            init = true;
        } else if (t == kKeyC && s == seg && !init) {
            if (n++ == j) {
                uint32_t last = k;
                for (uint32_t q = k + 1; q < kMax; q++) {
                    const uint32_t u = slot(q, tag);
                    if (!u || u == kKeyC || u == kKeyE || u == kKeyI || u == kKeyX)
                        break;
                    last = q;
                }
                flush_upto(link, last);
                g_matchLo = k;
                g_matchHi = last;
                return;
            }
        }
    }
}
/* checkNormalFace's pick (kKeyP), converted Link only (dt 1 draws Player01_random at the call, the original): the value
 * its face decision uses. A converted first-step call notes the pick and takes the common outcome (1.0: no rare face), as before; the
 * tick30 call takes the value made for the pick of its matched first-step call (call_tick30 runs before checkNormalFace,
 * at the start of setTextureAnime), 1.0 when that call made no pick (the frame's first step was not in procWait); other
 * steps 1.0. The draw count per frame follows the first step (A's frame, first-step AI-timer). */
static inline f64 pick() {
    if (t60::first_step()) {
        note_as(kKeyP);
        return 1.0;
    }
    if (!flushing())
        return 1.0;
    if (g_matchLo > g_matchHi)
        return cM_rnd(); /* unmatched tick30 call (unmatched_tick30 below): the original call's pick, at the call */
    const uint32_t tag = frameTag();
    for (uint32_t k = g_matchLo; k <= g_matchHi && k < kMax; k++)
        if (slot(k, tag) == kKeyP && wwhd_t60_latch_pending(kKeyF + k, tag))
            return g_pickValue[k];
    return 1.0;
}
/* (km75 frame 539, the call_first / call_tick30 matching principle) a tick30-step
 * setTextureAnime call that call_tick30 matched to no first-step call (the frame's first step made no such call: A's
 * original frame makes it once, here) is the original frame's call: it makes its own draws at the call, in A's order
 * (checkNormalFace's pick, dVar9, the glance chance where it reaches it; its timer draw is made at once on the tick30
 * step anyway). km75 frame 539: B's first step (procWait) made no call, the tick30 call drew neither the pick nor dVar9
 * (1.0 / none): B two draws short, the timer took A's pick value (83 frames, A 96), and frames 622 / 623 / 635 (the next
 * blink 13 frames early in B: an extra glance chance) followed from it. Valid right after call_tick30 in the same call. */
static inline bool unmatched_tick30() { return flushing() && g_matchLo > g_matchHi; }
/* Link's procedure body returned: the first step marks the segment end; the tick30 step makes segment 0's unmade
 * entries (first-step-only calls, the procedure's calls without a tick30 counterpart) here */
static inline void body_end(uint32_t link) {
    if (!conv())
        return;
    if (t60::first_step()) {
        note_as(kKeyE);
        return;
    }
    if (!t60::tick30())
        return;
    const uint32_t tag = frameTag();
    wwhd_t60_latch_set(kKeyTE, tag);
    for (uint32_t k = 0; k < kMax; k++) {
        const uint32_t t = slot(k, tag);
        if (!t)
            return;
        if (t == kKeyE) {
            flush_upto(link, k);
            return;
        }
    }
}
/* the start of Link's tick30 execute: entries noted while Link made no first-step execute this frame (no kKeyX: Link at
 * dt 1 before his conversion, his draw on an in-between pass, frame 11) are made here, as before */
static inline void flush(uint32_t link) {
    if (!flushing())
        return;
    const uint32_t tag = frameTag();
    for (uint32_t k = 0; k < kMax; k++) {
        const uint32_t t = slot(k, tag);
        if (!t)
            break;
        if (t == kKeyX)
            return;
    }
    flush_upto(link, kMax - 1);
}
/* the end of Link's tick30 execute: everything left */
static inline void flush_end(uint32_t link) {
    if (flushing())
        flush_upto(link, kMax - 1);
}
} // namespace lkT60Maba

/* procWait's idle-timer draw: lkT60Maba kKeyW (above; replaces lkT60WaitRnd execute-start flush). */
