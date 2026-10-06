/* Differential verification harness.
 *
 * For each WWHD function that has a candidate source implementation (VERIFY(addr, fn) in
 * wwhd_src), run the recompiled original and the candidate on identical inputs and compare:
 *   - the return value (type from the candidate's signature),
 *   - the net memory effect (every byte whose final value differs from its initial value,
 *     outside the stack below the entry SP),
 *   - the sequence of calls to other guest functions with their argument registers.
 * Callees are not executed (unless a unit links them as `real`): they are recorded and
 * answered by a mock (generated inputs) or with the results and memory effects recorded in
 * the game (recorded inputs, runtime/include/verify_tap.h).
 *
 * usage: verify [-n N] [-seed S] [-rec DIR] [-only ADDR[,ADDR...]] [-list] [-spec FILE] [-image FILE] [-v]
 */
#define GABI_HARNESS 1 /* the harness defines gmem_stack_reserve (gabi.h: weak no-op elsewhere) */
#include <setjmp.h>

#include <algorithm>
#include <cinttypes>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <set>
#include <csignal>
#include <unistd.h>
#include <sys/mman.h>
#include "native_frames.cpp" /* gmem_frame_info: build/verify/frames.tsv (frames_all.py) */
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <dirent.h>

#include "gabi.h"
#include "unit.h"
#include "verify_tap.h"
#include "vm.h"

namespace gabi {
thread_local Cpu* cpu;
static Candidate* g_cands;
Candidate::Candidate(u32 a, const char* n, EntryFn f, RetKind r) : addr(a), name(n), fn(f), ret(r), next(g_cands) { g_cands = this; }
}  // namespace gabi

extern "C" {
int g_ppc_trace;
volatile int g_core_preempt[3];
void ppc_trace_enter(uint32_t) {}
void ppc_preempt(Cpu*) {}
uint64_t ppc_timebase(void) { return 0; }
}

/* ------------------------------------------------------------------ hashing */
static inline uint64_t mix(uint64_t x) {
    x += 0x9E3779B97F4A7C15ull;
    x = (x ^ (x >> 30)) * 0xBF58476D1CE4E5B9ull;
    x = (x ^ (x >> 27)) * 0x94D049BB133111EBull;
    return x ^ (x >> 31);
}
static inline uint64_t hash3(uint64_t a, uint64_t b, uint64_t c) { return mix(a ^ mix(b ^ mix(c))); }

static std::vector<uint32_t>* g_dict;
static float rand_float(uint64_t h) {
    if (g_dict && !g_dict->empty() && (h >> 50) % 4 == 0) {
        uint32_t v = (*g_dict)[(h >> 20) % g_dict->size()];
        switch ((h >> 40) % 4) { /* the constant or a neighbour */
        case 1: v += 1; break;
        case 2: v -= 1; break;
        default: break;
        }
        return u32_as_f32(v);
    }
    switch (h % 16) {
    case 0: return 0.0f;
    case 1: return 1.0f;
    case 2: return -1.0f;
    case 3: return 0.5f;
    case 4: return (float)((int)((h >> 8) % 64) - 32);  /* small integers */
    case 5: return (float)((h >> 8) % 100000) / 100.0f; /* 0..1000 */
    default: {
        double u = (double)((h >> 11) & 0xFFFFFFFull) / (double)0x10000000;
        double scale = (h >> 40) % 3 == 0 ? 10.0 : ((h >> 40) % 3 == 1 ? 1000.0 : 100000.0);
        return (float)((u * 2.0 - 1.0) * scale);
    }
    }
}

/* a plausible 32-bit word for memory nobody described: zero, small ints, small per-byte values,
 * floats, pointers into the random heap, s16 pairs, -1 patterns, random bits */
static uint32_t typed_word(uint64_t h) {
    uint32_t k = h % 100, x = (uint32_t)(h >> 20);
    if (k < 14) return 0;
    if (k < 26) return x % 8;
    if (k < 36) return ((x & 3) << 24) | (((x >> 2) & 3) << 16) | (((x >> 4) & 3) << 8) | ((x >> 6) & 3);
    if (k < 56) return f32_as_u32(rand_float(h >> 7)); /* includes dictionary constants */
    if (k < 72) return 0x30000000u + (x & 0x00FFFFF0u);
    if (k < 82) return ((x & 0xFFFF) << 16) | ((x >> 13) & 0xFFFF);
    if (k < 88) return (k & 1) ? 0xFFFFFFFFu : 0x0000FFFFu;
    return (uint32_t)(h >> 32) ^ x;
}

/* ------------------------------------------------------------------ memory model */
/* A guest page (compact, 2026-10-06; was 25 KB per touched page): the bytes are held in 64-byte chunks that
 * exist only once a byte of them is touched, and a chunk's per-byte clobber epochs only once one of its
 * bytes can actually be clobbered (generated input with clobbering on, clobberable address; everywhere else
 * the epoch never influences a value). Pages, chunks and epoch arrays come from an arena that is unmapped
 * after every input, so a pathological input does not leave its high-water mark behind. */
struct Chunk {
    uint8_t d[64];
    uint8_t init[64];
    uint32_t* ep; /* nullptr: every byte current for epoch 0 (32-bit: >65535 mocked calls must not wrap) */
};
struct Page {
    uint64_t have[64];
    uint64_t wr[64];
    Chunk* ch[64];
};
struct SnapPage {
    uint8_t d[4096];
    uint64_t has[64];
};
struct Region {
    uint32_t addr, size;
    std::vector<uint8_t> data;
};

enum Mode { MODE_GEN, MODE_REC };

struct CallEv {
    uint32_t target;
    int kind;
    uint32_t r[11];
    double f[9];
    int nint, nflt;                 /* candidate: declared arguments */
    uint32_t sp = 0;                /* r1 the callee starts with (compared under `footprint`) */
    uint32_t notarg = 0;            /* original: registers that cannot be arguments of an unlisted target (target/table register, stale after a mock) */
    /* the rarely needed parts live in a separately allocated block, only when present (2026-10-06: a call
     * record was ~1.2 KB of mostly empty vectors; pathological inputs record millions of calls) */
    struct Extra {
        std::vector<uint32_t> stk;      /* stack argument words (sp+8...) */
        std::vector<uint8_t> stkref[4]; /* pointee bytes when a stack argument points into the stack */
        std::vector<uint8_t> sref[11];  /* pointee bytes of stack pointer arguments */
        std::vector<uint8_t> sflt[11];  /* per byte: covered by a float store (NaN payload tolerance) */
        std::vector<std::pair<uint32_t, std::vector<uint8_t>>> observed; /* selected globals at call entry */
        /* `framexact FUNC RET`: the call as a save state at the frame boundary sees it */
        bool fx = false;
        uint32_t lr = 0;
        uint32_t nv[18] = {};        /* r14..r31 */
        uint64_t fnv[18][2] = {};    /* f14..f31 ps0/ps1 bits */
        std::vector<uint8_t> frame;  /* [entry sp - frame size, entry sp + 8) */
    };
    std::unique_ptr<Extra> extra_;
    Extra& ex() { if (!extra_) extra_.reset(new Extra()); return *extra_; }
    const Extra& ex() const { static const Extra none; return extra_ ? *extra_ : none; }
};

struct Patch { uint32_t ea; uint8_t v; };
struct RecCall {
    uint32_t target;
    TapRegs at, after;
    std::vector<Patch> patches;
};

static struct VM {
    std::unordered_map<uint32_t, Page*> pages;
    std::vector<Page*> touched;
    std::unordered_map<uint32_t, SnapPage*> snap;
    std::vector<Region> image;
    uint64_t seed = 1;
    Mode mode = MODE_GEN;
    uint32_t entry_sp = 0, stack_lo = 0;
    /* run state */
    std::vector<CallEv> calls;
    std::unordered_map<uint32_t, uint32_t> per_target;
    std::set<uint32_t> dwords; /* complete explicit f64 store starts */
    std::set<uint32_t> fwords; /* words whose last store was an explicit float store (all depths) or inherited GHS copy */
    uint32_t fcopy[8];          /* NaN values recently loaded (32-bit) from float-stored words: an integer */
    unsigned nfcopy;            /* copy of such a word keeps its float tag (GHS struct copies) */
    int depth = 0;
    bool is_candidate = false;
    uint64_t budget = 0;
    jmp_buf abort_jmp;
    bool can_abort = false;
    std::string abort_reason;
    uint32_t stray = 0, first_stray = 0;
    /* generated mode: each mocked call starts a new epoch in which it may have changed any
     * heap/global word (not the image, not the scratch stack): 1 in kClobber per word */
    uint32_t epoch = 0;
    bool clobber = true;      /* -noclobber turns it off */
    bool clobber_now = true;  /* per input: on for half of the generated inputs (the other half
                               * keeps values across calls, so deep paths stay reachable) */
    /* constants the original reads from .rodata (collected by a pre-run): generated floats
     * come from this dictionary part of the time, so comparisons hit their boundaries */
    std::vector<uint32_t> dict;
    bool collect = false;
    std::vector<uint32_t>* cov = nullptr;
    uint32_t cov_func = 0;
    int traps = 0;
    /* recorded input */
    const std::vector<RecCall>* rec_calls = nullptr; /* the input's recorded calls (not copied) */
    std::vector<CallEv>* orig_calls = nullptr; /* for translating stack patches to the candidate */
    std::map<uint32_t, uint32_t> locals;       /* candidate: live gabi::Local slots (address -> 16-byte-aligned slot size) */
    std::map<uint32_t, uint32_t> local_objsz;  /* candidate: the same Locals' sizeof(T) */
    std::vector<std::pair<uint32_t, uint32_t>> reserved; /* candidate: native frames (gmem_stack_reserve) */
    uint32_t post_r[13]; bool have_post = false; /* volatile registers as a generated mock left them */
    /* stack guard (candidate side): harness-internal stack writes in progress; the candidate's SP
     * before the outgoing frame of the guest call that is running (0: none) */
    int stack_internal = 0;
    uint32_t call_top = 0;
    std::unordered_set<uint32_t> orig_stack; /* guarded-range bytes the original wrote for this input */
    std::vector<std::pair<uint32_t, int>> orig_seq; /* the original's top-level calls for this input (target, kind) */
} V;
/* constructor sizes (tools/verify/ctor_sizes.tsv, from sizes.py): a candidate gabi::Local passed as
 * `this` to a constructor must hold the whole object, or the constructor writes past it into the
 * caller's frame (game test: daSail_packet_c::draw, 0xF0-byte Local for a 0x11C-byte object) */
static std::unordered_map<uint32_t, uint32_t> g_ctor_size;
/* mockfield TARGET rN+OFF TYPE v1,v2,... | LO HI : a generated mock of TARGET writes a chosen value into the
 * object its argument rN points to (both sides alike, per call), so coherent output fixtures can be built
 * without making the callee real. TYPE u8/s8/u16/s16/u32/s32/f32. */
struct MockField { int reg; uint32_t off; int size; bool is_float; std::vector<double> pick; double lo, hi; };
static std::unordered_map<uint32_t, std::vector<MockField>> g_mockfield;
/* live-in argument registers of every function (build/verify/livein.tsv, livein_all.py): at a call to a target
 * the unit does not list (indirect call), compare what the resolved callee reads, not only what the candidate
 * passes (game test 2026-10-04: an isKindOf vcall without its r4 argument went unnoticed) */
static std::unordered_map<uint32_t, std::pair<uint32_t, uint32_t>> g_livein;
static std::set<uint32_t> g_nolivein; /* unit `nolivein FUNC`: documented false positives of the live-in check */

static const Region* image_region(uint32_t ea) {
    for (const Region& r : V.image)
        if (ea - r.addr < r.size) return &r;
    return nullptr;
}

/* initial value of a byte; *known: from snapshot / image / stack (not random) */
/* unit `stackfill`: the scratch stack below the entry SP starts with deterministic random bytes
 * instead of zeros (catches code that relies on a slot the original initialises) */
static bool g_stackfill = false;
static bool g_stackfill_periodic = false;

static uint8_t initial_byte(uint32_t ea, bool* known) {
    auto it = V.snap.find(ea >> 12);
    if (it != V.snap.end()) {
        uint32_t o = ea & 0xFFF;
        if (it->second->has[o >> 6] >> (o & 63) & 1) { *known = true; return it->second->d[o]; }
    }
    if (const Region* r = image_region(ea)) {
        *known = true;
        uint32_t o = ea - r->addr;
        return o < r->data.size() ? r->data[o] : 0;
    }
    if (ea - V.stack_lo < V.entry_sp - V.stack_lo) {
        *known = true;
        /* `stackfill periodic`: the byte depends only on ea & 3, so objects the two sides place at
         * different (equally aligned) stack addresses start with the same bytes */
        return g_stackfill ? (uint8_t)(hash3(V.seed, g_stackfill_periodic ? (ea & 3u) : ea, 0x57AC) >> 11) : 0;
    }
    *known = false;
    uint32_t w = typed_word(hash3(V.seed, ea & ~3u, 0x5EED));
    return (uint8_t)(w >> (24 - 8 * (ea & 3)));
}

/* bump arena for pages, chunks and epoch arrays: zero-filled chunks from mmap. After every input the first
 * chunk is kept (its used part zeroed again, so normal inputs never map/unmap) and every further chunk is
 * unmapped, so a pathological input does not leave its high-water mark behind. */
static std::vector<std::pair<void*, size_t>> g_arena_chunks;
static uint8_t* g_arena_cur = nullptr;
static size_t g_arena_left = 0;
static void* arena_zalloc(size_t n) {
    n = (n + 63) & ~(size_t)63;
    if (n > g_arena_left) {
        size_t sz = std::max<size_t>(n, 4u << 20);
        void* m = mmap(nullptr, sz, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
        if (m == MAP_FAILED) { fprintf(stderr, "harness: out of memory (arena)\n"); exit(3); }
        g_arena_chunks.push_back({m, sz});
        g_arena_cur = (uint8_t*)m;
        g_arena_left = sz;
    }
    void* r = g_arena_cur;
    g_arena_cur += n;
    g_arena_left -= n;
    return r;
}
static void arena_release() {
    if (g_arena_chunks.empty()) return;
    auto first = g_arena_chunks[0];
    size_t used = g_arena_chunks.size() == 1 ? first.second - g_arena_left : first.second;
    for (size_t i = 1; i < g_arena_chunks.size(); i++) munmap(g_arena_chunks[i].first, g_arena_chunks[i].second);
    g_arena_chunks.resize(1);
    memset(first.first, 0, used);
    g_arena_cur = (uint8_t*)first.first;
    g_arena_left = first.second;
}

static Page* page(uint32_t pn) {
    auto it = V.pages.find(pn);
    if (it != V.pages.end()) return it->second;
    Page* p = (Page*)arena_zalloc(sizeof(Page));
    V.pages[pn] = p;
    V.touched.push_back(p);
    return p;
}

static const uint32_t kClobber = 4;

/* unit `stable ADDR SIZE`: steered data that no callee writes (e.g. stage path data): never clobbered */
static std::vector<std::pair<uint32_t, uint32_t>> g_stable;

static bool clobberable(uint32_t ea) {
    if (image_region(ea)) return false;
    for (auto& r : g_stable)
        if (ea - r.first < r.second) return false;
    if (ea - V.stack_lo < V.entry_sp + 8 - V.stack_lo) return false;
    return true;
}

/* Proposal: invalidate typed provenance on any byte write, including nested effects. */
static inline void invalidate_float_overlap(uint32_t ea) {
    for (uint32_t k = ea - 3; k != ea + 1; ++k) V.fwords.erase(k);
    for (uint32_t k = ea - 7; k != ea + 1; ++k) V.dwords.erase(k);
}

static inline void materialize(Page* p, uint32_t ea, bool reading) {
    uint32_t o = ea & 0xFFF;
    if (!(p->have[o >> 6] >> (o & 63) & 1)) {
        bool known;
        uint8_t v = initial_byte(ea, &known);
        if (!known && reading && V.mode == MODE_REC && V.depth == 0) {
            if (!V.stray) V.first_stray = ea;
            V.stray++;
        }
        Chunk*& k = p->ch[o >> 6];
        if (!k) k = (Chunk*)arena_zalloc(sizeof(Chunk));
        k->d[o & 63] = k->init[o & 63] = v;
        if (k->ep) k->ep[o & 63] = 0;
        p->have[o >> 6] |= 1ull << (o & 63);
    }
    Chunk* k = p->ch[o >> 6];
    /* the epoch only matters where a mocked call can clobber the byte; that condition is fixed for the
     * whole run (input mode, clobber switch, stable ranges, stack bounds), so elsewhere it is not kept */
    uint32_t cur = k->ep ? k->ep[o & 63] : 0;
    if (cur != V.epoch && V.clobber && V.clobber_now && V.mode == MODE_GEN && clobberable(ea)) {
        /* did a mocked call since the byte was last current overwrite its word? (latest wins) */
        for (uint32_t e = V.epoch; e > cur; e--) {
            uint64_t h = hash3(V.seed, ea & ~3u, 0xC10B + e);
            if (h % kClobber == 0) {
                uint32_t w = typed_word(h >> 2);
                k->d[o & 63] = (uint8_t)(w >> (24 - 8 * (ea & 3)));
                break;
            }
        }
        if (!k->ep) k->ep = (uint32_t*)arena_zalloc(64 * sizeof(uint32_t));
        k->ep[o & 63] = V.epoch;
    }
}

/* WWHD_MEMSTAT=1: memory diagnostics on stderr (progress every 1M guest accesses, totals per run); off by default */
static bool g_memstat = getenv("WWHD_MEMSTAT") != nullptr;
static inline void tick() {
    if (g_memstat && (V.budget & 0xFFFFF) == 0xFFFFF) /* analysis: progress of one run every 1M accesses */
        fprintf(stderr, "memstat-progress %s seed=%llu budget=%llu pages=%zu snap=%zu calls=%zu epoch=%u depth=%d\n",
                V.is_candidate ? "cand" : "orig", (unsigned long long)V.seed, (unsigned long long)V.budget, V.pages.size(),
                V.snap.size(), V.calls.size(), V.epoch, V.depth);
    if (++V.budget > 20000000ull && V.can_abort) {
        V.abort_reason = "access budget exceeded";
        longjmp(V.abort_jmp, 1);
    }
}

static inline uint8_t rd8(uint32_t ea) {
    Page* p = page(ea >> 12);
    materialize(p, ea, true);
    uint32_t o = ea & 0xFFF;
    return p->ch[o >> 6]->d[o & 63];
}
/* Stack guard (game test 2026-10-05: hd_res_mgr loadFile wrote f+0x193 through an inlined
 * constructor into a 0x190-byte gabi::Local; such writes land in the scratch stack or the caller's
 * back chain / LR save words, which the net-effect comparison excludes). On the candidate side a
 * store into [stack_lo, entry_sp + 8) must fall inside a live gabi::Local slot:
 *   - depth 0 (the candidate itself): from the lowest live Local up to entry SP + 8 (below every Local
 *     are outgoing frames, built by do_call or by hand), except the harness's own slot initialisation;
 *   - depth > 0 (real callees): only at or above the caller's SP before the call (below it are the
 *     callee frames and the linkage/argument area).
 * A byte the original also wrote for the same input is allowed (generated pointers can fall into the
 * stack range). Mocks and recorded effects write with patch8 and are not checked. */
static void stack_guard(uint32_t ea) {
    if (ea - V.stack_lo >= V.entry_sp + 8 - V.stack_lo) return;
    if (!V.is_candidate) { V.orig_stack.insert(ea); return; }
    if (V.stack_internal) return;
    /* the original wrote the same byte (e.g. through an input pointer that happens to lie in the
     * stack range): not a candidate-only overflow */
    if (V.orig_stack.count(ea)) return;
    if (V.depth > 0 && (!V.call_top || ea < V.call_top)) return;
    /* below every gabi::Local: an outgoing frame the candidate builds by hand (stack arguments below a
     * lowered r1, e.g. f_op_msg_mng's J2DPicture::drawTexCoord binding); overflows run upwards */
    if (V.depth == 0 && ea < V.entry_sp && (V.locals.empty() || ea < V.locals.begin()->first)) return;
    for (auto& r : V.reserved)
        if (ea - r.first < r.second) return;
    auto it = V.locals.upper_bound(ea);
    uint32_t base = 0, size = 0;
    if (it != V.locals.begin()) {
        --it;
        if (ea - it->first < it->second) return;
        base = it->first; size = it->second;
    }
    if (!V.can_abort) return;
    char why[200];
    if (size)
        snprintf(why, sizeof why, "store outside any live Local at %08X (%s; nearest Local below: %08X size 0x%X, entry SP %08X)",
                 ea, V.depth ? "by a real callee" : "by the candidate", base, size, V.entry_sp);
    else
        snprintf(why, sizeof why, "store outside any live Local at %08X (%s; no Local below it, entry SP %08X)",
                 ea, V.depth ? "by a real callee" : "by the candidate", V.entry_sp);
    V.abort_reason = why;
    longjmp(V.abort_jmp, 1);
}
static inline void wr8(uint32_t ea, uint8_t v) {
    stack_guard(ea);
    Page* p = page(ea >> 12);
    materialize(p, ea, false);
    uint32_t o = ea & 0xFFF;
    invalidate_float_overlap(ea);
    p->ch[o >> 6]->d[o & 63] = v;
    p->wr[o >> 6] |= 1ull << (o & 63);
}
/* a callee's write (mock / recorded effect): changes memory but is not the function's own write */
static void patch8(uint32_t ea, uint8_t v) {
    Page* p = page(ea >> 12);
    materialize(p, ea, false);
    invalidate_float_overlap(ea);
    uint32_t o = ea & 0xFFF;
    p->ch[o >> 6]->d[o & 63] = v;
}
static uint8_t peek8(uint32_t ea) { /* no stray accounting */
    int d = V.depth;
    V.depth = 1;
    uint8_t v = rd8(ea);
    V.depth = d;
    return v;
}

extern "C" {
uint8_t vm_ld8(uint32_t ea) { tick(); return rd8(ea); }
uint16_t vm_ld16(uint32_t ea) { tick(); return (uint16_t)(rd8(ea) << 8 | rd8(ea + 1)); }
static int g_trace; /* -trace SEED: print the memory accesses and calls of that input */
uint32_t vm_ld32(uint32_t ea) {
    tick();
    uint32_t v = (uint32_t)rd8(ea) << 24 | (uint32_t)rd8(ea + 1) << 16 | (uint32_t)rd8(ea + 2) << 8 | rd8(ea + 3);
    if (g_trace && V.depth == 0) printf("        ld32 %08X -> %08X (epoch %u)\n", ea, v, V.epoch);
    if (V.collect && V.depth == 0 && ea - 0x10000000u < 0x0018C0B0u && (v & 0x7F800000u) != 0x7F800000u && V.dict.size() < 64 &&
        std::find(V.dict.begin(), V.dict.end(), v) == V.dict.end())
        V.dict.push_back(v); /* a .rodata word (float constant) */
    if (V.depth == 0 && (v & 0x7F800000u) == 0x7F800000u && (v & 0x007FFFFFu) && V.fwords.count(ea))
        V.fcopy[V.nfcopy++ % 8] = v;
    return v;
}
uint64_t vm_ld64(uint32_t ea) { return (uint64_t)vm_ld32(ea) << 32 | vm_ld32(ea + 4); }
void vm_st8(uint32_t ea, uint8_t v) { tick(); wr8(ea, v); }
void vm_st16(uint32_t ea, uint16_t v) { tick(); wr8(ea, v >> 8); wr8(ea + 1, (uint8_t)v); }
void vm_st32(uint32_t ea, uint32_t v) {
    tick();
    bool copied_float = false;
    if (V.depth == 0) {
        for (unsigned i = 0; i < 8 && i < V.nfcopy; i++)
            if (V.fcopy[i] == v) { copied_float = true; break; } /* inherited GHS value-copy rule */
    }
    if (g_trace && V.depth == 0) printf("        st32 %08X <- %08X (epoch %u)\n", ea, v, V.epoch);
    for (int i = 0; i < 4; i++) wr8(ea + i, (uint8_t)(v >> (24 - 8 * i)));
    if (copied_float) V.fwords.insert(ea);
}
void vm_st64(uint32_t ea, uint64_t v) { vm_st32(ea, (uint32_t)(v >> 32)); vm_st32(ea + 4, (uint32_t)v); }

uint8_t gmem_ld8(uint32_t ea) { return vm_ld8(ea); }
uint16_t gmem_ld16(uint32_t ea) { return vm_ld16(ea); }
uint32_t gmem_ld32(uint32_t ea) { return vm_ld32(ea); }
uint64_t gmem_ld64(uint32_t ea) { return vm_ld64(ea); }
void gmem_st8(uint32_t ea, uint8_t v) { vm_st8(ea, v); }
void gmem_st16(uint32_t ea, uint16_t v) { vm_st16(ea, v); }
void gmem_st32(uint32_t ea, uint32_t v) { vm_st32(ea, v); }
void gmem_st64(uint32_t ea, uint64_t v) { vm_st64(ea, v); }
void vm_stf32w(uint32_t ea, uint32_t v) {
    vm_st32(ea, v);
    V.fwords.insert(ea); /* explicit float stores also inside real callees */
}
void gmem_stf32(uint32_t ea, uint32_t v) { vm_stf32w(ea, v); }
void vm_stf64w(uint32_t ea, uint64_t v) {
    vm_st64(ea,v);
    V.dwords.insert(ea);
}
void gmem_stf64(uint32_t ea, uint64_t v) { vm_stf64w(ea,v); }
/* gabi::Local: a fresh stack slot gets the scratch stack's initial contents (zero, or the
 * `stackfill` bytes), undoing earlier host-side uses of the same slot */
void gmem_stack_fresh(uint32_t ea, uint32_t n, uint32_t object_size) {
    if (V.is_candidate && V.depth == 0) { /* a new Local: forget slots it overlaps (stack reuse), then record it */
        auto it = V.locals.lower_bound(ea);
        if (it != V.locals.begin()) {
            auto p = std::prev(it);
            if (p->first + p->second > ea) it = p;
        }
        while (it != V.locals.end() && it->first < ea + n) { V.local_objsz.erase(it->first); it = V.locals.erase(it); }
        V.locals[ea] = n;
        V.local_objsz[ea] = object_size ? object_size : n;
    }
    V.stack_internal++;
    for (uint32_t i = 0; i < n; i++) {
        bool known;
        wr8(ea + i, initial_byte(ea + i, &known));
    }
    V.stack_internal--;
}
/* gabi::NativeFrame: a reserved, uninitialised stack area (the original's frame layout): registered
 * for the stack guard like a Local, its bytes left as they are */
void gmem_stack_reserve(uint32_t ea, uint32_t n) {
    /* a native frame (gabi::NativeFrame / the automatic frame): candidate stores inside are allowed;
     * gabi::Locals placed inside it stay separate Locals */
    if (V.is_candidate && V.depth == 0) V.reserved.emplace_back(ea, n);
}
/* automatic native frames: a gabi::Local that does not fit into the original's frame (unit
 * survey: one stderr note per function and size, and a line in build/verify/frame_fallbacks.tsv) */
void gmem_frame_fallback(uint32_t func, uint32_t size, uint32_t used, uint32_t space, uint32_t frame) {
    static std::set<std::pair<uint32_t, uint32_t>> seen;
    if (!V.is_candidate || !seen.insert({func, size}).second) return;
    fprintf(stderr, "[nframe] %08X: Local of 0x%X bytes does not fit the original's frame (0x%X: object space 0x%X, 0x%X used)\n",
            func, size, frame, space, used);
    if (FILE* f = fopen("build/verify/frame_fallbacks.tsv", "a")) {
        fprintf(f, "%08X\t%u\t%u\t%u\t%u\n", func, size, used, space, frame);
        fclose(f);
    }
}
void gmem_stack_args(int on) { V.stack_internal += on ? 1 : -1; }
void gmem_call_top(uint32_t top) { if (V.depth == 0) V.call_top = top; }

void vm_cov(uint32_t, uint32_t block) {
    if (V.cov && V.depth == 0 && block < V.cov->size()) (*V.cov)[block]++;
}
void vm_trap(Cpu*, uint32_t, uint32_t) { V.traps++; }
}

static void reset_memory() {
    for (Page* p : V.touched) {
        memset(p->have, 0, sizeof p->have);
        memset(p->wr, 0, sizeof p->wr);
    }
}

static void clear_all() {
    /* pages live in the arena: release it (and the per-input containers' capacity) after every input */
    std::vector<Page*>().swap(V.touched);
    std::unordered_map<uint32_t, Page*>().swap(V.pages);
    arena_release();
    std::vector<CallEv>().swap(V.calls);
    for (auto& kv : V.snap) free(kv.second);
    V.snap.clear();
}

static void snap_set(uint32_t ea, uint8_t v) {
    SnapPage*& s = V.snap[ea >> 12];
    if (!s) s = (SnapPage*)calloc(1, sizeof(SnapPage));
    uint32_t o = ea & 0xFFF;
    s->d[o] = v;
    s->has[o >> 6] |= 1ull << (o & 63);
}
static bool snap_has(uint32_t ea) {
    auto it = V.snap.find(ea >> 12);
    if (it == V.snap.end()) return false;
    uint32_t o = ea & 0xFFF;
    return it->second->has[o >> 6] >> (o & 63) & 1;
}

/* unit `footprint FUNC SIZE`: also compare the function's own stack footprint, the SIZE bytes below
 * the entry SP plus the back chain/LR save words above it (game test 2026-10-05: a candidate that
 * leaves different stale bytes in its frame, or runs its callees at a different SP, changes what a
 * later function copies out of an uninitialised stack object; fsCreate -> initdata_to_card -> the
 * empty slots of cking.sav). Use with `stackfill`, so bytes neither side writes compare equal. */
static std::unordered_map<uint32_t, uint32_t> g_footprint;
static uint32_t g_cur_func_fp = 0; /* g_cur_func, visible here */
/* -calleesp: every function's callees must start at the original's sp; -footprint: every function's
 * frame (size from build/verify/frames.tsv) is compared as with `footprint` (automatic native
 * frames, 2026-10-05: survey of how far frames are exact) */
static bool g_calleesp_all = false, g_footprint_all = false;
static uint32_t footprint_of(uint32_t func) {
    if (auto f = g_footprint.find(func); f != g_footprint.end()) return f->second;
    if (g_footprint_all) { const GabiFrameInfo* fi = gmem_frame_info(func); return fi ? fi->size : 0; }
    return UINT32_MAX;
}

/* net effect of a run: bytes whose value changed, outside the scratch stack */
static std::map<uint32_t, uint8_t> net_writes() {
    uint32_t fp_lo = V.entry_sp + 8; /* no footprint: the whole scratch stack incl. back chain/LR save is excluded */
    if (uint32_t fs = footprint_of(g_cur_func_fp); fs != UINT32_MAX) fp_lo = V.entry_sp - fs;
    std::map<uint32_t, uint8_t> out;
    for (auto& kv : V.pages) {
        Page* p = kv.second;
        for (int w = 0; w < 64; w++) {
            uint64_t m = p->wr[w];
            while (m) {
                int b = __builtin_ctzll(m);
                m &= m - 1;
                uint32_t o = w * 64 + b, ea = (kv.first << 12) | o;
                if (ea - V.stack_lo < V.entry_sp + 8 - V.stack_lo && ea - V.stack_lo < fp_lo - V.stack_lo) continue; /* own frames + back chain/LR save */
                materialize(p, ea, false); /* apply calls made after the write (clobber) */
                /* recorded inputs: a written byte whose initial value was never read is unknown,
                 * so it counts as written whatever its value */
                bool unknown = V.mode == MODE_REC && !snap_has(ea) && !image_region(ea);
                const Chunk* k = p->ch[o >> 6];
                if (k->d[o & 63] != k->init[o & 63] || unknown) out[ea] = k->d[o & 63];
            }
        }
    }
    return out;
}

/* ------------------------------------------------------------------ calls */
static const CalleeInfo* callee_info(uint32_t t) {
    static std::unordered_map<uint32_t, const CalleeInfo*> idx;
    if (idx.empty())
        for (unsigned i = 0; i < unit_ncallees; i++) idx[unit_callees[i].addr] = &unit_callees[i];
    auto it = idx.find(t);
    return it == idx.end() ? nullptr : it->second;
}

static bool in_stack(uint32_t v) { return v - V.stack_lo < V.entry_sp - V.stack_lo; }

static uint32_t typed_ret(uint64_t h) {
    uint32_t k = h % 100;
    if (k < 28) return 0;
    if (k < 54) return 1;
    if (k < 62) return 0xFFFFFFFFu; /* -1 / 0xFF / 0xFFFF sentinels after shaping */
    if (k < 75) return (uint32_t)(h >> 33) % 16;
    if (k < 90) return 0x30000000u + ((uint32_t)(h >> 20) & 0x00FFFFF0u);
    return (uint32_t)(h >> 32);
}

static std::unordered_map<uint32_t, std::pair<int64_t, int64_t>> g_ret_specs; /* unit `ret` lines: callee -> result range */
static std::unordered_map<uint32_t, std::pair<double, double>> g_fret_specs;
static std::unordered_map<uint32_t, std::vector<int64_t>> g_ret_picks; /* `ret ADDR v1,v2,...` */ /* unit `fret` lines: callee -> f1 range */

/* a result of the shape the real callee returns (GHS callers do not re-extend small results) */
static uint32_t shape_ret(const CalleeInfo* ci, uint32_t v) {
    if (!ci) return v;
    switch (ci->retkind) {
    case 1: if (ci->retbits < 32) { if (ci->retbits == 1) return v & 1; v &= (1u << ci->retbits) - 1; } return v;
    case 2: if (ci->retbits < 32) { uint32_t s = 32 - ci->retbits; v = (uint32_t)((int32_t)(v << s) >> s); } return v;
    case 3: return ci->retconst;
    default: return v;
    }
}

static std::unordered_map<uint32_t, std::vector<std::pair<uint32_t, uint32_t>>> g_observe_specs;
/* `framexact FUNC RET` (cross-build save states, 2026-10-06): FUNC's call that returns to RET (the
 * original's LR at the call; also an indirect call) is on the guest stack when a save state is
 * taken at the frame boundary. At that call compare LR (the
 * return address the callee saves), r14..r31, f14..f31 (ps0 and ps1) and FUNC's whole frame
 * [entry sp - size, entry sp + 8) incl. its back chain, saved-register slots and LR save word;
 * at FUNC's exit compare r14..r31 and f14..f31 (the epilogue's reloads). Generated inputs give
 * r14..r31 and f14..f31 distinct values. Frame size from frames.tsv. Use with `stackfill`. */
static std::set<std::pair<uint32_t, uint32_t>> g_framexact;
static std::set<uint32_t> g_framexact_funcs;
/* `final FUNC ADDR SIZE`: bytes compared at the tested function's exit (postcondition), whatever
 * wrote them; catches restores of values a mocked callee may have changed */
static std::unordered_map<uint32_t, std::vector<std::pair<uint32_t, uint32_t>>> g_final_specs;
static uint32_t g_cur_func = 0;
/* `alloc TARGET rSIZE BASE [ALIGN]`: the mock of an allocator returns distinct, aligned, non-overlapping
 * blocks from BASE in call order (same sequence on both sides) */
struct AllocSpec { unsigned sizereg; uint32_t base, align, fixed; }; /* sizereg 0: fixed size (allocfixed) */
static std::unordered_map<uint32_t, AllocSpec> g_alloc_specs;
static std::unordered_map<uint32_t, uint32_t> g_alloc_cursor;
/* `nestedret TARGET VALUE`: inside a real callee, a nested call to TARGET returns VALUE in r3
 * instead of a random answer */
static std::unordered_map<uint32_t, uint32_t> g_nestedret;
/* `ret!` / `fret!`: steering that always applies (no 1-in-8 escape), also for nested calls inside
 * real callees */
static std::set<uint32_t> g_ret_strict, g_fret_strict;
/* `mockfetchsize TARGET` / `mockfetchinit TARGET`: GX2CalcFetchShaderSizeEx / GX2InitFetchShaderEx
 * imports. Size: the runtime HLE formula from the stream count
 * in r3. Init: writes 32 descriptor bytes at r3 and 16+n*16 program bytes at r4 (n = r5 <= 16),
 * derived deterministically from n and the n*32 input stream bytes at r6, so different inputs give
 * different outputs; nothing else is written. All call/argument comparisons stay. */
static std::set<uint32_t> g_fetch_size, g_fetch_init;

// Opt-in semantic effects for generated leaf mocks. Recorded effects are unchanged.
struct MoveSpec { unsigned dst, src, size; };
struct Utf16Spec { unsigned out, maxchars; };
static std::unordered_map<uint32_t, MoveSpec> g_move_specs;
static std::unordered_map<uint32_t, Utf16Spec> g_utf16_specs;
static uint64_t g_move_calls, g_move_bytes, g_utf16_fail, g_utf16_length[4097];


static void apply_fetch_effects(Cpu* c, uint32_t target, const uint32_t* argr) {
    if (g_fetch_size.count(target)) {
        uint32_t n = argr[3];
        uint32_t cf = ((((n + 15) / 16) + 1) * 8 + 15) & ~15u;
        c->r[3] = std::max(cf + n * 16, 16 + n * 16);
    }
    if (g_fetch_init.count(target)) {
        uint32_t n = argr[5];
        if (n > 16) { V.abort_reason = "fetch shader stream count > 16"; longjmp(V.abort_jmp, 1); }
        uint64_t acc = hash3(n, 0xFE7C, 0);
        V.depth++;
        for (uint32_t j = 0; j < n * 32; j++) acc = hash3(acc, peek8(argr[6] + j), j);
        for (uint32_t j = 0; j < 32; j++) patch8(argr[3] + j, (uint8_t)(hash3(acc, j, 1) >> 17));
        for (uint32_t j = 0; j < 16 + n * 16; j++) patch8(argr[4] + j, (uint8_t)(hash3(acc, j, 2) >> 17));
        V.depth--;
    }
}
static std::string callee_name(uint32_t t);
static void do_call(Cpu* c, uint32_t target, int kind, int nint, int nflt) {
    const CalleeInfo* ci = callee_info(target);
    /* a callee with a known pointee size (unit/global fact rN=SIZE or outSIZE): the candidate's gabi::Local passed
     * there must hold SIZE bytes, else the real callee reads/writes past it (game test: initTextureScroll 0x1C vs 0x40,
     * setSight 0x18 vs 0x28, dProcTool 12 vs 0x14) */
    if (V.is_candidate && V.depth == 0 && ci)
        for (int i = 3; i <= 10; i++) {
            uint32_t need = ci->ptrsz[i];
            bool from_ext = false;
            if (need == 255 || need == 0xFFFE) need = 0;
            /* static extent (extent_lint.py) of the code at the target: direct calls only (an indirect call's
             * resolved target in generated mode is whatever the random vtable word points to) */
            /* ... and only where the original's call at the same position is the same direct call (a
             * candidate may write a virtual call as gabi::call to a loaded address) */
            size_t k = V.calls.size();
            bool same_direct = kind != VM_CALL_INDIRECT && k < V.orig_seq.size() && V.orig_seq[k].first == target &&
                               V.orig_seq[k].second != VM_CALL_INDIRECT;
            if (same_direct && ci->ext[i] > need) { need = ci->ext[i]; from_ext = true; }
            if (!need || !in_stack(c->r[i])) continue;
            auto it = V.locals.upper_bound(c->r[i]);
            if (it == V.locals.begin()) continue;
            --it;
            uint32_t end = it->first + it->second;
            if (c->r[i] < end && c->r[i] + need > end) {
                static char why[200];
                snprintf(why, sizeof why, "undersized stack object: callee %08X uses 0x%X bytes via r%d at %08X%s, the gabi::Local holds 0x%X",
                         target, need, i, c->r[i], from_ext ? " (static extent of its code)" : "", end - c->r[i]);
                V.abort_reason = why;
                longjmp(V.abort_jmp, 1);
            }
        }
    if (V.is_candidate && V.depth == 0 && !g_ctor_size.empty() && in_stack(c->r[3])) {
        if (auto cs = g_ctor_size.find(target); cs != g_ctor_size.end()) {
            auto it = V.locals.upper_bound(c->r[3]);
            if (it != V.locals.begin()) {
                --it;
                /* a constructor builds exactly sizeof(class) bytes: compare with the Local's sizeof(T),
                 * not its 16-byte-aligned slot (a 0x118-byte Local for a 0x11C-byte DrawState passed
                 * the slot check only because of the alignment padding) */
                auto os = V.local_objsz.find(it->first);
                uint32_t end = it->first + (os != V.local_objsz.end() ? os->second : it->second);
                if (c->r[3] < end && c->r[3] + cs->second > end) {
                    static char why[160];
                    snprintf(why, sizeof why, "undersized stack object: constructor %08X needs 0x%X bytes at %08X, the gabi::Local holds 0x%X",
                             target, cs->second, c->r[3], end - c->r[3]);
                    V.abort_reason = why;
                    longjmp(V.abort_jmp, 1);
                }
            }
        }
    }
    if (V.depth > 0) { /* inside a real callee: answer silently */
        if (ci && ci->real) { ci->real(c); return; } /* a callee the unit lists as `real` (pure helper) also runs for real when nested */
        if (auto nr = g_nestedret.find(target); nr != g_nestedret.end()) { c->r[3] = nr->second; return; }
        if (g_fetch_size.count(target) || g_fetch_init.count(target)) { /* declared fetch-shader imports also when nested */
            uint32_t argr[11];
            for (int i = 3; i <= 10; i++) argr[i] = c->r[i];
            apply_fetch_effects(c, target, argr);
            return;
        }
        if (auto mv = g_move_specs.find(target); mv != g_move_specs.end()) { /* declared mockmove also when nested */
            uint32_t size = c->r[mv->second.size], dst = c->r[mv->second.dst], src = c->r[mv->second.src];
            if (size > 20000000u) { V.abort_reason = "memmove exceeds guest operation budget"; longjmp(V.abort_jmp, 1); }
            std::vector<uint8_t> bytes(size);
            for (uint32_t j = 0; j < size; j++) { tick(); bytes[j] = peek8(src + j); } /* capture first: overlap-safe */
            for (uint32_t j = 0; j < size; j++) { tick(); patch8(dst + j, bytes[j]); }
            c->r[3] = dst;
            return;
        }
        uint64_t h = hash3(V.seed, target, 0xABCDEF + V.per_target[target]++);
        if (!ci || ci->defr >> 3 & 1) c->r[3] = shape_ret(ci, typed_ret(h));
        if (!ci || ci->deff >> 1 & 1) c->f[1].ps0 = c->f[1].ps1 = rand_float(h >> 3);
        if (g_ret_strict.count(target)) {
            if (auto rp = g_ret_picks.find(target); rp != g_ret_picks.end()) c->r[3] = (uint32_t)rp->second[(h >> 13) % rp->second.size()];
            else if (auto rs = g_ret_specs.find(target); rs != g_ret_specs.end())
                c->r[3] = (uint32_t)(rs->second.first + (int64_t)((h >> 13) % (uint64_t)(rs->second.second - rs->second.first + 1)));
        }
        if (auto fs = g_fret_specs.find(target); fs != g_fret_specs.end() && g_fret_strict.count(target)) {
            uint64_t k2 = (h >> 17) % 1000003;
            c->f[1].ps0 = c->f[1].ps1 = (double)(float)(fs->second.first + (double)k2 / 1000003.0 * (fs->second.second - fs->second.first));
        }
        return;
    }
    size_t k = V.calls.size();
    if (g_trace) printf("        call %s\n", callee_name(target).c_str());
    CallEv ev;
    ev.target = target;
    ev.kind = kind;
    memcpy(ev.r, c->r, sizeof ev.r);
    for (int i = 0; i < 9; i++) ev.f[i] = c->f[i].ps0;
    ev.nint = nint;
    ev.nflt = nflt;
    if ((!ci || kind == VM_CALL_INDIRECT) && !V.is_candidate && V.depth == 0) /* also a bctrl that resolves to a listed callee */
        for (int i = 4; i <= 10; i++) {
            uint32_t v = c->r[i];
            /* the register holding the call target (mtctr source) or the table it was loaded from (vtable) */
            if (v == target) { ev.notarg |= 1u << i; continue; }
            if (true) /* any value: a vtable register may be unaligned or low in generated inputs */
                for (uint32_t off = 0; off < 0x400; off += 4)
                    if (((uint32_t)peek8(v + off) << 24 | (uint32_t)peek8(v + off + 1) << 16 | (uint32_t)peek8(v + off + 2) << 8 |
                         peek8(v + off + 3)) == target) { ev.notarg |= 1u << i; break; }
            /* a volatile register still holding what the previous mocked call left there: dead per the ABI */
            if (V.have_post && v == V.post_r[i]) ev.notarg |= 1u << i;
        }
    for (int i = 3; i <= 10; i++)
        if (in_stack(c->r[i])) {
            uint32_t n = ci && ci->ptrsz[i] ? ci->ptrsz[i] : 4;
            if (n == 0xFFFE) n = 4; /* explicit `rN=0` fact: captured like an unknown pointer; compare() accepts equal raw values first (scalar) */
            if (n == 255 || (ci && (ci->outonly >> i & 1))) { ev.ex().sref[i].push_back(0); continue; } /* constructor storage / output-only: not an input */
            CallEv::Extra& E = ev.ex();
            for (uint32_t j = 0; j < n; j++) E.sref[i].push_back(peek8(c->r[i] + j));
            E.sflt[i].assign(n, 0);
            for (uint32_t f : V.fwords) /* float-stored words overlapping the object */
                for (uint32_t k = 0; k < 4; k++)
                    if (f + k - c->r[i] < n) E.sflt[i][f + k - c->r[i]] = 1;
            for (uint32_t d : V.dwords)
                if (d-c->r[i]<n && n-(d-c->r[i])>=8) E.sflt[i][d-c->r[i]]=2;
        }
    {
        int ns = ci ? ci->nstack : 0;
        if (nint > 8) ns = std::max(ns, nint - 8);
        for (int j = 0; j < ns && j < 4; j++) {
            uint32_t w = 0;
            for (int b = 0; b < 4; b++) w = w << 8 | peek8(c->r[1] + 8 + 4 * j + b);
            ev.ex().stk.push_back(w);
            if (in_stack(w))
                for (int b = 0; b < 4; b++) ev.ex().stkref[j].push_back(peek8(w + b));
        }
    }
    if (auto obs = g_observe_specs.find(target); obs != g_observe_specs.end()) {
        for (auto [addr, size] : obs->second) {
            std::vector<uint8_t> bytes;
            bytes.reserve(size);
            for (uint32_t j = 0; j < size; j++) bytes.push_back(peek8(addr + j));
            ev.ex().observed.emplace_back(addr, std::move(bytes));
        }
    }
    ev.sp = c->r[1];
    if (g_framexact_funcs.count(g_cur_func)) {
        CallEv::Extra& E = ev.ex();
        E.fx = true;
        E.lr = c->lr;
        for (int i = 14; i <= 31; i++) {
            E.nv[i - 14] = c->r[i];
            memcpy(&E.fnv[i - 14][0], &c->f[i].ps0, 8);
            memcpy(&E.fnv[i - 14][1], &c->f[i].ps1, 8);
        }
        const GabiFrameInfo* fi = gmem_frame_info(g_cur_func);
        uint32_t fs = fi ? fi->size : 0;
        for (uint32_t ea = V.entry_sp - fs; ea != V.entry_sp + 8; ea++) E.frame.push_back(peek8(ea));
    }
    V.calls.push_back(std::move(ev));

    if (ci && ci->real) {
        V.depth++;
        ci->real(c);
        V.depth--;
        return;
    }
    if (V.mode == MODE_REC) {
        if (V.rec_calls && k < V.rec_calls->size()) {
            const RecCall& rc = (*V.rec_calls)[k];
            /* volatile state after the call, as recorded (r1/r2/r13-r31, f14-f31 are preserved) */
            Cpu after;
            tap_regs_to(&after, &rc.after);
            c->r[0] = after.r[0];
            for (int i = 3; i <= 12; i++) c->r[i] = after.r[i];
            for (int i = 0; i <= 13; i++) c->f[i] = after.f[i];
            for (int b : {0, 1, 2, 3, 4, 5, 6, 7, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31}) c->cr[b] = after.cr[b];
            c->ctr = after.ctr;
            c->xer_ca = after.xer_ca;
            c->xer_so = after.xer_so;
            c->xer_ov = after.xer_ov;
            /* a returned pointer into the original's stack (e.g. a callee returning its `out` argument): map it
             * to the candidate's corresponding local, as for the memory patches below (game test replay) */
            if (V.is_candidate)
                for (int r : {3, 4}) {
                    uint32_t v = c->r[r];
                    if (!in_stack(v)) continue;
                    uint32_t best = 0, bi = 0;
                    for (int i = 3; i <= 10; i++) {
                        uint32_t a = rc.at.r[i];
                        if (in_stack(a) && a <= v && v - a < 256 && a >= best) { best = a; bi = i; }
                    }
                    if (bi) c->r[r] = ev.r[bi] + (v - rc.at.r[bi]);
                }
            /* memory effects of the callee seen by the function */
            V.depth++;
            for (const Patch& p : rc.patches) {
                uint32_t ea = p.ea;
                if (V.is_candidate && in_stack(ea)) {
                    /* a callee writing through a stack pointer argument: map to the candidate's local */
                    uint32_t best = 0, bi = 0;
                    for (int i = 3; i <= 10; i++) {
                        uint32_t a = rc.at.r[i];
                        if (a <= ea && ea - a < 256 && a >= best) { best = a; bi = i; }
                    }
                    if (bi) ea = ev.r[bi] + (p.ea - rc.at.r[bi]);
                }
                patch8(ea, p.v);
            }
            V.depth--;
            return;
        }
    }
    // Capture the memmove source before this call's mock epoch: a copy does not
    // invent new source bytes. Capture all bytes first to support overlap.
    std::vector<uint8_t> move_bytes;
    uint32_t move_dst = 0;
    auto move = g_move_specs.find(target);
    if (move != g_move_specs.end()) {
        uint32_t size = c->r[move->second.size];
        move_dst = c->r[move->second.dst];
        if (size > 20000000u) {
            V.abort_reason = "memmove exceeds guest operation budget";
            longjmp(V.abort_jmp, 1);
        }
        move_bytes.reserve(size);
        V.depth++;
        for (uint32_t j = 0; j < size; j++) {
            tick();
            move_bytes.push_back(peek8(c->r[move->second.src] + j));
        }
        V.depth--;
    }
    /* generated mock: answer in the registers the real callee may write; memory it may have
     * changed is modelled by the new epoch */
    V.epoch++;
    uint64_t h = hash3(V.seed, target, V.per_target[target]++);
    uint32_t argr[11];
    for (int i = 3; i <= 10; i++) argr[i] = c->r[i]; /* output pointers as passed (the mock's results overwrite r3/r4) */
    if (!ci || ci->defr >> 3 & 1) c->r[3] = shape_ret(ci, typed_ret(h));
    auto rs = g_ret_specs.find(target);
    if (rs != g_ret_specs.end() && ((h >> 45) % 8 != 0 || g_ret_strict.count(target))) /* mostly in the declared domain */
        c->r[3] = (uint32_t)(rs->second.first + (int64_t)((h >> 13) % (uint64_t)(rs->second.second - rs->second.first + 1)));
    if (ci && ci->defr >> 4 & 1) c->r[4] = typed_ret(h >> 5);
    if (!ci || ci->deff >> 1 & 1) c->f[1].ps0 = c->f[1].ps1 = (double)rand_float(h >> 9);
    auto rp = g_ret_picks.find(target);
    if (rp != g_ret_picks.end() && ((h >> 45) % 8 != 0 || g_ret_strict.count(target))) c->r[3] = (uint32_t)rp->second[(h >> 13) % rp->second.size()];
    auto fs = g_fret_specs.find(target);
    if (fs != g_fret_specs.end() && ((h >> 45) % 8 != 0 || g_fret_strict.count(target))) {
        uint64_t k = (h >> 17) % 1000003;
        double v = fs->second.first + (double)k / 1000003.0 * (fs->second.second - fs->second.first);
        if (k % 5 == 0) v = fs->second.first;
        if (k % 5 == 1) v = fs->second.second;
        c->f[1].ps0 = c->f[1].ps1 = (double)(float)v;
    }
    apply_fetch_effects(c, target, argr);
    if (V.depth == 0) { for (int i = 4; i <= 12; i++) V.post_r[i] = c->r[i]; V.have_post = true; }
    if (auto al = g_alloc_specs.find(target); al != g_alloc_specs.end()) {
        uint32_t size = al->second.sizereg ? argr[al->second.sizereg] : al->second.fixed, a = al->second.align;
        uint32_t& cur = g_alloc_cursor.try_emplace(target, al->second.base).first->second;
        cur = (cur + a - 1) & ~(a - 1);
        c->r[3] = cur;
        cur += (size + a - 1) & ~(a - 1);
        if (size > 0x1000000u) { V.abort_reason = "alloc size exceeds 16 MB"; longjmp(V.abort_jmp, 1); }
    }
    if (move != g_move_specs.end()) {
        V.depth++;
        for (uint32_t j = 0; j < move_bytes.size(); j++) {
            tick();
            patch8(move_dst + j, move_bytes[j]);
        }
        V.depth--;
        c->r[3] = move_dst; // OSBlockMove / memmove return the destination.
        if (!V.is_candidate) { g_move_calls++; g_move_bytes += move_bytes.size(); }
    }
    if (auto u = g_utf16_specs.find(target); u != g_utf16_specs.end()) {
        // Lookup may fail without writing any byte. On success it writes only
        // the chosen nonzero UTF-16 prefix and its terminator; untouched tail
        // bytes retain their prior values. Never unconditional out34.
        bool success = (h % 4) != 0;
        unsigned length = (h >> 7) % (u->second.maxchars + 1);
        if (!V.is_candidate) {
            if (success) g_utf16_length[length]++;
            else g_utf16_fail++;
        }
        if (success) {
            V.depth++;
            for (unsigned j = 0; j <= length; j++) {
                uint64_t ch = hash3(h, j, 0x16);
                uint16_t value = j == length ? 0 :
                    ((ch % 4) ? 0x61u : (0x20u + ((ch >> 13) % 95)));
                patch8(argr[u->second.out] + 2 * j, value >> 8);
                patch8(argr[u->second.out] + 2 * j + 1, value);
            }
            V.depth--;
        }
    }
    if (ci) {
        V.depth++;
        for (int i = 3; i <= 10; i++)
            if ((ci->fill >> i & 1) && ci->ptrsz[i] && ci->ptrsz[i] != 255 && ci->ptrsz[i] != 0xFFFE && in_stack(argr[i])) /* only declared output storage (rN=outSIZE / inoutSIZE) */
                for (uint32_t j = 0; j < ci->ptrsz[i]; j += 4) {
                    uint32_t w = typed_word(hash3(h, i, j));
                    for (uint32_t b = 0; b < 4 && j + b < ci->ptrsz[i]; b++) patch8(argr[i] + j + b, (uint8_t)(w >> (24 - 8 * b)));
                }
        V.depth--;
    }
    if (auto mfs = g_mockfield.find(target); mfs != g_mockfield.end()) {
        V.depth++;
        int k = 0;
        for (const MockField& mf : mfs->second) {
            uint64_t hk = hash3(h, 0x6D66, k++);
            uint32_t base = mf.reg ? argr[mf.reg] : 0;
            if (mf.reg && base == 0) continue; /* a null object: nothing to fill */
            double v = !mf.pick.empty() ? mf.pick[(hk >> 7) % mf.pick.size()]
                     : mf.is_float ? mf.lo + (double)((hk >> 11) % 1000001) / 1000000.0 * (mf.hi - mf.lo)
                                   : mf.lo + (double)((hk >> 11) % (uint64_t)(mf.hi - mf.lo + 1));
            uint32_t bits;
            if (mf.is_float) { float fv = (float)v; memcpy(&bits, &fv, 4); }
            else bits = (uint32_t)(int64_t)v;
            for (int b = 0; b < mf.size; b++) patch8(base + mf.off + b, (uint8_t)(bits >> (8 * (mf.size - 1 - b))));
        }
        V.depth--;
    }
}

extern "C" {
void vm_call(Cpu* c, uint32_t target, int kind) { do_call(c, target, kind, -1, -1); }
void gmem_call(Cpu* c, uint32_t target, int kind, int nint, int nflt) { do_call(c, target, kind, nint, nflt); }
}

/* ------------------------------------------------------------------ inputs */
struct FieldSpec {
    uint32_t func; /* 0 = all */
    int base_reg;  /* -1 absolute */
    uint32_t off;
    int size;
    bool is_float;
    double lo, hi;
    std::vector<double> pick; /* `field ... TYPE v1,v2,...`: one of these values */
};
static std::vector<FieldSpec> g_specs;
/* arg FUNC rN|fN LO HI: argument register ranges for generated inputs (edges drawn often) */
struct ArgSpec {
    uint32_t func;
    bool is_float;
    int reg;
    double lo, hi;
    std::vector<double> pick;
    bool strict;
};
static std::vector<ArgSpec> g_arg_specs;
/* fieldstr FUNC|* ADDR|rN+OFF [SIZE] "s1","s2",...|@stages: one whole NUL-terminated string per input
 * (same on both sides); about 1 input in 16 gets the empty string and 1 in 16 keeps the generated
 * bytes (as does each `@keep` pick, to mix with earlier `field` steering of the same bytes). SIZE (default 8) bytes from ADDR are also made `stable` for absolute addresses. */
struct StrSpec {
    uint32_t func;
    int base_reg;
    uint32_t off;
    std::vector<std::string> pick;
};
static std::vector<StrSpec> g_str_specs;
static std::vector<std::string> g_stage_names; /* tools/verify/stage_names.txt, loaded on first @stages */
static std::string g_spec_dir = ".";

static void load_ctor_sizes(const char* spec_path) {
    std::string p = spec_path;
    size_t k = p.rfind('/');
    p = (k == std::string::npos ? std::string(".") : p.substr(0, k)) + "/../ctor_sizes.tsv"; /* units/X.txt -> ctor_sizes.tsv */
    FILE* f = fopen(p.c_str(), "r");
    if (!f) return;
    char line[256];
    while (fgets(line, sizeof line, f)) {
        unsigned a, n;
        if (line[0] != '#' && sscanf(line, "%x %x", &a, &n) == 2 && n) g_ctor_size[a] = n;
    }
    fclose(f);
}

static void load_livein() {
    FILE* f = fopen("build/verify/livein.tsv", "r"); /* verify.py / mutate.py run with cwd = repository root */
    if (!f) return;
    char line[128];
    while (fgets(line, sizeof line, f)) {
        unsigned a, im, fm;
        if (sscanf(line, "%x %x %x", &a, &im, &fm) == 3) g_livein[a] = {im, fm};
    }
    fclose(f);
}

static void load_specs(const char* path) {
    {
        std::string p = path;
        size_t k = p.rfind('/');
        g_spec_dir = k == std::string::npos ? std::string(".") : p.substr(0, k);
    }
    load_ctor_sizes(path);
    load_livein();
    FILE* f = fopen(path, "r");
    if (!f) return;
    static char line[16384];
    while (fgets(line, sizeof line, f)) {
        size_t len = strlen(line);
        if (len == sizeof line - 1 && line[len - 1] != '\n') {
            fprintf(stderr, "unit file line longer than %zu characters (split it): %.80s...\n", sizeof line - 2, line);
            exit(2);
        }
        char* h = strchr(line, '#');
        if (h) *h = 0;
        char kw[32], fn[32], where[64], ty[16];
        double lo, hi;
        int n = sscanf(line, "%31s %31s %63s %15s %lf %lf", kw, fn, where, ty, &lo, &hi);
        if (n < 1) continue;
        if (!strcmp(kw, "mockmove")) { // mockmove TARGET rDST rSRC rSIZE
            unsigned target, dst, src, size;
            if (sscanf(line, "%*s %x r%u r%u r%u", &target, &dst, &src, &size) != 4 ||
                dst < 3 || dst > 10 || src < 3 || src > 10 || size < 3 || size > 10) {
                fprintf(stderr, "invalid mockmove directive: %s", line); exit(2);
            }
            g_move_specs[target] = {dst, src, size};
            continue;
        }
        if (!strcmp(kw, "mockutf16")) { // mockutf16 TARGET rOUT MAX_NONZERO_CHARS
            unsigned target, out, maxchars;
            if (sscanf(line, "%*s %x r%u %u", &target, &out, &maxchars) != 3 ||
                out < 3 || out > 10 || maxchars > 4096) {
                fprintf(stderr, "invalid mockutf16 directive: %s", line); exit(2);
            }
            g_utf16_specs[target] = {out, maxchars};
            continue;
        }
        if (!strcmp(kw, "nolivein")) { /* nolivein FUNC: skip the indirect-call live-in comparison in FUNC (document why) */
            unsigned fa;
            if (sscanf(line, "%*s %x", &fa) == 1) g_nolivein.insert(fa);
            continue;
        }
        if (!strcmp(kw, "mockfield") || !strcmp(kw, "mockglobal")) { /* mockglobal TARGET HEXADDR TYPE VALUES: opt-in generated callback global write */
            MockField mf{};
            char wh[64], tyb[16];
            static char vals[16384];
            char hib[64];
            unsigned tgt;
            int m = sscanf(line, "%*s %x %63s %15s %16383s %63s", &tgt, wh, tyb, vals, hib);
            char* plus = nullptr;
            if (m < 4 || (!strcmp(kw,"mockfield") && wh[0] != 'r')) { fprintf(stderr, "invalid mockfield directive: %s", line); exit(2); }
            if (!strcmp(kw, "mockglobal")) {
                char* end;
                mf.reg = 0;
                mf.off = (uint32_t)strtoul(wh, &end, 16);
                if (*end || (strcmp(tyb,"u8") && strcmp(tyb,"u16") && strcmp(tyb,"u32") && strcmp(tyb,"f32"))) {
                    fprintf(stderr,"invalid mockglobal directive: %s",line); exit(2);
                }
            } else {
                mf.reg = (int)strtol(wh + 1, &plus, 10);
                mf.off = plus && *plus == '+' ? (uint32_t)strtoul(plus + 1, nullptr, 0) : 0;
            }
            mf.is_float = tyb[0] == 'f';
            mf.size = atoi(tyb + 1) / 8;
            if ((!strcmp(kw,"mockfield") && (mf.reg < 3 || mf.reg > 10)) || !(mf.size == 1 || mf.size == 2 || mf.size == 4)) { fprintf(stderr, "invalid mockfield directive: %s", line); exit(2); }
            if (strchr(vals, ',')) {
                for (char* t = strtok(vals, ","); t; t = strtok(nullptr, ","))
                    mf.pick.push_back(mf.is_float ? strtod(t, nullptr) : (double)strtoll(t, nullptr, 0));
            } else {
                mf.lo = mf.is_float ? strtod(vals, nullptr) : (double)strtoll(vals, nullptr, 0);
                mf.hi = m >= 5 ? (mf.is_float ? strtod(hib, nullptr) : (double)strtoll(hib, nullptr, 0)) : mf.lo;
            }
            g_mockfield[tgt].push_back(mf);
            continue;
        }
        if (!strcmp(kw, "footprint")) { /* footprint FUNC SIZE */
            char sb[64];
            if (sscanf(line, "%*s %*s %63s", sb) == 1) g_footprint[(uint32_t)strtoul(fn, nullptr, 16)] = (uint32_t)strtoul(sb, nullptr, 0);
            continue;
        }
        if (!strcmp(kw, "stackfill")) { /* stackfill [periodic] */
            g_stackfill = true;
            if (n >= 2 && !strcmp(fn, "periodic")) g_stackfill_periodic = true;
            continue;
        }
        if (!strcmp(kw, "fieldstr")) { /* fieldstr FUNC|* ADDR|rN+OFF [SIZE] "a","b",...|@stages */
            StrSpec ss{};
            char fb[32], wb[64];
            int used = 0;
            if (sscanf(line, "%*s %31s %63s %n", fb, wb, &used) < 2 || !used) { fprintf(stderr, "invalid fieldstr directive: %s", line); exit(2); }
            ss.func = strcmp(fb, "*") ? (uint32_t)strtoul(fb, nullptr, 16) : 0;
            if (wb[0] == 'r') {
                char* plus;
                ss.base_reg = (int)strtol(wb + 1, &plus, 10);
                ss.off = *plus == '+' ? (uint32_t)strtoul(plus + 1, nullptr, 0) : 0;
            } else {
                ss.base_reg = -1;
                ss.off = (uint32_t)strtoul(wb, nullptr, 16);
            }
            const char* q = line + used;
            uint32_t size = 8;
            if (*q >= '0' && *q <= '9') { char* e; size = (uint32_t)strtoul(q, &e, 0); q = e; }
            while (*q) {
                while (*q == ' ' || *q == '\t' || *q == ',' || *q == '\n' || *q == '\r') q++;
                if (!*q) break;
                if (*q == '"') {
                    const char* e = strchr(q + 1, '"');
                    if (!e) { fprintf(stderr, "invalid fieldstr string: %s", line); exit(2); }
                    std::string v; /* \xNN escapes a byte (e.g. "sea\x01" for a non-terminated prefix) */
                    for (const char* t = q + 1; t < e; t++) {
                        if (t[0] == '\\' && t[1] == 'x' && t + 3 < e && isxdigit((unsigned char)t[2]) && isxdigit((unsigned char)t[3])) {
                            char hx[3] = {t[2], t[3], 0};
                            v.push_back((char)strtoul(hx, nullptr, 16));
                            t += 3;
                        } else v.push_back(*t);
                    }
                    ss.pick.push_back(v);
                    q = e + 1;
                } else if (!strncmp(q, "@keep", 5)) { /* this pick keeps the generated (field-steered) bytes */
                    ss.pick.push_back(std::string("\xFF@keep", 6));
                    q += 5;
                } else if (!strncmp(q, "@stages", 7)) {
                    if (g_stage_names.empty()) {
                        std::string sp = g_spec_dir + "/../stage_names.txt";
                        FILE* sf = fopen(sp.c_str(), "r");
                        if (!sf) { fprintf(stderr, "fieldstr: no %s\n", sp.c_str()); exit(2); }
                        char sl[128];
                        while (fgets(sl, sizeof sl, sf)) {
                            char* t = strtok(sl, " \t\r\n");
                            if (t && t[0] != '#') g_stage_names.emplace_back(t);
                        }
                        fclose(sf);
                    }
                    ss.pick.insert(ss.pick.end(), g_stage_names.begin(), g_stage_names.end());
                    q += 7;
                } else { fprintf(stderr, "invalid fieldstr value: %s", line); exit(2); }
            }
            if (ss.pick.empty()) { fprintf(stderr, "fieldstr without strings: %s", line); exit(2); }
            if (ss.base_reg < 0) g_stable.emplace_back(ss.off, size);
            g_str_specs.push_back(ss);
            continue;
        }
        if (!strcmp(kw, "stable")) { /* stable ADDR SIZE */
            char sa[64], sb[64];
            if (sscanf(line, "%*s %63s %63s", sa, sb) == 2)
                g_stable.emplace_back((uint32_t)strtoul(sa, nullptr, 16), (uint32_t)strtoul(sb, nullptr, 0));
            continue;
        }
        if (!strcmp(kw, "alloc")) { /* alloc TARGET rSIZE BASE [ALIGN] */
            unsigned target, reg, base, align = 32;
            int m = sscanf(line, "%*s %x r%u %x %u", &target, &reg, &base, &align);
            if (m < 3 || reg < 3 || reg > 10 || align == 0 || (align & (align - 1))) {
                fprintf(stderr, "invalid alloc directive: %s", line); exit(2);
            }
            g_alloc_specs[target] = {reg, base, align, 0};
            continue;
        }
        if (!strcmp(kw, "allocfixed")) { /* allocfixed TARGET SIZE BASE [ALIGN]: a factory whose request size is fixed */
            unsigned target, size, base, align = 32;
            char extra[8];
            int m = sscanf(line, "%*s %x %i %x %u %7s", &target, (int*)&size, &base, &align, extra);
            if (m < 3 || m > 4 || size == 0 || size > 0x1000000u || align == 0 || (align & (align - 1))) {
                fprintf(stderr, "invalid allocfixed directive: %s", line); exit(2);
            }
            g_alloc_specs[target] = {0, base, align, size};
            continue;
        }
        if (!strcmp(kw, "mockfetchsize") || !strcmp(kw, "mockfetchinit")) { /* mockfetch{size,init} TARGET */
            unsigned target;
            if (sscanf(line, "%*s %x", &target) != 1) { fprintf(stderr, "invalid %s directive: %s", kw, line); exit(2); }
            (kw[9] == 's' ? g_fetch_size : g_fetch_init).insert(target);
            continue;
        }
        if (!strcmp(kw, "nestedret")) { /* nestedret TARGET VALUE */
            unsigned target, value;
            if (sscanf(line, "%*s %x %x", &target, &value) != 2) {
                fprintf(stderr, "invalid nestedret directive: %s", line); exit(2);
            }
            g_nestedret[target] = value;
            continue;
        }
        if (!strcmp(kw, "final")) { /* final FUNC ADDR SIZE */
            unsigned fnc, addr, size;
            if (sscanf(line, "%*s %x %x %x", &fnc, &addr, &size) != 3 || size == 0 || size > 0x10000) {
                fprintf(stderr, "invalid final directive: %s", line); exit(2);
            }
            g_final_specs[fnc].emplace_back(addr, size);
            continue;
        }
        if (!strcmp(kw, "framexact")) { /* framexact FUNC RET */
            unsigned func, ret;
            if (sscanf(line, "%*s %x %x", &func, &ret) != 2) { fprintf(stderr, "invalid framexact directive: %s", line); exit(2); }
            g_framexact.insert({func, ret});
            g_framexact_funcs.insert(func);
            continue;
        }
        if (!strcmp(kw, "observe")) { /* observe CALLEE GLOBAL SIZE: exact bytes before call effects */
            unsigned target, addr, size;
            if (sscanf(line, "%*s %x %x %u", &target, &addr, &size) != 3 ||
                size == 0 || size > 4096 || uint64_t(addr) + size > 0x100000000ull) {
                fprintf(stderr, "invalid observe directive: %s", line);
                exit(2);
            }
            g_observe_specs[target].emplace_back(addr, size);
            continue;
        }
        if (!strcmp(kw, "ret") || !strcmp(kw, "ret!")) { /* ret[!] ADDR LO HI: a callee's results (generated mocks) */
            static char sa[16384]; char sb[64];
            int m = sscanf(line, "%*s %31s %16383s %63s", fn, sa, sb);
            uint32_t fa = (uint32_t)strtoul(fn, nullptr, 16);
            if (kw[3] == '!') g_ret_strict.insert(fa);
            if (m >= 2 && strchr(sa, ',')) { /* ret ADDR v1,v2,...: one of these values */
                for (char* t = strtok(sa, ","); t; t = strtok(nullptr, ",")) g_ret_picks[fa].push_back(strtoll(t, nullptr, 0));
            } else if (m == 3) {
                g_ret_specs[fa] = {strtoll(sa, nullptr, 0), strtoll(sb, nullptr, 0)};
            }
            continue;
        }
        if (!strcmp(kw, "fret") || !strcmp(kw, "fret!")) { /* fret[!] ADDR LO HI: a callee's f1 results (generated mocks), edges often */
            double a, b;
            if (sscanf(line, "%*s %31s %lf %lf", fn, &a, &b) == 3) {
                g_fret_specs[(uint32_t)strtoul(fn, nullptr, 16)] = {a, b};
                if (kw[4] == '!') g_fret_strict.insert((uint32_t)strtoul(fn, nullptr, 16));
            }
            continue;
        }
        if ((!strcmp(kw, "arg") || !strcmp(kw, "arg!")) && n >= 4) { /* arg[!] FUNC rN|fN LO [HI]  or  v1,v2,... */
            ArgSpec a{};
            a.strict = kw[3] == '!'; /* arg!: always applied (no unsteered inputs) */
            a.func = strcmp(fn, "*") ? (uint32_t)strtoul(fn, nullptr, 16) : 0;
            a.is_float = where[0] == 'f';
            a.reg = atoi(where + 1);
            static char sa[16384]; char sb[64];
            int m = sscanf(line, "%*s %*s %*s %16383s %63s", sa, sb);
            if (strchr(sa, ',')) {
                for (char* t = strtok(sa, ","); t; t = strtok(nullptr, ","))
                    a.pick.push_back(a.is_float ? strtod(t, nullptr) : (double)strtoll(t, nullptr, 0));
            } else {
                a.lo = a.is_float ? strtod(sa, nullptr) : (double)strtoll(sa, nullptr, 0);
                a.hi = m >= 2 ? (a.is_float ? strtod(sb, nullptr) : (double)strtoll(sb, nullptr, 0)) : a.lo;
            }
            g_arg_specs.push_back(a);
            continue;
        }
        if (n < 5 || strcmp(kw, "field")) continue;
        if (n == 5) hi = lo;
        FieldSpec s{};
        s.func = strcmp(fn, "*") ? (uint32_t)strtoul(fn, nullptr, 16) : 0;
        if (where[0] == 'r') {
            char* plus;
            s.base_reg = (int)strtol(where + 1, &plus, 10);
            s.off = *plus == '+' ? (uint32_t)strtoul(plus + 1, nullptr, 0) : 0;
        } else {
            s.base_reg = -1;
            s.off = (uint32_t)strtoul(where, nullptr, 16);
        }
        s.is_float = ty[0] == 'f';
        s.size = atoi(ty + 1) / 8;
        s.lo = lo;
        s.hi = hi;
        {
            static char sv[16384];
            if (sscanf(line, "%*s %*s %*s %*s %16383s", sv) == 1 && strchr(sv, ',')) {
                for (char* t = strtok(sv, ","); t; t = strtok(nullptr, ","))
                    s.pick.push_back(s.is_float ? strtod(t, nullptr) : (double)strtoll(t, nullptr, 0));
            } else if (!s.is_float && sscanf(line, "%*s %*s %*s %*s %16383s", sv) == 1) {
                s.lo = (double)strtoll(sv, nullptr, 0);
                char sh[64];
                s.hi = sscanf(line, "%*s %*s %*s %*s %*s %63s", sh) == 1 ? (double)strtoll(sh, nullptr, 0) : s.lo;
            }
        }
        g_specs.push_back(s);
    }
    fclose(f);
}

static void load_image(const char* path) {
    FILE* f = fopen(path, "rb");
    if (!f) { fprintf(stderr, "no image %s (run mkimage.py)\n", path); exit(1); }
    uint32_t hdr[2];
    while (fread(hdr, 4, 2, f) == 2) {
        Region r;
        r.addr = hdr[0];
        r.size = hdr[1];
        r.data.resize(r.size);
        if (fread(r.data.data(), 1, r.size, f) != r.size) break;
        V.image.push_back(std::move(r));
    }
    fclose(f);
}

struct Input {
    std::string label;
    Cpu entry;
    bool recorded = false;
    std::vector<RecCall> calls;
    std::map<uint32_t, uint8_t> rec_net; /* recorded net writes outside the stack (replay check) */
    TapRegs rec_exit;
};

static const uint32_t kGenSp = 0x7EFFF000u;

static void base_cpu(Cpu* c) {
    memset(c, 0, sizeof *c);
    c->gqr[2] = 0x40004; c->gqr[3] = 0x50005; c->gqr[4] = 0x60006; c->gqr[5] = 0x70007;
    c->fpscr = 4;
    c->lr = 0x0BADC0DEu;
}

static void make_generated(const OrigFunc* of, uint64_t seed, Input* in) {
    char b[64];
    snprintf(b, sizeof b, "gen seed=%" PRIu64, seed);
    in->label = b;
    Cpu* c = &in->entry;
    base_cpu(c);
    c->r[1] = kGenSp;
    int fi = 1;
    for (int i = 3; i <= 10; i++) {
        uint64_t h = hash3(seed, i, 0xA4C);
        uint32_t x = (uint32_t)(h >> 16);
        switch (of->argtype[i]) {
        case 'p': c->r[i] = 0x40000000u + (uint32_t)(i - 3) * 0x01000000u + ((h % 4 == 0) ? 0 : 0); break;
        case 'h': c->r[i] = (uint32_t)(int32_t)(int16_t)x; break;
        case 'H': c->r[i] = x & 0xFFFF; break;
        case 'c': c->r[i] = (uint32_t)(int32_t)(int8_t)x; break;
        case 'b': c->r[i] = (h % 3 == 0) ? (x & 0xFF) : (x & 3); break;
        case 'i': c->r[i] = (h % 2) ? (x & 0xF) : x; break;
        default: c->r[i] = typed_word(h); break;
        }
    }
    for (; fi <= 8; fi++) c->f[fi].ps0 = c->f[fi].ps1 = rand_float(hash3(seed, fi, 0xF1));
    /* `footprint`: the caller's non-volatile registers get distinct values, so a candidate that
     * writes the original's saved-register slots (stmw / stw rN) must store the right ones */
    if (footprint_of(of->addr) != UINT32_MAX || g_framexact_funcs.count(of->addr))
        for (int i = 14; i <= 31; i++) c->r[i] = (uint32_t)(hash3(seed, i, 0x5A7E) >> 16);
    if (g_framexact_funcs.count(of->addr)) /* paired singles with distinct halves: ps1 is saved and reloaded too */
        for (int i = 14; i <= 31; i++) {
            c->f[i].ps0 = rand_float(hash3(seed, i, 0xF5A0));
            c->f[i].ps1 = rand_float(hash3(seed, i, 0xF5A1));
        }
    for (const ArgSpec& s : g_arg_specs) {
        if (s.func && s.func != of->addr) continue;
        uint64_t h = hash3(seed, (uint64_t)s.reg + (s.is_float ? 100 : 0), 0xA5C);
        if (h % 8 == 7 && !s.strict) continue; /* leave some inputs unsteered */
        if (s.is_float) {
            double v = s.lo + (double)(h % 1000003) / 1000003.0 * (s.hi - s.lo);
            if (h % 7 == 0) v = s.lo;
            if (h % 7 == 1) v = s.hi;
            c->f[s.reg].ps0 = c->f[s.reg].ps1 = (double)(float)v;
        } else {
            int64_t v = (int64_t)s.lo + (int64_t)((h >> 8) % (uint64_t)((int64_t)s.hi - (int64_t)s.lo + 1));
            if (h % 7 == 0) v = (int64_t)s.lo;
            if (h % 7 == 1) v = (int64_t)s.hi;
            c->r[s.reg] = (uint32_t)v;
        }
        if (!s.pick.empty()) {
            double v = s.pick[(h >> 11) % s.pick.size()];
            if (s.is_float) c->f[s.reg].ps0 = c->f[s.reg].ps1 = (double)(float)v;
            else c->r[s.reg] = (uint32_t)(int64_t)v;
        }
    }
    for (const FieldSpec& s : g_specs) {
        if (s.func && s.func != of->addr) continue;
        uint32_t a = (s.base_reg >= 0 ? c->r[s.base_reg] : 0) + s.off;
        uint64_t h = hash3(seed, a, 0xF1E1D);
        double v = s.lo + (double)(h % 1000003) / 1000003.0 * (s.hi - s.lo);
        uint64_t raw;
        if (s.is_float) raw = s.size == 4 ? f32_as_u32((float)v) : f64_as_u64(v);
        else raw = (uint64_t)(int64_t)(s.lo + (double)(h % (uint64_t)(s.hi - s.lo + 1)));
        if (h % 7 == 0) raw = s.is_float ? (s.size == 4 ? f32_as_u32((float)s.lo) : f64_as_u64(s.lo)) : (uint64_t)(int64_t)s.lo; /* edges */
        if (h % 7 == 1) raw = s.is_float ? (s.size == 4 ? f32_as_u32((float)s.hi) : f64_as_u64(s.hi)) : (uint64_t)(int64_t)s.hi;
        if (!s.pick.empty()) {
            double v = s.pick[(h >> 5) % s.pick.size()];
            raw = s.is_float ? (s.size == 4 ? f32_as_u32((float)v) : f64_as_u64(v)) : (uint64_t)(int64_t)v;
        }
        for (int k = 0; k < s.size; k++) snap_set(a + k, (uint8_t)(raw >> (8 * (s.size - 1 - k))));
    }
    for (const StrSpec& s : g_str_specs) {
        if (s.func && s.func != of->addr) continue;
        uint32_t a = (s.base_reg >= 0 ? c->r[s.base_reg] : 0) + s.off;
        uint64_t h = hash3(seed, a, 0x57A6E);
        if (h % 16 == 15) continue;                       /* keep the generated bytes */
        const std::string& v = h % 16 == 14 ? std::string() : s.pick[(h >> 4) % s.pick.size()];
        if (v.size() == 6 && v[0] == '\xFF' && v.compare(1, 5, "@keep") == 0) continue;
        for (size_t k = 0; k < v.size(); k++) snap_set(a + (uint32_t)k, (uint8_t)v[k]);
        snap_set(a + (uint32_t)v.size(), 0);
    }
}

/* tap file -> snapshot + per-call results/patches */
static bool load_recorded(const char* path, Input* in, std::string* why) {
    FILE* f = fopen(path, "rb");
    if (!f) { *why = "cannot open"; return false; }
    TapHeader h;
    if (fread(&h, sizeof h, 1, f) != 1 || h.magic != TAP_MAGIC) { fclose(f); *why = "bad header"; return false; }
    in->label = path;
    in->recorded = true;
    base_cpu(&in->entry);
    tap_regs_to(&in->entry, &h.entry);
    std::unordered_map<uint32_t, uint8_t> shadow;
    std::vector<std::pair<uint32_t, uint8_t>> writes;
    uint32_t sp = in->entry.r[1];
    bool ok = true;
    for (;;) {
        TapEvent e;
        if (fread(&e, sizeof e, 1, f) != 1) { *why = "truncated"; ok = false; break; }
        if (e.type == TAP_READ || e.type == TAP_WRITE) {
            for (int k = 0; k < e.size; k++) {
                uint32_t a = e.ea + k;
                uint8_t v = (uint8_t)(e.value >> (8 * (e.size - 1 - k)));
                if (e.type == TAP_WRITE) { shadow[a] = v; writes.push_back({a, v}); continue; }
                auto it = shadow.find(a);
                bool own_frame = a - (sp - 0x10000) < 0x10000;
                if (it == shadow.end() && own_frame && !in->calls.empty()) {
                    /* first read of the function's own frame after a call: a callee's output */
                    in->calls.back().patches.push_back({a, v});
                    shadow[a] = v;
                } else if (it == shadow.end()) { snap_set(a, v); shadow[a] = v; }
                else if (it->second != v) {
                    if (in->calls.empty()) { *why = "memory changed without a call (another thread)"; ok = false; }
                    else in->calls.back().patches.push_back({a, v});
                    it->second = v;
                }
            }
        } else if (e.type == TAP_CALL) {
            RecCall rc;
            rc.target = e.ea;
            if (fread(&rc.at, sizeof rc.at, 1, f) != 1) { ok = false; break; }
            in->calls.push_back(rc);
        } else if (e.type == TAP_RET) {
            if (in->calls.empty() || fread(&in->calls.back().after, sizeof(TapRegs), 1, f) != 1) { ok = false; break; }
        } else if (e.type == TAP_END) {
            if (fread(&in->rec_exit, sizeof(TapRegs), 1, f) != 1) ok = false;
            break;
        } else { *why = "bad event"; ok = false; break; }
    }
    fclose(f);
    /* expected net writes: final shadow values that differ from the initial snapshot */
    for (auto& w : writes) {
        uint32_t a = w.first;
        if (a - (sp - 0x10000) < 0x10000 + 8) continue;
        uint8_t fin = shadow[a];
        bool known = snap_has(a) || image_region(a); /* same rule as net_writes() */
        uint8_t init = 0;
        if (known) init = initial_byte(a, &known);
        if (!known || init != fin) in->rec_net[a] = fin;
    }
    return ok;
}

/* ------------------------------------------------------------------ runs and comparison */
struct Result {
    uint32_t entry_sp = 0;
    Cpu exit;
    std::vector<CallEv> calls;
    std::map<uint32_t, uint8_t> net;
    std::set<uint32_t> dwords; /* complete explicit f64 store starts */
    std::set<uint32_t> fwords; /* words whose last store was a float store */
    std::vector<uint8_t> final_bytes; /* `final` ranges at exit */
    bool aborted = false;
    std::string why;
    uint32_t stray = 0, first_stray = 0;
    int traps = 0;
};

static void run(void (*fn)(Cpu*), const Input& in, bool candidate, Result* out, std::vector<uint32_t>* cov) {
    reset_memory();
    V.calls.clear();
    V.per_target.clear();
    g_alloc_cursor.clear();
    V.fwords.clear();
    V.dwords.clear();
    V.nfcopy = 0;
    V.epoch = 0;
    V.depth = 0;
    V.is_candidate = candidate;
    V.locals.clear();
    V.reserved.clear();
    V.local_objsz.clear();
    V.stack_internal = 0;
    V.call_top = 0;
    if (!candidate) V.orig_stack.clear();
    V.have_post = false;
    V.budget = 0;
    V.stray = 0;
    V.traps = 0;
    V.cov = cov;
    V.mode = in.recorded ? MODE_REC : MODE_GEN;
    V.rec_calls = &in.calls;
    V.entry_sp = in.entry.r[1];
    V.stack_lo = in.entry.r[1] - 0x10000;
    Cpu c = in.entry;
    gabi::cpu = &c;
    V.can_abort = true;
    if (setjmp(V.abort_jmp) == 0) {
        fn(&c);
    } else {
        out->aborted = true;
        out->why = V.abort_reason;
    }
    V.can_abort = false;
    V.cov = nullptr;
    out->exit = c;
    out->entry_sp = in.entry.r[1];
    out->calls = std::move(V.calls); /* moved, not copied */
    V.calls.clear();
    if (!candidate) {
        V.orig_seq.clear();
        for (auto& e : out->calls) V.orig_seq.push_back({e.target, e.kind});
    }
    out->net = net_writes();
    out->fwords = V.fwords;
    out->dwords = V.dwords;
    if (auto fs = g_final_specs.find(g_cur_func); fs != g_final_specs.end() && !out->aborted)
        for (auto& r : fs->second)
            for (uint32_t j = 0; j < r.second; j++) out->final_bytes.push_back(peek8(r.first + j));
    out->stray = V.stray;
    out->first_stray = V.first_stray;
    out->traps = V.traps;
    if (g_memstat) { /* WWHD_MEMSTAT (diagnostic, off by default): per run guest pages, arena bytes, calls, accesses */
        size_t arena = 0;
        for (auto& ch : g_arena_chunks) arena += ch.second;
        fprintf(stderr, "memstat %s seed=%llu pages=%zu arena=%.1f MB snap=%zu calls=%zu budget=%llu\n", candidate ? "cand" : "orig",
                (unsigned long long)V.seed, V.pages.size(), arena / 1048576.0, V.snap.size(), out->calls.size(),
                (unsigned long long)V.budget);
    }
}

static std::string hex32(uint32_t v) { char b[16]; snprintf(b, sizeof b, "%08X", v); return b; }

static std::string callee_name(uint32_t t) {
    const CalleeInfo* ci = callee_info(t);
    return hex32(t) + (ci && ci->name[0] ? std::string(" ") + ci->name : "");
}

/* NaN payloads are not part of the specification: when two NaNs meet, which one propagates
 * depends on the host compiler's operand order (in the recompiled original as much as in the
 * candidate), and clang may fold a float load/store pair that would quiet a signalling NaN.
 * So any two NaNs compare equal (-strictnan: bit-exact). */
static bool g_strict_nan = false;
static bool is_nan32(uint32_t w) { return (w & 0x7F800000u) == 0x7F800000u && (w & 0x007FFFFFu); }
static bool same_f(double a, double b) {
    if (!g_strict_nan && std::isnan(a) && std::isnan(b)) return true;
    return f64_as_u64(a) == f64_as_u64(b);
}

/* net memory effects equal, up to NaN payloads of aligned float words */
static bool same_net(const std::map<uint32_t, uint8_t>& x, const std::map<uint32_t, uint8_t>& y, uint32_t* where,
                     const std::set<uint32_t>& fx, const std::set<uint32_t>& fy,
                     const std::set<uint32_t>& dx = {}, const std::set<uint32_t>& dy = {}) {
    if (x == y) return true;
    std::set<uint32_t> diff; /* bytes whose final values differ */
    for (auto& kv : x) { auto it = y.find(kv.first); if (it == y.end() || it->second != kv.second) diff.insert(kv.first); }
    for (auto& kv : y) { auto it = x.find(kv.first); if (it == x.end() || it->second != kv.second) diff.insert(kv.first); }
    auto final_word = [&](const std::map<uint32_t, uint8_t>& m, uint32_t a) {
        uint32_t w = 0;
        for (uint32_t b = 0; b < 4; b++) {
            bool known;
            uint8_t init = initial_byte(a + b, &known);
            auto it = m.find(a + b);
            w = w << 8 | (it != m.end() ? it->second : init);
        }
        return w;
    };
    if (!g_strict_nan) {
        std::set<uint32_t> doubles(dx); doubles.insert(dy.begin(),dy.end());
        for (uint32_t d : doubles) {
            uint64_t ax=(uint64_t)final_word(x,d)<<32 | final_word(x,d+4);
            uint64_t ay=(uint64_t)final_word(y,d)<<32 | final_word(y,d+4);
            auto nan64=[](uint64_t v) { return (v&0x7FF0000000000000ULL)==0x7FF0000000000000ULL && (v&0x000FFFFFFFFFFFFFULL)!=0; };
            if(nan64(ax)&&nan64(ay)) for(uint32_t k=0;k<8;k++) diff.erase(d+k);
        }
    }
    /* NaN payload tolerance only for words that one side last wrote with a float store (the
     * recompiled original may turn an lfs/stfs pair into an integer copy, which keeps a
     * signalling NaN): an integer word (e.g. two s16) that looks like a NaN must match exactly */
    if (!g_strict_nan) {
        std::set<uint32_t> fl(fx);
        fl.insert(fy.begin(), fy.end());
        for (uint32_t f : fl) {
            // Guest addresses wrap modulo 2^32, including a float spanning address zero.
            bool overlaps = false;
            for (uint32_t b = 0; b < 4; ++b) overlaps |= diff.count(f + b) != 0;
            if (!overlaps) continue;
            if (is_nan32(final_word(x, f)) && is_nan32(final_word(y, f)))
                for (uint32_t b = 0; b < 4; ++b) diff.erase(f + b);
        }
    }
    if (diff.empty()) return true;
    *where = *diff.begin();
    return false;
}

/* stack objects passed by pointer: same bytes, except that pointers into the stack (e.g. an
 * object pointing at its own members) are compared relative to the object's address */
static bool same_stack_obj(const std::vector<uint8_t>& x, uint32_t bx, uint32_t spx, const std::vector<uint8_t>& y, uint32_t by,
                           uint32_t spy, const std::vector<uint8_t>& flx = {}, const std::vector<uint8_t>& fly = {}) {
    if (x.size() != y.size()) return false;
    std::vector<uint8_t> normalized(y);
    if (!g_strict_nan) for(size_t d=0;d+8<=x.size();d++) {
        if(!((flx.size()>d && flx[d]==2)||(fly.size()>d && fly[d]==2))) continue;
        uint64_t ax=0,ay=0;
        for(size_t k=0;k<8;k++) { ax=ax<<8|x[d+k]; ay=ay<<8|y[d+k]; }
        auto nan64=[](uint64_t v) { return (v&0x7FF0000000000000ULL)==0x7FF0000000000000ULL && (v&0x000FFFFFFFFFFFFFULL)!=0; };
        if(nan64(ax)&&nan64(ay)) for(size_t k=0;k<8;k++) normalized[d+k]=x[d+k];
    }
    const auto& exactY=normalized;
    for (size_t i = 0; i < x.size(); i += 4) {
        if (i + 4 > x.size()) return memcmp(&x[i], &exactY[i], x.size() - i) == 0;
        uint32_t wx = (uint32_t)x[i] << 24 | x[i + 1] << 16 | x[i + 2] << 8 | x[i + 3];
        uint32_t wy = (uint32_t)exactY[i] << 24 | exactY[i + 1] << 16 | exactY[i + 2] << 8 | exactY[i + 3];
        bool px = wx - (spx - 0x10000) < 0x10000, py = wy - (spy - 0x10000) < 0x10000;
        if (wx == wy) continue; /* same value: equal whether or not it is a stack address */
        /* NaN payloads of float-stored words (either side) are not compared, as in memory */
        if (!g_strict_nan && is_nan32(wx) && is_nan32(wy) &&
            ((flx.size() > i && flx[i]==1) || (fly.size() > i && fly[i]==1)))
            continue;
        if (px && py) {
            if (wx - bx != wy - by) return false;
        } else if (wx != wy) {
            return false;
        }
    }
    return true;
}

/* first difference between original (a) and candidate (b), "" if equivalent */
static std::string compare(const Result& a, const Result& b, gabi::RetKind rk, const Input& in) {
    char buf[512];
    switch (rk) {
    case gabi::RET_VOID: break;
    case gabi::RET_INT1:
    case gabi::RET_INT2:
    case gabi::RET_INT4: {
        uint32_t m = rk == gabi::RET_INT1 ? 0xFF : rk == gabi::RET_INT2 ? 0xFFFF : 0xFFFFFFFF;
        if ((a.exit.r[3] & m) != (b.exit.r[3] & m)) {
            snprintf(buf, sizeof buf, "return r3: original %08X candidate %08X", a.exit.r[3], b.exit.r[3]);
            return buf;
        }
        break;
    }
    case gabi::RET_PAIR32:
        if (a.exit.r[3] != b.exit.r[3] || a.exit.r[4] != b.exit.r[4]) {
            snprintf(buf, sizeof buf, "return r3:r4 pair32: original %08X:%08X candidate %08X:%08X", a.exit.r[3], a.exit.r[4], b.exit.r[3], b.exit.r[4]);
            return buf;
        }
        break;
    case gabi::RET_AGG6:
        if (a.exit.r[3] != b.exit.r[3] || (a.exit.r[4] & 0xFFFF0000u) != (b.exit.r[4] & 0xFFFF0000u)) {
            snprintf(buf, sizeof buf, "return r3:r4 aggregate6: original %08X:%04X candidate %08X:%04X", a.exit.r[3], a.exit.r[4] >> 16, b.exit.r[3], b.exit.r[4] >> 16);
            return buf;
        }
        break;
    case gabi::RET_FLOAT:
        if (!same_f(a.exit.f[1].ps0, b.exit.f[1].ps0)) {
            snprintf(buf, sizeof buf, "return f1: original %.9g candidate %.9g", a.exit.f[1].ps0, b.exit.f[1].ps0);
            return buf;
        }
        break;
    }
    size_t n = std::min(a.calls.size(), b.calls.size());
    for (size_t k = 0; k < n; k++) {
        const CallEv& x = a.calls[k];
        const CallEv& y = b.calls[k];
        if (x.target != y.target) {
            snprintf(buf, sizeof buf, "call #%zu: original calls %s, candidate %s", k, callee_name(x.target).c_str(),
                     callee_name(y.target).c_str());
            return buf;
        }
        if ((g_calleesp_all || footprint_of(g_cur_func_fp) != UINT32_MAX) && x.sp != y.sp) { /* callees must run at the original's sp */
            snprintf(buf, sizeof buf, "call #%zu %s: callee sp original %08X candidate %08X (footprint)", k,
                     callee_name(x.target).c_str(), x.sp, y.sp);
            return buf;
        }
        const CallEv::Extra &xe = x.ex(), &ye = y.ex();
        if (xe.observed != ye.observed) {
            snprintf(buf, sizeof buf, "call #%zu %s: observed global bytes differ", k, callee_name(x.target).c_str());
            return buf;
        }
        if (xe.fx && ye.fx && g_framexact.count({g_cur_func, xe.lr})) {
            if (xe.lr != ye.lr) {
                snprintf(buf, sizeof buf, "call #%zu %s: LR original %08X candidate %08X (framexact)", k, callee_name(x.target).c_str(), xe.lr, ye.lr);
                return buf;
            }
            for (int i = 0; i < 18; i++) {
                if (xe.nv[i] != ye.nv[i]) {
                    snprintf(buf, sizeof buf, "call #%zu %s: r%d original %08X candidate %08X (framexact)", k, callee_name(x.target).c_str(), i + 14,
                             xe.nv[i], ye.nv[i]);
                    return buf;
                }
                if (xe.fnv[i][0] != ye.fnv[i][0] || xe.fnv[i][1] != ye.fnv[i][1]) {
                    snprintf(buf, sizeof buf, "call #%zu %s: f%d differs (framexact)", k, callee_name(x.target).c_str(), i + 14);
                    return buf;
                }
            }
            if (xe.frame != ye.frame) {
                size_t i = 0;
                while (i < xe.frame.size() && i < ye.frame.size() && xe.frame[i] == ye.frame[i]) i++;
                snprintf(buf, sizeof buf, "call #%zu %s: frame byte sp+0x%zX original %02X candidate %02X (framexact)", k,
                         callee_name(x.target).c_str(), i, i < xe.frame.size() ? xe.frame[i] : 0, i < ye.frame.size() ? ye.frame[i] : 0);
                return buf;
            }
        }
        const CalleeInfo* ci = callee_info(x.target);
        uint32_t im = 0, fm = 0;
        if (ci) { im = ci->imask; fm = ci->fmask; }
        /* an indirect call (bctrl) whose generated target happens to be a listed callee: the listed callee's
         * argument mask does not make dead registers of the original (target/table register, stale after a
         * mock) arguments */
        if (ci && x.kind == VM_CALL_INDIRECT) im &= ~(x.notarg & 0x7F0u);
        if (!ci || ci->declared) {
            for (int i = 0; i < y.nint; i++) im |= 1u << (3 + i);
            for (int i = 0; i < y.nflt; i++) fm |= 1u << (1 + i);
        }
        if (!ci && !g_nolivein.count(g_cur_func)) /* a target the unit does not list (indirect call): also what the resolved callee reads */
            if (auto li = g_livein.find(x.target); li != g_livein.end()) {
                im |= li->second.first & ~x.notarg & 0x7F0u; /* r4..r10; r3 (this) is always the candidate's first argument */
                fm |= li->second.second;
            }
        for (int i = 3; i <= 10; i++) {
            if (!(im >> i & 1)) continue;
            /* explicit `rN=0` fact: equal raw values are equal (a scalar that happens to lie in the stack range, e.g.
             * weather GX2DrawIndexedEx); different stack pointers still compare by pointee as before */
            if (ci && ci->ptrsz[i] == 0xFFFE && x.r[i] == y.r[i]) continue;
            bool sa = !xe.sref[i].empty(), sb = !ye.sref[i].empty();
            if (sa && sb) {
                if (!same_stack_obj(xe.sref[i], x.r[i], a.entry_sp, ye.sref[i], y.r[i], b.entry_sp, xe.sflt[i], ye.sflt[i])) {
                    snprintf(buf, sizeof buf, "call #%zu %s: r%d -> stack data differs", k, callee_name(x.target).c_str(), i);
                    return buf;
                }
            } else if (x.r[i] != y.r[i]) {
                snprintf(buf, sizeof buf, "call #%zu %s: r%d original %08X candidate %08X", k, callee_name(x.target).c_str(), i,
                         x.r[i], y.r[i]);
                return buf;
            }
        }
        for (size_t j = 0; j < std::min(xe.stk.size(), ye.stk.size()); j++) {
            bool sa = !xe.stkref[j].empty(), sb = !ye.stkref[j].empty();
            if (sa && sb ? xe.stkref[j] != ye.stkref[j] : xe.stk[j] != ye.stk[j]) {
                snprintf(buf, sizeof buf, "call #%zu %s: stack argument %zu original %08X candidate %08X", k,
                         callee_name(x.target).c_str(), j, xe.stk[j], ye.stk[j]);
                return buf;
            }
        }
        for (int i = 1; i <= 8; i++) {
            if (!(fm >> i & 1)) continue;
            if (!same_f(x.f[i], y.f[i])) {
                snprintf(buf, sizeof buf, "call #%zu %s: f%d original %.9g candidate %.9g", k, callee_name(x.target).c_str(), i,
                         x.f[i], y.f[i]);
                return buf;
            }
        }
    }
    if (a.calls.size() != b.calls.size()) {
        const CallEv& e = a.calls.size() > n ? a.calls[n] : b.calls[n];
        snprintf(buf, sizeof buf, "call count: original %zu, candidate %zu (first extra: %s by the %s)", a.calls.size(),
                 b.calls.size(), callee_name(e.target).c_str(), a.calls.size() > n ? "original" : "candidate");
        return buf;
    }
    if (a.final_bytes != b.final_bytes) {
        size_t i = 0;
        while (i < a.final_bytes.size() && i < b.final_bytes.size() && a.final_bytes[i] == b.final_bytes[i]) i++;
        snprintf(buf, sizeof buf, "final-state byte #%zu differs (original %02X candidate %02X)", i,
                 i < a.final_bytes.size() ? a.final_bytes[i] : 0, i < b.final_bytes.size() ? b.final_bytes[i] : 0);
        return buf;
    }
    if (g_framexact_funcs.count(g_cur_func))
        for (int i = 14; i <= 31; i++) {
            if (a.exit.r[i] != b.exit.r[i]) {
                snprintf(buf, sizeof buf, "exit r%d: original %08X candidate %08X (framexact)", i, a.exit.r[i], b.exit.r[i]);
                return buf;
            }
            if (memcmp(&a.exit.f[i], &b.exit.f[i], 16) != 0) {
                snprintf(buf, sizeof buf, "exit f%d differs (framexact)", i);
                return buf;
            }
        }
    uint32_t wdiff = 0;
    if (!same_net(a.net, b.net, &wdiff, a.fwords, b.fwords, a.dwords, b.dwords)) {
        uint32_t ea = wdiff;
        for (uint32_t k = 0; k < 4; k++) {
            auto fa = a.net.find(wdiff + k), fb = b.net.find(wdiff + k);
            bool sa = fa != a.net.end(), sb = fb != b.net.end();
            if (sa != sb || (sa && fa->second != fb->second)) { ea = wdiff + k; break; }
        }
        auto fa = a.net.find(ea), fb = b.net.find(ea);
        snprintf(buf, sizeof buf, "memory %08X: original %s, candidate %s (%zu vs %zu bytes changed)", ea,
                 fa == a.net.end() ? "unchanged" : hex32(fa->second).substr(6).c_str(),
                 fb == b.net.end() ? "unchanged" : hex32(fb->second).substr(6).c_str(), a.net.size(), b.net.size());
        return buf;
    }
    (void)in;
    return "";
}

/* the replay of the original must reproduce the recording, else the input is unusable */
static std::string replay_check(const Result& a, const Input& in) {
    char buf[256];
    if (a.stray) { snprintf(buf, sizeof buf, "replay read unrecorded memory (%u bytes, first %08X)", a.stray, a.first_stray); return buf; }
    if (a.calls.size() != in.calls.size()) { snprintf(buf, sizeof buf, "replay made %zu calls, recording %zu", a.calls.size(), in.calls.size()); return buf; }
    for (size_t k = 0; k < a.calls.size(); k++)
        if (a.calls[k].target != in.calls[k].target) { snprintf(buf, sizeof buf, "replay call #%zu differs", k); return buf; }
    if (a.net != in.rec_net) { snprintf(buf, sizeof buf, "replay writes differ from the recording (%zu vs %zu bytes)", a.net.size(), in.rec_net.size()); return buf; }
    if (a.exit.r[3] != in.rec_exit.r[3] || !same_f(a.exit.f[1].ps0, in.rec_exit.ps0[1])) return "replay return value differs";
    return "";
}

/* ------------------------------------------------------------------ main */
/* a host crash inside the code under test (e.g. a candidate dereferencing a host pointer): name the function and
 * the side before dying, instead of exiting silently with rc 139 */
static void crash_handler(int sig) {
    char buf[160];
    int n = snprintf(buf, sizeof buf, "\nverify: host signal %d while running %s side of %08X (g_cur_func); input aborted\n",
                     sig, V.is_candidate ? "the candidate" : "the original", g_cur_func);
    if (n > 0) { ssize_t w = write(2, buf, (size_t)n); (void)w; }
    signal(sig, SIG_DFL);
    raise(sig);
}

int main(int argc, char** argv) {
    signal(SIGSEGV, crash_handler);
    signal(SIGBUS, crash_handler);
    int ngen = 1000, verbose = 0;
    uint64_t seed0 = 1;
    const char* recdir = nullptr;
    const char* image = "build/verify/image.bin";
    std::set<uint32_t> only;  /* -only ADDR[,ADDR...]: test these functions only (shards of a heavy unit) */
    bool list_only = false;   /* -list: print the unit's candidate addresses and exit */
    int range_lo = 0, range_hi = -1; /* -range A:B: generated inputs A..B-1 only (recorded inputs only when A == 0) */
    bool shardout = false;    /* -shardout: machine-readable per-function results for verify.py -shards to merge */
    uint64_t trace_seed = 0;
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-n") && i + 1 < argc) ngen = atoi(argv[++i]);
        else if (!strcmp(argv[i], "-seed") && i + 1 < argc) seed0 = strtoull(argv[++i], nullptr, 0);
        else if (!strcmp(argv[i], "-rec") && i + 1 < argc) recdir = argv[++i];
        else if (!strcmp(argv[i], "-image") && i + 1 < argc) image = argv[++i];
        else if (!strcmp(argv[i], "-only") && i + 1 < argc) {
            for (char* p = argv[++i]; *p;) {
                char* e;
                uint32_t a = (uint32_t)strtoul(p, &e, 16);
                if (e == p) { fprintf(stderr, "bad -only list %s\n", argv[i]); return 2; }
                only.insert(a);
                p = *e == ',' ? e + 1 : e;
            }
        }
        else if (!strcmp(argv[i], "-list")) list_only = true;
        else if (!strcmp(argv[i], "-range") && i + 1 < argc) { if (sscanf(argv[++i], "%d:%d", &range_lo, &range_hi) != 2) { fprintf(stderr, "bad -range\n"); return 2; } }
        else if (!strcmp(argv[i], "-shardout")) shardout = true;
        else if (!strcmp(argv[i], "-spec") && i + 1 < argc) load_specs(argv[++i]);
        else if (!strcmp(argv[i], "-v")) verbose++;
        else if (!strcmp(argv[i], "-noclobber")) V.clobber = false;
        else if (!strcmp(argv[i], "-strictnan")) g_strict_nan = true;
        else if (!strcmp(argv[i], "-calleesp")) g_calleesp_all = true;
        else if (!strcmp(argv[i], "-footprint")) { g_footprint_all = true; g_calleesp_all = true; }
        else if (!strcmp(argv[i], "-trace") && i + 1 < argc) trace_seed = strtoull(argv[++i], nullptr, 0);
        else { fprintf(stderr, "unknown option %s\n", argv[i]); return 2; }
    }
    load_image(image);

    std::vector<gabi::Candidate*> cands;
    for (gabi::Candidate* c = gabi::g_cands; c; c = c->next) cands.push_back(c);
    std::sort(cands.begin(), cands.end(), [](auto* a, auto* b) { return a->addr < b->addr; });
    if (list_only) {
        for (gabi::Candidate* c : cands) printf("%08X\n", c->addr);
        return 0;
    }

    int total_fail = 0;
    if (!shardout)
        printf("%-8s  %-40s %10s %10s %8s  %s\n", "addr", "function", "generated", "recorded", "coverage", "first difference");
    for (gabi::Candidate* cand : cands) {
        if (!only.empty() && !only.count(cand->addr)) continue;
        const OrigFunc* of = nullptr;
        for (unsigned i = 0; i < unit_nfuncs; i++)
            if (unit_funcs[i].addr == cand->addr) of = &unit_funcs[i];
        if (!of) { printf("%08X  %-40s  no original in unit\n", cand->addr, cand->name); total_fail++; continue; }
        std::vector<uint32_t> cov(of->nblocks + 1);
        g_cur_func = cand->addr;
        g_cur_func_fp = cand->addr;
        int gpass = 0, gn = 0, rpass = 0, rn = 0, rbad = 0, aborted = 0;
        std::string first;
        long first_idx = -1, cur_idx = 0; /* input index of the first difference (generated i, then ngen + recorded k) */
        /* a function whose result the original's callers use (ret_values.tsv): the register they read
         * is compared at return whatever the candidate declares (void, or the wrong register class:
         * game test, mDoAud_tact_getBeatFrames 025E1F08 declared u32 but the original returns f1) */
        gabi::RetKind eff_ret = cand->ret;
        if (of->retregs) {
            gabi::RetKind tk = (of->retregs & 5) == 5 ? gabi::RET_PAIR32
                             : (of->retregs & 2) ? gabi::RET_FLOAT
                             : of->retbits == 8 ? gabi::RET_INT1 : of->retbits == 16 ? gabi::RET_INT2 : gabi::RET_INT4;
            auto cls = [](gabi::RetKind k) { return k == gabi::RET_VOID ? 0 : k == gabi::RET_FLOAT ? 2 : (k == gabi::RET_PAIR32 || k == gabi::RET_AGG6) ? 3 : 1; };
            int dc = cls(eff_ret), tc = cls(tk);
            if (dc != tc && !(dc == 3 && tc == 1)) eff_ret = tk;
        }
        auto one = [&](Input& in, bool rec) {
            Result a, b;
            run(of->fn, in, false, &a, &cov);
            if (rec) {
                std::string why = replay_check(a, in);
                if (!why.empty()) {
                    rbad++;
                    if (verbose) printf("    unusable %s: %s\n", in.label.c_str(), why.c_str());
                    return;
                }
            }
            run(cand->fn, in, true, &b, nullptr);
            if (a.aborted || b.aborted) {
                if (a.aborted && b.aborted) { aborted++; return; }
            }
            std::string d = a.aborted != b.aborted ? std::string("only one side aborted: ") + (a.aborted ? a.why : b.why)
                                                   : compare(a, b, eff_ret, in);
            if (!d.empty() && b.stray && rec) d += " [candidate read unrecorded memory]";
            (rec ? rn : gn)++;
            if (d.empty()) (rec ? rpass : gpass)++;
            else {
                if (first.empty()) { first = d + "  {" + in.label + "}"; first_idx = cur_idx; }
                if (verbose) printf("    FAIL %s: %s\n", in.label.c_str(), d.c_str());
                if (verbose > 2) { /* full net effects of both sides */
                    for (auto& kv : a.net) printf("      orig %08X=%02X%s\n", kv.first, kv.second, b.net.count(kv.first) && b.net.at(kv.first) == kv.second ? "" : "  <");
                    for (auto& kv : b.net) printf("      cand %08X=%02X%s\n", kv.first, kv.second, a.net.count(kv.first) && a.net.at(kv.first) == kv.second ? "" : "  <");
                }
            }
        };
        /* dictionary of the original's .rodata constants (pre-runs, not compared) */
        V.dict.clear();
        g_dict = nullptr;
        V.collect = true;
        for (int i = 0; i < 8 && ngen; i++) {
            clear_all();
            Input in;
            make_generated(of, seed0 * 7777ull + i, &in);
            V.seed = seed0 * 7777ull + i;
            Result a;
            run(of->fn, in, false, &a, nullptr);
        }
        V.collect = false;
        g_dict = &V.dict;
        int glo = range_lo, ghi = range_hi < 0 ? ngen : std::min(range_hi, ngen);
        for (int i = glo; i < ghi; i++) {
            cur_idx = i;
            clear_all();
            Input in;
            make_generated(of, seed0 * 1000003ull + i, &in);
            V.seed = seed0 * 1000003ull + i;
            V.clobber_now = i % 2 == 0;
            g_trace = V.seed == trace_seed;
            if (g_trace) printf("    trace %s (clobber %d)\n", in.label.c_str(), V.clobber_now);
            one(in, false);
            g_trace = 0;
        }
        g_dict = nullptr;
        if (recdir && range_lo == 0) {
            std::string d = std::string(recdir) + "/" + hex32(cand->addr);
            if (DIR* dir = opendir(d.c_str())) {
                std::vector<std::string> files;
                while (dirent* e = readdir(dir))
                    if (strstr(e->d_name, ".tap")) files.push_back(d + "/" + e->d_name);
                closedir(dir);
                std::sort(files.begin(), files.end());
                long ridx = 0;
                for (auto& fpath : files) {
                    cur_idx = ngen + ridx++;
                    clear_all();
                    Input in;
                    std::string why;
                    V.seed = 77;
                    if (!load_recorded(fpath.c_str(), &in, &why)) {
                        rbad++;
                        if (verbose) printf("    unusable %s: %s\n", fpath.c_str(), why.c_str());
                        continue;
                    }
                    one(in, true);
                }
            }
        }
        uint32_t hit = 0;
        for (uint32_t k = 0; k < of->nblocks; k++) hit += cov[k] > 0;
        if (shardout) { /* @SHARD addr gpass gn rpass rn rbad aborted first_idx nblocks covered-blocks | name | first */
            printf("@SHARD %08X %d %d %d %d %d %d %ld %u ", cand->addr, gpass, gn, rpass, rn, rbad, aborted, first_idx, of->nblocks);
            bool any = false;
            for (uint32_t k = 0; k < of->nblocks; k++) if (cov[k]) { printf(any ? ",%u" : "%u", k); any = true; }
            printf("%s|%s|%s\n", any ? "" : "-", cand->name, first.c_str());
            continue;
        }
        char g[32], r[32], cv[32];
        snprintf(g, sizeof g, "%d/%d", gpass, gn);
        snprintf(r, sizeof r, rbad ? "%d/%d(-%d)" : "%d/%d", rpass, rn, rbad);
        snprintf(cv, sizeof cv, "%u/%u", hit, of->nblocks);
        bool ok = gpass == gn && rpass == rn && (gn + rn) > 0;
        if (!ok) total_fail++;
        printf("%08X  %-40.40s %10s %10s %8s  %s%s\n", cand->addr, cand->name, g, r, cv, ok ? "ok" : first.c_str(),
               aborted ? " (some inputs aborted on both sides)" : "");
        if (verbose > 1) {
            printf("    uncovered blocks:");
            for (uint32_t k = 0; k < of->nblocks; k++) if (!cov[k]) printf(" %u", k);
            printf("\n");
        }
    }
    if (!g_move_specs.empty() || !g_utf16_specs.empty()) {
        fprintf(stderr, "semantic effects: move calls=%llu bytes=%llu, utf16 failure=%llu lengths=",
                (unsigned long long)g_move_calls, (unsigned long long)g_move_bytes,
                (unsigned long long)g_utf16_fail);
        for (unsigned i = 0; i <= 4096; i++)
            if (g_utf16_length[i]) fprintf(stderr, "%u:%llu,", i, (unsigned long long)g_utf16_length[i]);
        fprintf(stderr, "\n");
    }
    return total_fail ? 1 : 0;
}
