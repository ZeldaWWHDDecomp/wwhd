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
#include <array>
#include <memory>
#include <iterator>
#include <cinttypes>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <map>
#include <tuple>
#include <set>
#include <csignal>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <cerrno>
#include <fcntl.h>
#include "native_frames.cpp" /* gmem_frame_info: build/verify/frames.tsv (frames_all.py) */
#include "blobfile.h"        /* blobfile directive: Yaz0 / SARC / BFRES chains */
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <dirent.h>
#include <strings.h>
#include <dlfcn.h>
#include <signal.h>
#include <mach/mach.h> /* task_info: the host memory budget (resident size) */
#include <sys/ucontext.h>
#include <malloc/malloc.h>
#include <mach-o/dyld.h> /* also -listfn: candidate entry addresses as link-map addresses (verify.py result cache) */
#include <pthread.h>

#include "gabi.h"
#include "unit.h"
#include "verify_tap.h"
#include "vm.h"
#include "gthr_model.h" /* `osmem ... threads`: the guest-thread object model */
#include "ax_model.h"   /* `osmem ... ax`: the snd_core / AX frame model */
#include "pad_model.h"  /* `osmem ... pad`: the GamePad / Pro Controller reads from a press schedule */
#include "dynload_model.h" /* `osmem ... act` / `dynload`: nn::act, OSDynLoad and the erreula exports */
#include "portans_model.h" /* `osmem ... portconst`: constant answers as the port's HLE gives them */

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
/* Host memory per touched page: arena blocks are
 * 16-byte aligned (64 before: a 136-byte chunk took 192), the clobber epochs are 16-bit offsets from a per-chunk base (128
 * bytes instead of 256; a chunk whose epochs do not fit widens to 32 bits, exact either way: ep_get/ep_set), and the float
 * marks of a page live in a side table allocated by the first mark (most pages have none). */
struct Chunk {
    uint8_t d[64];
    uint8_t init[64];
    void* ep;         /* nullptr: every byte current for epoch 0; else 64 x uint16_t (epw == 0: 0 = epoch 0, v = epbase + v - 1)
                       * or 64 x uint32_t (epw == 1: >65535 mocked calls must not wrap) */
    uint32_t epbase;  /* epw == 0: the epoch of offset 1 */
    uint8_t epw;      /* 1: ep holds 32-bit epochs */
    uint8_t epclean;  /* 1: every 16-bit entry is 0 (epbase may be chosen anew) */
};
struct TPage;
struct Page {
    uint64_t have[64];
    uint64_t wr[64];
    /* float marks (side table, nullptr = none): fm[0..63] byte addresses that start an f32 word whose last store was a float
     * store (was V.fwords), fm[64..127] f64 starts (was V.dwords). Arena-allocated, except a template page's own table (TPage::fm) */
    uint64_t* fm;
    Chunk* ch[64];
    /* -fastreset (2026-10-28): `dirty` = the page is in V.touched (every page is, except a template page that still holds
     * its template state); `t` = the page's template (setupsnapshot state) or nullptr; `pn` = page number */
    uint32_t pn;
    uint8_t dirty;
    uint32_t run; /* page budget: the run (V.run_serial) that last touched this page */
    const struct TPage* t;
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

/* Large per-run buffers straight from the kernel (2026-11-02). A run's call records can reach millions (lighting_lightfix
 * 0276C8B8: 28M top-level calls under the spec's callbudget, 160 bytes each). Through malloc, the vector's doubling growth peaked
 * at ~2.5x the records, and libmalloc kept every freed large block in its large-entry cache, still dirty: vmmap at the next
 * function showed "MALLOC_LARGE (empty)" 5.5 GB (heap -s: 66 MB live), so the process stayed at 1-6 GB through every later
 * function. BigAlloc maps blocks of 1 MB and more with mmap and returns them with munmap, so freed memory leaves the process
 * at once; smaller blocks use operator new as before. */
template <class T> struct BigAlloc {
    using value_type = T;
    static constexpr size_t kBig = 1u << 20;
    BigAlloc() = default;
    template <class U> BigAlloc(const BigAlloc<U>&) {}
    T* allocate(size_t n) {
        size_t b = n * sizeof(T);
        if (b < kBig) return static_cast<T*>(::operator new(b));
        void* p = mmap(nullptr, b, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
        if (p == MAP_FAILED) throw std::bad_alloc();
        return static_cast<T*>(p);
    }
    void deallocate(T* p, size_t n) {
        size_t b = n * sizeof(T);
        if (b < kBig) ::operator delete(p);
        else munmap(p, b);
    }
    template <class U> bool operator==(const BigAlloc<U>&) const { return true; }
    template <class U> bool operator!=(const BigAlloc<U>&) const { return false; }
};
template <class T> using BigVec = std::vector<T, BigAlloc<T>>;

struct CallEv {
    uint32_t target;
    int kind;
    uint32_t r[11];
    double f[9];
    int nint, nflt;                 /* candidate: declared arguments */
    uint32_t sp = 0;                /* r1 the callee starts with (compared under `footprint`) */
    uint32_t lr = 0;                /* the return address the call passes (the original's bl; LR fix 2026-10-12) */
    uint32_t notarg = 0;            /* original: registers that cannot be arguments of an unlisted target (target/table register, stale after a mock) */
    /* the rarely needed parts live in a separately allocated block, only when present (2026-10-06: a call
     * record was ~1.2 KB of mostly empty vectors; pathological inputs record millions of calls) */
    struct Extra {
        std::vector<uint32_t> stk;      /* stack argument words (sp+8...) */
        std::vector<uint8_t> stkref[4]; /* pointee bytes when a stack argument points into the stack */
        std::vector<uint8_t> sref[11];  /* pointee bytes of stack pointer arguments */
        std::vector<uint8_t> href[11];  /* pointee bytes of heap pointer arguments with an rN=heapSIZE fact */
        std::vector<uint8_t> sflt[11];  /* per byte: covered by a float store (NaN payload tolerance) */
        std::vector<std::pair<uint32_t, std::vector<uint8_t>>> observed; /* selected globals at call entry */
        /* per-site / conditional observearg spans: (index into the target's observearg/observeptr list, bytes);
         * the condition is decided at compare time on the original's call */
        std::vector<std::pair<uint32_t, std::vector<uint8_t>>> sobserved;
        /* `observenested`: the observed calls made inside this top-level call (inside `real` callees), in call order:
         * (target, for each of the target's spans a flag byte 1/0 (bytes present / null base) followed by the bytes) */
        std::vector<std::pair<uint32_t, std::vector<uint8_t>>> nested;
        uint32_t crmask = 0, crbits = 0;   /* `callcr`: CR bits at call entry (bit b of crbits = cr[b]) */
        /* `framexact FUNC RET`: the call as a save state at the frame boundary sees it */
        bool fx = false;
        uint32_t lr = 0;
        uint32_t nv[18] = {};        /* r14..r31 */
        uint64_t fnv[18][2] = {};    /* f14..f31 ps0/ps1 bits */
        std::vector<uint8_t> frame;  /* [entry sp - frame size, entry sp + 8) */
        /* -observe-pointees: the object an undeclared heap/static pointer argument points into: its start and full size,
         * kind (1 heap block, 2 static object), and the bytes captured from pfrom on (at most the cap) */
        uint32_t pbase[11] = {}, psize[11] = {}, pfrom[11] = {};
        uint8_t pkind[11] = {};
        std::vector<uint8_t> pref[11];
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
    BigVec<CallEv> calls;
    std::unordered_map<uint32_t, uint32_t> per_target;
    std::unordered_map<uint32_t, uint32_t> obs_ord; /* top-level calls per observearg/observeptr target so far (`TARGET@N`) */
    bool fany = false; /* any fw/dw mark set in this run */
    uint32_t fcopy[8];          /* NaN values recently loaded (32-bit) from float-stored words: an integer */
    unsigned nfcopy;            /* copy of such a word keeps its float tag (GHS struct copies) */
    int depth = 0;
    bool is_candidate = false;
    uint64_t budget = 0;
    uint64_t npages = 0;  /* distinct guest pages this run touched (page budget) */
    uint32_t run_serial = 0; /* counts runs; Page::run marks the pages the current run touched */
    uint64_t ncalls = 0;  /* guest calls in this run, top level and nested (call budget) */
    int stop_count = 0;         /* `stopat`: top-level calls to the terminal target so far in this run */
    jmp_buf abort_jmp;
    bool can_abort = false;
    std::string abort_reason;
    /* model_reject(): the mock model that rejected this run's last call */
    bool mrej = false;
    uint32_t mrej_target = 0, mrej_in = 0;
    int mrej_depth = 0;
    std::vector<uint32_t> real_chain; /* `real` callees being executed (outermost first), printed by -trace on an abort */
    uint32_t stray = 0, first_stray = 0;
    bool p0 = false;       /* -p0exclude: the original's run touched page 0 (see g_p0exclude) */
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
    BigVec<CallEv>* orig_calls = nullptr; /* for translating stack patches to the candidate */
    std::map<uint32_t, uint32_t> locals;       /* candidate: live gabi::Local slots (address -> 16-byte-aligned slot size) */
    std::map<uint32_t, uint32_t> local_objsz;  /* candidate: the same Locals' sizeof(T) */
    std::vector<std::pair<uint32_t, uint32_t>> reserved; /* candidate: native frames (gmem_stack_reserve) */
    uint32_t post_r[13]; bool have_post = false; /* volatile registers as a generated mock left them */
    /* stack guard (candidate side): harness-internal stack writes in progress; the candidate's SP
     * before the outgoing frame of the guest call that is running (0: none) */
    int stack_internal = 0;
    uint32_t call_top = 0;
    std::unordered_set<uint32_t> orig_stack; /* guarded-range bytes the original wrote for this input */
    BigVec<std::pair<uint32_t, int>> orig_seq; /* the original's top-level calls for this input (target, kind) */
    BigVec<uint32_t> orig_lr;  /* ... and the return address each of them passed */
} V;
/* `osmem ... threads` (gthr_sched.inc), declared early for the memory accessors and the osm namespace */
namespace gthr {
extern bool on;
extern uint32_t main_obj, cap;
extern uint64_t sched_seed, clock_origin, budget, slice_len, clock_rate, log_cap;
extern bool drain_on;
void tested_reset();
void tested_starts();
extern int64_t os_bias;
static const uint32_t kAxHostLo = 0x6FE00000u, kAxHostHi = 0x6FE40000u; /* `ax`: host_alloc stand-in region */
static const uint32_t kMainCore = 1, kMainPrio = 16, kMainStackTop = 0x7EFFF000u /* kGenSp */, kMainStackSize = 0x10000u,
                      kRpxEntry = 0x028EA120u;
extern int g_multi;
void slice_hit();
void announce();
int current_id();
bool models(uint32_t t);
bool call(Cpu* c, uint32_t target, const uint32_t* a);
uint64_t clock_now(); /* the model's virtual tick clock (`portmem`) */
bool ax_call(Cpu* c, uint32_t target, const uint32_t* a);
bool in_thread_stack(uint32_t ea);
}
/* constructor sizes (tools/verify/ctor_sizes.tsv, from sizes.py): a candidate gabi::Local passed as
 * `this` to a constructor must hold the whole object, or the constructor writes past it into the
 * caller's frame (game test: daSail_packet_c::draw, 0xF0-byte Local for a 0x11C-byte object) */
static std::unordered_map<uint32_t, uint32_t> g_ctor_size;
/* mockfield TARGET rN+OFF TYPE v1,v2,... | LO HI : a generated mock of TARGET writes a chosen value into the
 * object its argument rN points to (both sides alike, per call), so coherent output fixtures can be built
 * without making the callee real. TYPE u8/s8/u16/s16/u32/s32/f32. */
/* `[rN+OFF]+OFF2` in place of rN+OFF: the value goes through the pointer stored at rN+OFF (a descriptor whose first
 * word points to the caller's loop index; AGL-GX2#3 request, 2026-10-23); a null pointer is skipped. */
struct MockField { int reg; uint32_t off; int size; bool is_float; std::vector<double> pick; double lo, hi; bool deref = false; uint32_t off2 = 0; };
static std::unordered_map<uint32_t, std::vector<MockField>> g_mockfield;
/* live-in argument registers of every function (build/verify/livein.tsv, livein_all.py): at a call to a target
 * the unit does not list (indirect call), compare what the resolved callee reads, not only what the candidate
 * passes (game test 2026-10-04: an isKindOf vcall without its r4 argument went unnoticed) */
static std::unordered_map<uint32_t, std::pair<uint32_t, uint32_t>> g_livein;
static std::set<uint32_t> g_nolivein; /* unit `nolivein FUNC`: documented false positives of the live-in check */
/* `stopat FUNC TARGET N`: FUNC does not return (a thread loop); each run of FUNC ends at its N-th top-level call to the
 * mocked TARGET (recorded and compared like every call; its return is not simulated). Both sides must stop there; then the
 * call sequence and the net memory effect up to that point are compared, the exit registers are not. A run that stops on
 * one side only differs. 2026-10-21, for JASTaskThread::run 02810B9C. */
/* `stopat FUNC TARGET N live REG...`: the named GPRs/FPRs (r0..r31,
 * f0..f31; an FPR compares ps0 and ps1) are compared at the terminal call on both sides, for a value the original keeps live
 * past the stop (e.g. f13 read by the suffix). Without `live` nothing changes. */
struct StopSpec { uint32_t target; int n; uint32_t live_r = 0, live_f = 0; };
static std::map<uint32_t, StopSpec> g_stop_specs;

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

/* -fastreset: the input-independent leading `fill`/`copy` lines of the current function (absolute addresses), built
 * once per function instead of per input (AGL world: a 16 MB heap fill and a 16 MB blob, ~37 MB of SnapPages per
 * input). V.snap holds the per-input rest and wins over it, as the later lines did when both were in V.snap.
 * nullptr outside generated inputs (clear_all resets it; make_generated sets it). */
static std::unordered_map<uint32_t, SnapPage*>* g_snap_base = nullptr;
[[gnu::always_inline]] static inline const SnapPage* snap_page_of(uint32_t ea, uint32_t o) { /* the layer holding the
                                                                                              * byte's initial value, or nullptr */
    if (!V.snap.empty()) {
        auto it = V.snap.find(ea >> 12);
        if (it != V.snap.end() && (it->second->has[o >> 6] >> (o & 63) & 1)) return it->second;
    }
    if (__builtin_expect(g_snap_base != nullptr, 0)) {
        auto jt = g_snap_base->find(ea >> 12);
        if (jt != g_snap_base->end() && (jt->second->has[o >> 6] >> (o & 63) & 1)) return jt->second;
    }
    return nullptr;
}

/* `osmem` (namespace osm): ranges whose untouched bytes start as zero, as the port's zero-initialised guest memory
 * (sorted, disjoint [lo, hi) pairs; empty unless the directive is given) */
static std::vector<std::pair<uint32_t, uint32_t>> g_zero_init;
/* Capture route.
 * `capture NAME SHA256` + `capturemap ADDR SIZE` + `capturecall SHA256`: guest memory captured from the plain port at the tested
 * function's entry (port debug hook WWHD_GUEST_CAPTURE). The files are private game data, outside every repo, under
 * $WWHD_CAPTURE_STORE (default build/capture, git-ignored); the spec names them by SHA-256 only.
 *  - Only bytes inside a `capturemap` window take their initial value from the capture (a page the port did not hold is zero);
 *    every other byte keeps the normal rule (image, stack, zero, generated).
 *  - .rodata and the port-runtime regions are refused as windows (kOff). Diagnostic exception: with WWHD_CAPTURE_READLOG set
 *    they may be mapped, to measure the read set.
 *  - Nothing on top: a spec with `capture` refuses setup, postsetup, poke, field, fieldstr, fill, copy, blob.
 *  - `capturecall SHA256`: the manifest capture.txt beside the capture, hash-checked: its `func` must be the function of the spec's
 *    `arg!` lines and each of those must equal the captured register (the drift guard).
 *  - No clobbering in a capture run: it is one real state, not a family of hypothetical callees (clobberable).
 * Diagnostics: WWHD_CAPTURE_READLOG=FILE appends the capture-served bytes guest code read; WWHD_CAPTURE_GENLOG=FILE appends every
 * generated (known = false) byte guest code read, which the closed-read-set check needs to be empty. */
namespace capm {
static bool present = false;
static std::string dir;                                    /* directory of the capture file */
static std::vector<uint8_t> file;                          /* the kept pages of the file */
static std::unordered_map<uint32_t, const uint8_t*> pages; /* 4 KB page -> its bytes (absent = zero) */
static std::vector<std::pair<uint32_t, uint32_t>> map;     /* sorted [lo, hi) */
static std::set<uint32_t>* readlog = nullptr;
static std::set<uint32_t>* genlog = nullptr;
static uint32_t call_func = 0;
static std::map<int, uint32_t> call_regs;
static const struct { uint32_t lo, hi; const char* what; } kOff[] = {
    {0x10000000u, 0x1018C0C0u, ".rodata (the image serves it)"},
    {0x60000000u, 0x70000000u, "the port's runtime region"},
    {0xC0000000u, 0xC2000000u, "the import and shared-data slots"},
    {0xE0000000u, 0xE2800000u, "the foreground bucket"},
    {0xF4000000u, 0xF6000000u, "MEM1"},
};
static inline bool mapped(uint32_t ea) {
    if (map.empty()) return false;
    auto it = std::upper_bound(map.begin(), map.end(), std::make_pair(ea, 0xFFFFFFFFu));
    return it != map.begin() && ea < std::prev(it)->second;
}
static inline uint8_t byte(uint32_t ea) {
    auto it = pages.find(ea & ~0xFFFu);
    return it == pages.end() ? 0 : it->second[ea & 0xFFF];
}
static void dump_runs(const std::set<uint32_t>& set, const std::string& path) {
    FILE* o = fopen(path.c_str(), "a");
    if (!o) return;
    uint32_t lo = 0, hi = 0; bool open = false; /* coalesced [lo, hi) runs */
    for (uint32_t a : set) {
        if (open && a == hi) { hi++; continue; }
        if (open) fprintf(o, "%08X %X\n", lo, hi - lo);
        lo = a; hi = a + 1; open = true;
    }
    if (open) fprintf(o, "%08X %X\n", lo, hi - lo);
    fclose(o);
}
}
static uint8_t initial_byte(uint32_t ea, bool* known) {
    if (const SnapPage* sp = snap_page_of(ea, ea & 0xFFF)) { *known = true; return sp->d[ea & 0xFFF]; }
    if (__builtin_expect(!capm::map.empty(), 0) && capm::mapped(ea)) { *known = true; return capm::byte(ea); }
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
    if (__builtin_expect(!g_zero_init.empty(), 0)) {
        auto it = std::upper_bound(g_zero_init.begin(), g_zero_init.end(), std::make_pair(ea, 0xFFFFFFFFu));
        if (it != g_zero_init.begin() && ea < std::prev(it)->second) { *known = true; return 0; }
    }
    *known = false;
    uint32_t w = typed_word(hash3(V.seed, ea & ~3u, 0x5EED));
    return (uint8_t)(w >> (24 - 8 * (ea & 3)));
}

/* bump arena for pages, chunks, epoch arrays and float-mark tables: zero-filled chunks from mmap. After every input the first
 * chunk is kept (its used part zeroed again, so normal inputs never map/unmap) and every further chunk is
 * unmapped, so a pathological input does not leave its high-water mark behind. */
static std::vector<std::pair<void*, size_t>> g_arena_chunks;
static uint8_t* g_arena_cur = nullptr;
static size_t g_arena_left = 0;
static void* arena_zalloc(size_t n) {
    n = (n + 15) & ~(size_t)15; /* 16-byte blocks */
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

static struct { uint32_t pn; Page* p; } g_pcache[256];
/* a template page (-fastreset) leaves its template state: from now on it is in V.touched like every other page.
 * g_pcache holds only pages in V.touched (fast_reset() and clear_all() empty it when they restore template pages), so a
 * cache hit needs no check */
static void page_dirty(Page* p) {
    p->dirty = 1;
    V.touched.push_back(p);
}
/* Page budget (2026-10-31, AGL#8 02794270 without -nulltrap): a run whose original walks a NULL-based pointer chain through
 * readable page 0 strides across the address space and touches a new guest page every ~32 accesses, so its access
 * budget (20M) allows ~600k pages, about 7 KB of host memory each (about 10 GB per process), built and freed per input.
 * Every run (setup, pokes, function; each side) may touch at most this many distinct guest pages (default 32768 = a
 * 128 MB span; the largest legitimate run measured on 2026-10-31 touches 6150). One more aborts the input on the side
 * that ran, like the access, call and time budgets (an abort never counts as a detected difference). The count is per
 * run, not per allocation, so both sides of an input count alike although the candidate reuses the pages the
 * original's run created. A harness-internal read (g_in_peek) is counted but never aborts; the next guest access does.
 * `pagebudget N` (unit file) raises it. */
static uint64_t g_page_budget = 32768;
static int g_in_peek = 0; /* harness-internal reads (peek8: argument capture, vtable scans, mock models) are not guest accesses */
/* Host memory budget (2026-11-01: AGL#16 027C0CEC #5/#56 and AGL#14 archive_members_synthetic #2 grew
 * the harness past the 3 GB RSS watchdog, which killed the whole process, so those mutants left no receipt). The process's
 * resident size is sampled at the run's safe points (every 65536 guest accesses, every 256 newly touched guest pages) and
 * while net_writes() collects the run's net effect (every 65536 entries: AGL#14 #2 at seed 7 left 54M written bytes, a
 * 2.6 GB std::map then, ~430 MB as a NetMap, after its access-budget abort);
 * past the budget the input aborts on the side that ran, like the access, call, page and time budgets (an abort never
 * counts as a detected difference), with the reason `host memory budget exceeded (RSS N MB > B MB in one run)`.
 * Default 2816 MB (under the 3072 MB watchdog); `hostmembudget MB` (unit file, 64..65536) sets it, the environment
 * WWHD_VERIFY_HOSTMEM_MB overrides both (0 = off). RSS depends on the process's history, so such an abort is treated like
 * a time limit (never cached as the original's result: is_env_limit). Host code that allocates without guest accesses
 * is not sampled (the watchdog stays the backstop there). */
static bool g_hostmem_env = getenv("WWHD_VERIFY_HOSTMEM_MB") && *getenv("WWHD_VERIFY_HOSTMEM_MB"); /* the directive does not override it */
static uint64_t g_hostmem_budget_mb = [] {
    const char* e = getenv("WWHD_VERIFY_HOSTMEM_MB");
    if (!e || !*e) return (uint64_t)2816;
    char* end;
    unsigned long long v = strtoull(e, &end, 10);
    if (*end || (v && (v < 64ull || v > 65536ull))) {
        fprintf(stderr, "invalid WWHD_VERIFY_HOSTMEM_MB=%s (0 = off, or 64..65536)\n", e);
        exit(2);
    }
    return (uint64_t)v;
}();
static uint64_t host_rss_mb() {
    mach_task_basic_info_data_t info;
    mach_msg_type_number_t n = MACH_TASK_BASIC_INFO_COUNT;
    if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&info, &n) != KERN_SUCCESS) return 0;
    return (uint64_t)info.resident_size >> 20;
}
[[gnu::cold, gnu::noinline]] static void hostmem_check() {
    if (!g_hostmem_budget_mb || !V.can_abort || g_in_peek) return;
    uint64_t mb = host_rss_mb();
    if (mb > g_hostmem_budget_mb) {
        V.abort_reason = "host memory budget exceeded (RSS " + std::to_string(mb) + " MB > " +
                         std::to_string(g_hostmem_budget_mb) + " MB in one run)";
        longjmp(V.abort_jmp, 1);
    }
}
[[gnu::noinline]] static void page_note(Page* p) { /* first touch of p in this run */
    p->run = V.run_serial;
    if (++V.npages > g_page_budget && V.can_abort && !g_in_peek) {
        V.abort_reason = "page budget exceeded (" + std::to_string(g_page_budget) + " guest pages touched in one run)";
        longjmp(V.abort_jmp, 1);
    }
    if ((V.npages & 0xFF) == 0) hostmem_check();
}
[[gnu::always_inline]] static inline Page* page_peek(uint32_t pn) { /* nullptr when the page was never touched */
    auto& ce = g_pcache[pn & 255];
    if (ce.p && ce.pn == pn) return ce.p; /* cached: touched in this run already (run() empties the cache) */
    auto it = V.pages.find(pn);
    if (it == V.pages.end()) return nullptr;
    ce.pn = pn; ce.p = it->second;
    if (!ce.p->dirty) page_dirty(ce.p);
    if (ce.p->run != V.run_serial) page_note(ce.p);
    return it->second;
}
static Page* page(uint32_t pn) {
    if (Page* q = page_peek(pn)) return q;
    Page* p = (Page*)arena_zalloc(sizeof(Page));
    p->pn = pn;
    V.pages[pn] = p;
    page_dirty(p);
    page_note(p);
    return p;
}

static const uint32_t kClobber = 4;

/* unit `stable ADDR SIZE`: steered data that no callee writes (e.g. stage path data): never clobbered */
static std::vector<std::pair<uint32_t, uint32_t>> g_stable;

static bool clobberable(uint32_t ea) {
    if (capm::present) return false; /* a capture is one real state (namespace capm) */
    if (image_region(ea)) return false;
    for (auto& r : g_stable)
        if (ea - r.first < r.second) return false;
    if (ea - V.stack_lo < V.entry_sp + 8 - V.stack_lo) return false;
    return true;
}

/* Proposal: invalidate typed provenance on any byte write, including nested effects. */
/* a page's float marks (Page::fm): read-only views (zeros when the page has no table) and the table itself (allocated) */
static const uint64_t kNoMarks[128] = {};
static inline const uint64_t* pfw(const Page* p) { return p->fm ? p->fm : kNoMarks; }
static inline const uint64_t* pdw(const Page* p) { return p->fm ? p->fm + 64 : kNoMarks + 64; }
static inline uint64_t* pfm(Page* p) {
    if (!p->fm) p->fm = (uint64_t*)arena_zalloc(128 * sizeof(uint64_t));
    return p->fm;
}
/* a page's marks := fw/dw (no table is allocated for none) */
static void page_set_marks(Page* p, const uint64_t* fw, const uint64_t* dw) {
    bool any = false;
    for (int w = 0; w < 64 && !any; w++) any = fw[w] | dw[w];
    if (!any) { if (p->fm) memset(p->fm, 0, 128 * sizeof(uint64_t)); return; }
    uint64_t* m = pfm(p);
    memcpy(m, fw, 64 * sizeof(uint64_t));
    memcpy(m + 64, dw, 64 * sizeof(uint64_t));
}
static inline bool fmark_test(bool dbl, uint32_t a) {
    Page* p = page_peek(a >> 12);
    uint32_t o = a & 0xFFF;
    return p && p->fm && (p->fm[(dbl ? 64 : 0) + (o >> 6)] >> (o & 63) & 1);
}
static inline void fmark_set(bool dbl, uint32_t a) {
    Page* p = page(a >> 12);
    uint32_t o = a & 0xFFF;
    pfm(p)[(dbl ? 64 : 0) + (o >> 6)] |= 1ull << (o & 63);
    V.fany = true;
}
static inline void fmark_clear(bool dbl, uint32_t lo, uint32_t hi) { /* inclusive, wraps like the old key loop */
    for (uint32_t a = lo;; a++) {
        Page* p = page_peek(a >> 12);
        if (p && p->fm) { uint32_t o = a & 0xFFF; p->fm[(dbl ? 64 : 0) + (o >> 6)] &= ~(1ull << (o & 63)); }
        if (a == hi) break;
    }
}

/* a chunk's clobber epochs (Chunk::ep): the same values as a plain uint32_t[64] (absent = all 0) */
static inline uint32_t ep_get(const Chunk* k, int b) {
    if (!k->ep) return 0;
    if (k->epw) return ((const uint32_t*)k->ep)[b];
    uint16_t v = ((const uint16_t*)k->ep)[b];
    return v ? k->epbase + v - 1 : 0;
}
static inline void ep_zero(Chunk* k) { /* every byte back to epoch 0 */
    if (!k->ep) return;
    memset(k->ep, 0, k->epw ? 64 * sizeof(uint32_t) : 64 * sizeof(uint16_t));
    k->epclean = !k->epw;
}
[[gnu::noinline]] static void ep_set_slow(Chunk* k, int b, uint32_t e) { /* e != 0 does not fit the 16-bit form */
    uint16_t* a = (uint16_t*)k->ep;
    bool clean = true;
    for (int i = 0; i < 64 && clean; i++) clean = a[i] == 0 || i == b;
    if (clean) { /* nothing else to keep: a new base */
        a[b] = 1;
        k->epbase = e;
        return;
    }
    uint32_t* w = (uint32_t*)arena_zalloc(64 * sizeof(uint32_t));
    for (int i = 0; i < 64; i++) w[i] = a[i] ? k->epbase + a[i] - 1 : 0;
    w[b] = e;
    k->ep = w;
    k->epw = 1;
}
static inline void ep_set(Chunk* k, int b, uint32_t e) {
    if (!k->ep) {
        if (!e) return; /* absent = 0 */
        k->ep = arena_zalloc(64 * sizeof(uint16_t));
        k->epw = 0;
        k->epclean = 1;
    }
    if (k->epw) { ((uint32_t*)k->ep)[b] = e; return; }
    uint16_t* a = (uint16_t*)k->ep;
    if (!e) { a[b] = 0; return; }
    if (k->epclean) { k->epbase = e; k->epclean = 0; }
    if (e >= k->epbase && e - k->epbase < 0xFFFFu) { a[b] = (uint16_t)(e - k->epbase + 1); return; }
    ep_set_slow(k, b, e);
}
static inline void invalidate_float_overlap(uint32_t ea, uint32_t n = 1) {
    /* bytes [ea, ea+n): forget every f32 word start in [ea-3, ea+n-1] and f64 start in [ea-7, ea+n-1] */
    if (!V.fany) return;
    fmark_clear(false, ea - 3, ea + n - 1);
    fmark_clear(true, ea - 7, ea + n - 1);
}

static inline void materialize(Page* p, uint32_t ea, bool reading) {
    uint32_t o = ea & 0xFFF;
    if (!(p->have[o >> 6] >> (o & 63) & 1)) {
        bool known;
        uint8_t v = initial_byte(ea, &known);
        if (__builtin_expect(capm::present, 0) && reading && !g_in_peek) {
            if (capm::readlog && capm::mapped(ea)) capm::readlog->insert(ea);
            if (capm::genlog && !known) capm::genlog->insert(ea);
        }
        if (!known && reading && V.mode == MODE_REC && V.depth == 0) {
            if (!V.stray) V.first_stray = ea;
            V.stray++;
        }
        Chunk*& k = p->ch[o >> 6];
        if (!k) k = (Chunk*)arena_zalloc(sizeof(Chunk));
        else if (!p->have[o >> 6]) ep_zero(k); /* the chunk's first byte in this run: its other epochs are stale (each is reset
                                                * when its byte materializes), so the 16-bit form may start a new base */
        k->d[o & 63] = k->init[o & 63] = v;
        ep_set(k, o & 63, 0);
        p->have[o >> 6] |= 1ull << (o & 63);
    }
    Chunk* k = p->ch[o >> 6];
    /* the epoch only matters where a mocked call can clobber the byte; that condition is fixed for the
     * whole run (input mode, clobber switch, stable ranges, stack bounds), so elsewhere it is not kept */
    uint32_t cur = ep_get(k, o & 63);
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
        ep_set(k, o & 63, V.epoch);
    }
}

/* `accessbudget N` (unit file, 2026-10-26): this unit's guest memory-access budget per run (default 20M; AGL#18's original
 * ShaderMgr archive setup legitimately needs more) */
static uint64_t g_access_budget = 20000000ull;
/* `opbudget BYTES`: the per-operation byte cap of the mockset, mockzero
 * and memmove (mockmove) models together (default 20,000,000; 1..1 GB; decimal or 0x-hex; once per spec). A larger
 * fill or copy aborts the input with "<op> exceeds guest operation budget". Under -v every fill or copy over 1 MB
 * through these models is listed on stderr (`opbudget: OP TARGET dst DST size SIZE lr LR ...`). */
static uint32_t g_op_budget = 20000000u;
static bool g_op_budget_set = false;
static int g_verbose = 0; /* -v count, for model diagnostics */
/* WWHD_MEMSTAT=1: memory diagnostics on stderr (progress every 1M guest accesses, totals per run); off by default */
static bool g_memstat = getenv("WWHD_MEMSTAT") != nullptr;
static inline void tick() {
    if (g_memstat && (V.budget & 0xFFFFF) == 0xFFFFF) /* analysis: progress of one run every 1M accesses */
        fprintf(stderr, "memstat-progress %s seed=%llu budget=%llu pages=%zu snap=%zu calls=%zu epoch=%u depth=%d\n",
                V.is_candidate ? "cand" : "orig", (unsigned long long)V.seed, (unsigned long long)V.budget, V.pages.size(),
                V.snap.size(), V.calls.size(), V.epoch, V.depth);
    if (++V.budget > g_access_budget && V.can_abort) {
        V.abort_reason = "access budget exceeded";
        longjmp(V.abort_jmp, 1);
    }
    if ((V.budget & 0xFFFF) == 0) hostmem_check();
}

/* -watch ADDR[:LEN] (2026-10-26, AGL#18): print every guest byte access to [ADDR, ADDR+LEN) at any depth (setups and
 * nested real callees included) with the side, depth and innermost `real` function, to stderr */
static uint32_t g_watch_lo = 0, g_watch_len = 0;
static void watch_note(char kind, uint32_t ea, uint8_t v) {
    fprintf(stderr, "watch %c %08X %02X side=%s depth=%d in=%08X seed=%llu\n", kind, ea, v, V.is_candidate ? "cand" : "orig",
            V.depth, V.real_chain.empty() ? 0u : V.real_chain.back(), (unsigned long long)V.seed);
}
/* -nulltrap / unit `nulltrap` (2026-10-26, AGL#13): console DSI emulation. On the Wii U a guest access to page 0
 * (EA 0x0000..0x0FFF) is a DSI exception; without the option the harness lets it read and write guest page-0 bytes,
 * and a mutant's NULL walk (`for (p = at(0); p != at(4n); ++p)`: at(0) is the host nullptr, at(4n) is PPC_MEM_BASE + 4n,
 * so the host pointer never meets its end) spins until a budget aborts it. With the option, any guest load or store of a
 * byte in page 0 by the tested function or a `real` callee (any depth) ends that side's run at once, as a DSI would: a
 * trap event (Result::dsi) the comparison reports as `trap: ...` when one side takes it; both sides taking it compare
 * like a `stopat` end (no return value): at the same access, calls, traps and net effect up to the fault, without the dying
 * frames and linkage words [stack_lo, entry SP + 8) (2026-10-26 AGL#4); at different accesses only the common call prefix
 * (2026-10-26 AGL#13).
 * Inside a `setup`/`postsetup` routine it is a fixture error (exit 2). Harness-internal reads (peek8)
 * and mock/recorded effects (patch8) are not guest accesses and never trap. Off: one predictable branch per byte. */
static bool g_nulltrap = false;
/* -p0exclude: note, without trapping, whether the
 * ORIGINAL's run touched page 0 (the same accesses -nulltrap would trap: guest loads/stores, not harness peeks). A
 * difference on such an input comes after the original's page-0 access, which the zero-page rule excludes as evidence
 *: it is counted like an input aborted on both sides and reported on stderr
 * (`p0exclude: ADDR: N ...`), not as a FAIL. Differences on inputs where the original did not touch page 0 stay FAILs. */
static bool g_p0exclude = false;
static bool g_nulltrap_unit = false, g_nulltrap_cli = false; /* where -nulltrap came from: the unit's `nulltrap` directive / the command line */
[[gnu::cold, gnu::noinline]] static void p0_note();
static const char* g_setup_what = nullptr; /* "setup"/"postsetup" while such a routine runs */
/* `strictmocks` (unit file, opt-in, 2026-11-02, ): inside a setup or postsetup, a call
 * that gets the GENERATED answer of the generic mock for a value-returning import (r3 or f1 declared, or no declaration)
 * ends the run with exit 2 naming the import, the setup and the call site: the fixture would build on an invented value.
 * The answer itself is the point of failure (the harness cannot see the caller's register reads). Answers a spec states
 * (ret!/retarg/mockset/osmem parts/...) and imports the unit declares `importvoid SLOT` pass; every other import counts as
 * value-returning (mkunit gives imports defr = deff = all ones). Off by default: units without it are unchanged. */
static bool g_strictmocks = false;
/* `strictmocks report` (diagnostic only; a fixture is accepted only under the stopping mode): the same check counts every
 * hit per (setup, import, innermost real function, lr) and goes on; each key's first hit prints a `strictmocks report: first:`
 * line at once, and a normal process exit prints one line per key with its count. The run goes on with the generated
 * answers, so it can fault later like the unit without the flag. */
static bool g_strictmocks_report = false;
/* key -> (the import's name, taken at the first hit; count). A leaked heap object, and the printer below uses only what it holds:
 * it runs from atexit, and the callee table callee_name reads may already be destroyed by then (2026-11-02: a host signal 11 in
 * callee_info under exit, after the rows; the counts never printed) */
static std::map<std::tuple<std::string, uint32_t, uint32_t, uint32_t, uint32_t>, std::pair<std::string, uint64_t>>* g_strictmocks_hits = nullptr;
static void strictmocks_print() {
    if (!g_strictmocks_hits) return;
    for (auto& [k, v] : *g_strictmocks_hits)
        fprintf(stderr, "strictmocks report: %s %08X calls import %s from %08X (lr %08X): %llu time(s)\n", std::get<0>(k).c_str(),
                std::get<1>(k), v.first.c_str(), std::get<3>(k), std::get<4>(k), (unsigned long long)v.second);
    if (g_strictmocks_hits->empty()) fprintf(stderr, "strictmocks report: no generated import answer inside setups\n");
}
static uint32_t g_setup_target = 0;
struct DsiEv { char kind; uint32_t ea; int depth; uint32_t in; };
static DsiEv g_dsi;
static int g_dsi_thread = 0; /* `osmem ... threads`: the guest thread that took the fault (0 = main) */
/* the fault kinds of Result::dsi: 'R' load, 'W' store (page-0 DSI), 'X' instruction fetch (an indirect call or branch to a
 * page-0 target on either side, 2026-11-02 or a candidate-only non-code target: an ISI on the console,
 * 2026-10-29 ) */
static const char* dsi_what(char kind) { return kind == 'R' ? "load" : kind == 'W' ? "store" : "fetch"; }
[[gnu::cold, gnu::noinline]] static void nulltrap_hit(char kind, uint32_t ea) {
    if (!V.can_abort) return; /* outside a run: not a guest access of the code under test */
    uint32_t in = V.real_chain.empty() ? 0u : V.real_chain.back();
    if (g_setup_what) {
        if (kind == 'X')
            fprintf(stderr, "nulltrap: %s %08X called a page-0/non-code target (fetch %08X, %s side, seed %llu, depth %d, in=%08X): "
                    "the fixture must not call through such a pointer (an ISI on the console)\n", g_setup_what, g_setup_target, ea,
                    V.is_candidate ? "candidate" : "original", (unsigned long long)V.seed, V.depth, in);
        else
            fprintf(stderr, "nulltrap: %s %08X touched page 0 (%s %08X, %s side, seed %llu, depth %d, in=%08X): the fixture "
                    "must not access page 0 (a DSI on the console)\n", g_setup_what, g_setup_target, dsi_what(kind),
                    ea, V.is_candidate ? "candidate" : "original", (unsigned long long)V.seed, V.depth, in);
        exit(2);
    }
    g_dsi = DsiEv{kind, ea, V.depth, in};
    g_dsi_thread = gthr::current_id();
    longjmp(V.abort_jmp, 4);
}
[[gnu::cold, gnu::noinline]] static void p0_note() {
    if (V.can_abort && !V.is_candidate && !g_setup_what) V.p0 = true; /* the original's run itself, not its setups */
}
static inline uint8_t rd8(uint32_t ea) {
    if (__builtin_expect(g_nulltrap, 0) && ea < 0x1000 && !g_in_peek) nulltrap_hit('R', ea);
    if (__builtin_expect(g_p0exclude, 0) && ea < 0x1000 && !g_in_peek) p0_note();
    Page* p = page(ea >> 12);
    materialize(p, ea, true);
    uint32_t o = ea & 0xFFF;
    if (g_watch_len && !g_in_peek && ea - g_watch_lo < g_watch_len) watch_note('R', ea, p->ch[o >> 6]->d[o & 63]);
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
static inline void wr8_raw(uint32_t ea, uint8_t v) { /* the caller has invalidated the float provenance */
    if (__builtin_expect(g_nulltrap, 0) && ea < 0x1000) nulltrap_hit('W', ea);
    if (__builtin_expect(g_p0exclude, 0) && ea < 0x1000) p0_note();
    stack_guard(ea);
    Page* p = page(ea >> 12);
    materialize(p, ea, false);
    uint32_t o = ea & 0xFFF;
    p->ch[o >> 6]->d[o & 63] = v;
    p->wr[o >> 6] |= 1ull << (o & 63);
    if (g_watch_len && ea - g_watch_lo < g_watch_len) watch_note('W', ea, v);
}
static inline void wr8(uint32_t ea, uint8_t v) {
    invalidate_float_overlap(ea);
    wr8_raw(ea, v);
}
/* a callee's write (mock / recorded effect): changes memory but is not the function's own write */
static void patch8(uint32_t ea, uint8_t v) {
    Page* p = page(ea >> 12);
    materialize(p, ea, false);
    invalidate_float_overlap(ea);
    uint32_t o = ea & 0xFFF;
    p->ch[o >> 6]->d[o & 63] = v;
    if (g_watch_len && ea - g_watch_lo < g_watch_len) watch_note('P', ea, v); /* mock/recorded effect write */
}
static uint8_t peek8(uint32_t ea) { /* no stray accounting */
    int d = V.depth;
    V.depth = 1;
    g_in_peek++;
    uint8_t v = rd8(ea);
    g_in_peek--;
    V.depth = d;
    return v;
}

extern "C" {
/* `osmem ... threads`: the slice preemption counts guest accesses (only while a second guest thread exists) */
#define GTHR_SLICE() do { if (__builtin_expect(gthr::g_multi, 0)) gthr::slice_hit(); } while (0)
uint8_t vm_ld8(uint32_t ea) { GTHR_SLICE(); tick(); return rd8(ea); }
uint16_t vm_ld16(uint32_t ea) { GTHR_SLICE(); tick(); return (uint16_t)(rd8(ea) << 8 | rd8(ea + 1)); }
static int g_trace; /* -trace SEED: print the memory accesses and calls of that input */
uint32_t vm_ld32(uint32_t ea) {
    GTHR_SLICE();
    tick();
    uint32_t v = (uint32_t)rd8(ea) << 24 | (uint32_t)rd8(ea + 1) << 16 | (uint32_t)rd8(ea + 2) << 8 | rd8(ea + 3);
    if (g_trace && V.depth == 0) printf("        ld32 %08X -> %08X (epoch %u)\n", ea, v, V.epoch);
    if (V.collect && V.depth == 0 && ea - 0x10000000u < 0x0018C0B0u && (v & 0x7F800000u) != 0x7F800000u && V.dict.size() < 64 &&
        std::find(V.dict.begin(), V.dict.end(), v) == V.dict.end())
        V.dict.push_back(v); /* a .rodata word (float constant) */
    if (V.depth == 0 && (v & 0x7F800000u) == 0x7F800000u && (v & 0x007FFFFFu) && fmark_test(false, ea))
        V.fcopy[V.nfcopy++ % 8] = v;
    return v;
}
uint64_t vm_ld64(uint32_t ea) { return (uint64_t)vm_ld32(ea) << 32 | vm_ld32(ea + 4); }
void vm_st8(uint32_t ea, uint8_t v) { GTHR_SLICE(); tick(); wr8(ea, v); }
void vm_st16(uint32_t ea, uint16_t v) { GTHR_SLICE(); tick(); invalidate_float_overlap(ea, 2); wr8_raw(ea, v >> 8); wr8_raw(ea + 1, (uint8_t)v); }
void vm_st32(uint32_t ea, uint32_t v) {
    GTHR_SLICE();
    tick();
    bool copied_float = false;
    if (V.depth == 0) {
        for (unsigned i = 0; i < 8 && i < V.nfcopy; i++)
            if (V.fcopy[i] == v) { copied_float = true; break; } /* inherited GHS value-copy rule */
    }
    if (g_trace && V.depth == 0) printf("        st32 %08X <- %08X (epoch %u)\n", ea, v, V.epoch);
    invalidate_float_overlap(ea, 4);
    for (int i = 0; i < 4; i++) wr8_raw(ea + i, (uint8_t)(v >> (24 - 8 * i)));
    if (copied_float) fmark_set(false, ea);
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
    fmark_set(false, ea); /* explicit float stores also inside real callees */
}
void gmem_stf32(uint32_t ea, uint32_t v) { vm_stf32w(ea, v); }
void vm_stf64w(uint32_t ea, uint64_t v) {
    vm_st64(ea,v);
    fmark_set(true, ea);
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
/* gabi::trap: the candidate's side of a trap instruction, counted like the original's vm_trap (compare() checks
 * the counts) */
void gmem_trap(Cpu* c, uint32_t addr) { vm_trap(c, addr, 0); }
/* gabi::load_reserved / store_conditional / sync: the reference's lwarx/stwcx. rule (vm_gen.h) on observed memory */
uint32_t gmem_lwarx(Cpu* c, uint32_t ea) { c->res_addr = ea; c->res_val = vm_ld32(ea); return c->res_val; }
int gmem_stwcx(Cpu* c, uint32_t ea, uint32_t v) {
    int ok = c->res_addr == ea && vm_ld32(ea) == c->res_val;
    if (ok) vm_st32(ea, v);
    c->res_addr = 0xFFFFFFFFu;
    c->cr[0] = 0; c->cr[1] = 0; c->cr[2] = (uint8_t)ok; c->cr[3] = c->xer_so;
    return ok;
}
void gmem_sync(void) {}
/* the runtime trap entry (ppc.h) for objects that reference it outside the generated code: the same trap event */
void ppc_trap(Cpu* c, uint32_t addr) { vm_trap(c, addr, 0); }
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

/* ------------------------------------------------------------------ -fastreset */
/* -fastreset (2026-10-28, lib2 verify speed): with `setupsnapshot`, every generated input used to install the captured
 * post-setup state into fresh pages (AGL world: ~7 MB of materialized bytes, ~110k chunks, per input and side), rebase
 * all of it (init := current, write bits cleared), scan all of it for the net effect and unmap it again: ~95% of an
 * AGL#12 input's CPU time, the tested function ~2%. With -fastreset the snapshot is built once per function as
 * "template pages" that stay in V.pages across inputs. A template page holds exactly the state an input's run has
 * right after the post-setup rebase would have for an untouched page: the snapshot's materialized bytes, current =
 * initial value, no write bits, the snapshot's float marks. Any access to a page (page()/page_peek()) puts it into
 * V.touched (Page::dirty); at the start of the next run and in clear_all() only the pages in V.touched are restored
 * (template pages) or cleared/dropped (all other pages), so each run starts from the same state as before:
 *   - runs that install the snapshot (SNAP_USE, generated input): fast_reset() + the snapshot's counters; the rebase
 *     loop over V.touched then rebases every page that is not in its template (= rebased) state;
 *   - every other run (setupsnapshot capture/validation, recorded inputs, functions without a snapshot): reset_memory()
 *     clears all pages, template pages included, exactly as before;
 *   - a run that ends (abort) after the install but before the rebase gets the snapshot's own initial values and write
 *     bits back on the template pages (tmpl_unbaseline), as the old install had left them.
 * The leading input-independent `fill`/`copy` lines of a function go into g_snap_base once per function (see
 * initial_byte). Results are identical with and without -fastreset (validated on AGL#12 and non-agl units, seeds 1/7,
 * with and without -nulltrap, and on mutate.py receipts). On by default since 2026-10-28; off: `-nofastreset` or
 * WWHD_VERIFY_FASTRESET=0. */
static bool g_fastreset = [] { const char* e = getenv("WWHD_VERIFY_FASTRESET"); return !(e && !strcmp(e, "0")); }(); /* default on */
struct TChunk { uint8_t d[64]; uint8_t init_pre[64]; };
struct TPage {
    uint64_t have[64], fw[64], dw[64];
    uint64_t* fm;         /* the live page's float-mark table (persistent, outside the arena), nullptr when the snapshot has none */
    uint64_t wr_pre[64];  /* the snapshot's write bits (state before the post-setup rebase) */
    TChunk* tc[64];       /* the snapshot's chunks (current bytes, and their initial values before the rebase) */
    Chunk* live[64];      /* the live page's chunk objects for them (persistent, outside the arena) */
};
static std::vector<Page*> g_tmpl_pages;  /* the live template pages (persistent, outside the arena) */
static bool g_fast_prebase = false;      /* a fast run installed the template state and has not reached the rebase yet */

static void page_clear_bits(Page* p) {
    memset(p->have, 0, sizeof p->have);
    memset(p->wr, 0, sizeof p->wr);
    if (p->fm) memset(p->fm, 0, 128 * sizeof(uint64_t));
}
static void tmpl_restore(Page* p) { /* back to the template state; chunk pointers into the arena are dropped */
    const TPage* t = p->t;
    memcpy(p->have, t->have, sizeof p->have);
    memset(p->wr, 0, sizeof p->wr);
    p->fm = t->fm; /* (a table the run allocated in the arena is dropped) */
    if (t->fm) { memcpy(t->fm, t->fw, sizeof t->fw); memcpy(t->fm + 64, t->dw, sizeof t->dw); }
    for (int w = 0; w < 64; w++) {
        if (const TChunk* tc = t->tc[w]) {
            Chunk* k = t->live[w];
            memcpy(k->d, tc->d, 64);
            memcpy(k->init, tc->d, 64);
            k->ep = nullptr; /* every byte current at epoch 0 */
            p->ch[w] = k;
        } else
            p->ch[w] = nullptr;
    }
    p->dirty = 0;
}

static void reset_memory() {
    for (Page* p : g_tmpl_pages) /* template pages are cleared like all others */
        if (!p->dirty) page_dirty(p);
    for (Page* p : V.touched) page_clear_bits(p);
    V.fany = false;
}
/* -fastreset, a run that installs the snapshot: template pages in V.touched go back to their template state (and leave
 * V.touched), every other page is cleared as reset_memory() does */
static void fast_reset() {
    size_t k = 0;
    for (Page* p : V.touched) {
        if (p->t) tmpl_restore(p);
        else { page_clear_bits(p); V.touched[k++] = p; }
    }
    V.touched.resize(k);
    memset(g_pcache, 0, sizeof g_pcache); /* it may hold restored (clean) template pages */
    V.fany = false;
}
/* the run ended between the install and the rebase: the template pages get the snapshot's initial values and write bits
 * (the state the old per-input install left until the rebase) */
static void tmpl_unbaseline() {
    for (Page* p : g_tmpl_pages) {
        if (!p->dirty) page_dirty(p);
        const TPage* t = p->t;
        for (int w = 0; w < 64; w++) {
            p->wr[w] |= t->wr_pre[w];
            if (const TChunk* tc = t->tc[w])
                for (uint64_t m = t->have[w]; m; m &= m - 1) {
                    int b = __builtin_ctzll(m);
                    p->ch[w]->init[b] = tc->init_pre[b];
                }
        }
    }
}

/* Shared fill pages: a `fill` line covering a whole 4 KB page (the AGL/J3D worlds fill 16 MB heaps with
 * zeros: ~4.6 KB of SnapPage per page, 79 MB for a J3D lighting spec) points the page at one read-only page per fill byte
 * instead of a copy. A page that is written later (snap_set, a partial fill, a copy or blob) gets its own copy first. */
static SnapPage* g_fill_pages[256];
static SnapPage* fill_page(uint8_t b) {
    if (!g_fill_pages[b]) {
        SnapPage* s = (SnapPage*)malloc(sizeof(SnapPage));
        memset(s->d, b, sizeof s->d);
        memset(s->has, 0xFF, sizeof s->has);
        g_fill_pages[b] = s;
    }
    return g_fill_pages[b];
}
static inline bool snap_shared(const SnapPage* s) { return s && s == g_fill_pages[s->d[0]]; }
static void snap_free(SnapPage* s) { if (!snap_shared(s)) free(s); }
static SnapPage* snap_own(SnapPage*& s) { /* a page of its own to write into (new: zero, no bytes) */
    if (!s) s = (SnapPage*)calloc(1, sizeof(SnapPage));
    else if (snap_shared(s)) { SnapPage* c = (SnapPage*)malloc(sizeof(SnapPage)); memcpy(c, s, sizeof(SnapPage)); s = c; }
    return s;
}

static uint64_t g_input_serial = 0; /* clear_all() count: -fastreset's kept validation state belongs to one input */
static void clear_all() {
    g_input_serial++;
    /* pages live in the arena: release it (and the per-input containers' capacity) after every input */
    if (!g_tmpl_pages.empty()) { /* -fastreset: template pages stay (restored), all other pages go */
        for (Page* p : V.touched) {
            if (p->t) tmpl_restore(p);
            else V.pages.erase(p->pn);
        }
        std::vector<Page*>().swap(V.touched);
    } else {
        std::vector<Page*>().swap(V.touched);
        std::unordered_map<uint32_t, Page*>().swap(V.pages);
    }
    memset(g_pcache, 0, sizeof g_pcache);
    arena_release();
    BigVec<CallEv>().swap(V.calls);
    for (auto& kv : V.snap) snap_free(kv.second);
    V.snap.clear();
    g_snap_base = nullptr;
}

static void snap_set(uint32_t ea, uint8_t v) {
    SnapPage* s = snap_own(V.snap[ea >> 12]);
    uint32_t o = ea & 0xFFF;
    s->d[o] = v;
    s->has[o >> 6] |= 1ull << (o & 63);
}
/* range forms of snap_set (2026-10-26, AGL#18: a 16 MB fill + 16 MB blob per input went byte by byte through the page
 * map): the same bytes and `has` bits, one page lookup per 4 KB */
static void snap_range(uint32_t ea, uint32_t n, const uint8_t* src, uint8_t fill,
                       std::unordered_map<uint32_t, SnapPage*>& snap = V.snap) {
    while (n) {
        uint32_t o = ea & 0xFFF, k = std::min<uint32_t>(n, 0x1000 - o);
        SnapPage*& slot = snap[ea >> 12];
        if (!src && k == 0x1000) { /* a whole page of one byte: the shared page (the same bytes and `has` bits) */
            snap_free(slot);
            slot = fill_page(fill);
            ea += k; n -= k;
            if (ea == 0) break;
            continue;
        }
        SnapPage* sp = snap_own(slot);
        if (src) memcpy(sp->d + o, src, k); else memset(sp->d + o, fill, k);
        for (uint32_t b = o; b < o + k;) {
            if ((b & 63) == 0 && o + k - b >= 64) { sp->has[b >> 6] = ~0ull; b += 64; }
            else { sp->has[b >> 6] |= 1ull << (b & 63); b++; }
        }
        ea += k; n -= k; if (src) src += k;
        if (ea == 0) break; /* wrapped */
    }
}
static bool snap_has(uint32_t ea) { return snap_page_of(ea, ea & 0xFFF) != nullptr; }

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

/* A net effect: address -> final byte, ascending addresses. Was a std::map<uint32_t, uint8_t>: ~48 host
 * bytes per changed guest byte (a setupsnapshot capture run of a J3D lighting spec holds ~4.9M of them, ~230 MB); here 8.
 * The map operations the harness uses, with the same results; entries are only appended in ascending order. */
struct NetMap {
    using value_type = std::pair<uint32_t, uint8_t>;
    using const_iterator = BigVec<value_type>::const_iterator;
    BigVec<value_type> v;
    size_t size() const { return v.size(); }
    bool empty() const { return v.empty(); }
    const_iterator begin() const { return v.begin(); }
    const_iterator end() const { return v.end(); }
    const_iterator find(uint32_t a) const {
        auto it = std::lower_bound(v.begin(), v.end(), a, [](const value_type& e, uint32_t x) { return e.first < x; });
        return it != v.end() && it->first == a ? it : v.end();
    }
    size_t count(uint32_t a) const { return find(a) != end(); }
    uint8_t at(uint32_t a) const {
        auto it = find(a);
        if (it == end()) throw std::out_of_range("NetMap::at");
        return it->second;
    }
    void push(uint32_t a, uint8_t b) { /* a above every address held */
        if (!v.empty() && v.back().first >= a) { fprintf(stderr, "harness: NetMap out of order (%08X after %08X)\n", a, v.back().first); abort(); }
        v.emplace_back(a, b);
    }
    void insert(const value_type& kv) { push(kv.first, kv.second); }
    void emplace_hint(const_iterator, uint32_t a, uint8_t b) { push(a, b); }
    bool operator==(const NetMap& o) const { return v == o.v; }
    bool operator!=(const NetMap& o) const { return v != o.v; }
};

/* net effect of a run: bytes whose value changed, outside the scratch stack */
/* *over (host memory budget, 2026-11-01): set when the process's resident size passed the budget while the map was being
 * built (sampled every 65536 entries; a runaway mutant's run can leave tens of millions of written bytes, 8 bytes of
 * NetMap each), the map is then dropped and the caller ends the run as an abort */
static NetMap net_writes(bool* over = nullptr, uint64_t* over_mb = nullptr) {
    uint32_t fp_lo = V.entry_sp + 8; /* no footprint: the whole scratch stack incl. back chain/LR save is excluded */
    if (uint32_t fs = footprint_of(g_cur_func_fp); fs != UINT32_MAX) fp_lo = V.entry_sp - fs;
    NetMap out;
    /* V.touched: every page except untouched -fastreset template pages, which have no write bits (the same set as
     * V.pages without -fastreset); in page order, so the addresses come out ascending */
    std::vector<Page*> pg(V.touched);
    std::sort(pg.begin(), pg.end(), [](const Page* x, const Page* y) { return x->pn < y->pn; });
    pg.erase(std::unique(pg.begin(), pg.end()), pg.end());
    size_t nwr = 0; /* an upper bound: no reallocation (a capture run's ~5M entries would pass through 1.5-2x that) */
    for (Page* p : pg)
        for (int w = 0; w < 64; w++) nwr += __builtin_popcountll(p->wr[w]);
    out.v.reserve(nwr);
    size_t sampled = 0;
    for (Page* p : pg) {
        for (int w = 0; w < 64; w++) {
            uint64_t m = p->wr[w];
            while (m) {
                int b = __builtin_ctzll(m);
                m &= m - 1;
                uint32_t o = w * 64 + b, ea = (p->pn << 12) | o;
                if (ea - V.stack_lo < V.entry_sp + 8 - V.stack_lo && ea - V.stack_lo < fp_lo - V.stack_lo) continue; /* own frames + back chain/LR save */
                if (__builtin_expect(gthr::g_multi, 0) && gthr::in_thread_stack(ea)) continue; /* `threads`: a guest thread's private stack */
                materialize(p, ea, false); /* apply calls made after the write (clobber) */
                /* recorded inputs: a written byte whose initial value was never read is unknown,
                 * so it counts as written whatever its value */
                bool unknown = V.mode == MODE_REC && !snap_has(ea) && !image_region(ea);
                const Chunk* k = p->ch[o >> 6];
                if (k->d[o & 63] != k->init[o & 63] || unknown) out.push(ea, k->d[o & 63]);
                if (over && g_hostmem_budget_mb && out.size() >= sampled + 65536) {
                    sampled = out.size();
                    if (uint64_t mb = host_rss_mb(); mb > g_hostmem_budget_mb) { /* the sampled size, before the map is released */
                        *over = true;
                        if (over_mb) *over_mb = mb;
                        BigVec<NetMap::value_type>().swap(out.v);
                        return out;
                    }
                }
            }
        }
    }
    if (out.v.capacity() > 2 * out.v.size() + 1024) out.v.shrink_to_fit(); /* most written bytes unchanged */
    return out;
}

/* ------------------------------------------------------------------ calls */
/* compared pointee size of argument rI at this call: a dynamic extent (rN=dynK*rM) is K*r[M], capped at the
 * declared ptrsz (overflow-checked); otherwise the static ptrsz */
static uint32_t pointee_size(const CalleeInfo* ci, int i, const Cpu* c) {
    if (!ci) return 0;
    if (ci->dynsrc[i] == 0xFF) { /* rN=cstr[:MAX]: through the first NUL inclusive; an unterminated string within MAX
                                    * is captured as MAX+1 bytes so it can only compare equal to another unterminated one */
        uint32_t n = 0;
        while (n < ci->ptrsz[i] && peek8(c->r[i] + n) != 0) n++;
        return n + 1;
    }
    if (ci->dynsrc[i]) {
        uint64_t n = (uint64_t)c->r[ci->dynsrc[i]] * ci->dynmul[i];
        return n > ci->ptrsz[i] ? ci->ptrsz[i] : (uint32_t)n;
    }
    return ci->ptrsz[i];
}
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
/* `callcr TARGET BIT[,BIT...]`: the listed CR bits (0..31, bit 6 = cr1.eq, the variadic-call float flag) are recorded
 * at every call to TARGET and compared like an argument (NW-SND#11 request: AXSetVoiceRmtIIRCoefs is variadic) */
static std::unordered_map<uint32_t, uint32_t> g_callcr_specs;
/* `observeptr TARGET rN+OFF SIZE`: at every call to TARGET, the SIZE bytes the pointer stored at rN+OFF points to are
 * compared (the pointer itself is not; a null pointer records nothing) (AGL-GX2#3 request: a callback descriptor's
 * pointer to the caller's loop index) */
struct ObservePtr {
    unsigned reg; uint32_t off, size; bool direct = false;
    /* per-call-site forms of observearg (2026-10-28, AGL#12: one GX2SetVertexUniformReg site passes a fully defined
     * 16-byte uniform, another a buffer holding the original's conversion temps): `TARGET@N` = only the N-th top-level
     * call to TARGET in the run (0-based); `if rK=V` / `if lr=ADDR` = only calls where the ORIGINAL's rK (or return
     * address) equals V; the candidate's call at the same position is then compared whatever its own rK */
    int site = -1;
    int creg = 0;      /* 0: no condition; 3..10: rK; 32: lr */
    uint32_t cval = 0;
    /* dereference form `[rN+OFF]+OFF2` (2026-10-30, AGL#3 Mip request): the span is SIZE bytes at the word stored at
     * rN+OFF plus OFF2 (nothing when rN or that word is 0); kept with the per-site records (sobserved) */
    bool deref = false;
    uint32_t off2 = 0;
};
/* `observearg TARGET rN+OFF SIZE`: at every call to TARGET, the SIZE bytes at rN+OFF (the argument's own object, no
 * dereference) are compared, in addition to the raw argument (SEAD#11 request: input spans of mixed SDK records such as
 * GX2CalcSurfaceSizeAndAlignment's surface descriptor; output-only fields stay out) */
static std::unordered_map<uint32_t, std::vector<ObservePtr>> g_observeptr_specs;
/* `observenested TARGET [SPAN SIZE]` (2026-10-30, AGL#3 Mip/D3C request): calls to TARGET made inside `real` callees
 * (depth > 0, during the tested function's run; setups are not recorded) are recorded in order with the enclosing
 * top-level call, each with the listed spans (rN+OFF or [rN+OFF]+OFF2, SIZE bytes, read at call entry); per top-level call
 * the original's and the candidate's sequences (count, order, targets, bytes) are compared. A line without a span records
 * the calls only. Several lines per target add spans in line order. */
static std::unordered_map<uint32_t, std::vector<ObservePtr>> g_nested_specs;
/* the bytes of one observe span at call entry (rN+OFF, or [rN+OFF]+OFF2 through the stored word); empty when rN or the
 * stored pointer is 0 */
static uint32_t gw32(uint32_t a);
static bool span_bytes(const Cpu* c, const ObservePtr& o, std::vector<uint8_t>& bytes) {
    if (!c->r[o.reg]) return false;
    uint32_t at = c->r[o.reg] + o.off;
    if (o.deref) {
        uint32_t p = gw32(at);
        if (!p) return false;
        at = p + o.off2;
    }
    for (uint32_t j = 0; j < o.size; j++) bytes.push_back(peek8(at + j));
    return true;
}
static std::string span_name(const ObservePtr& o, uint32_t plus = 0) {
    char b[64];
    if (o.deref) snprintf(b, sizeof b, "[r%u+0x%X]+0x%X", o.reg, o.off, o.off2 + plus);
    else snprintf(b, sizeof b, "r%u+0x%X", o.reg, o.off + plus);
    return b;
}

/* -observe-pointees (2026-10-28, opt-in; harness option, mutate.py/verify.py/nulltrap_compare.py pass it through): at every
 * top-level mocked call, each argument register the comparison checks that holds an undeclared pointer into a real heap
 * block or a static object also has the bytes of that object compared (original vs candidate at the same call index).
 * Heap blocks: the sead ExpHeaps named by `observeheap ADDR` lines or -observe-heap ADDR options; with none, the shared
 * agl world's 16 MB root heap at 0x6A000000 when a valid ExpHeap header is there (exp_heap_valid; no spec line needed),
 * walked through their child heaps (same vtable = ExpHeap; a pointer inside a child of another kind, e.g. a FrameHeap,
 * has no block bounds and is not observed) and used-block lists; the layout read from the original 02755FE0 heap:
 * heap +0x1C start, +0x20 size, +0x28 child list {prev, next} with +0x30 count and +0x34 node offset, +0xA8 used-block
 * list with +0xB0 count and +0xB4 node offset; a block is a 0x10-byte header {prev, next, size, ...} and its data
 * [header + 0x10, + size). Static objects: build/verify/statics.tsv (mkstatics.py: every relocation target in
 * .rodata/.data/.bss and the section bounds; the object is [greatest boundary <= pointer, next boundary)). Objects larger
 * than the cap (-observe-pointees-cap, default 64 KB) are compared in a cap-sized window around the pointer (the
 * difference message says "truncated"). Stack pointers keep their own rules; a callee argument with any declared
 * pointee (`callee TARGET rN=SIZE` incl. rN=0 and rN=heapSIZE, a dynamic extent, an observearg/observeptr on that
 * register) keeps exactly its declared semantics. */
static bool g_observe_pointees = false;
static uint32_t g_obs_cap = 0x10000;
static std::vector<uint32_t> g_obs_heaps;   /* `observeheap ADDR` */
static std::vector<uint32_t> g_statics;     /* sorted static-object boundaries */
static uint64_t g_obs_n[3], g_obs_trunc, g_obs_bytes; /* observed pointer arguments (heap, static), truncated, bytes (stderr) */
static uint32_t gw32(uint32_t a) {
    return (uint32_t)peek8(a) << 24 | (uint32_t)peek8(a + 1) << 16 | (uint32_t)peek8(a + 2) << 8 | peek8(a + 3);
}
static bool exp_block(uint32_t h, uint32_t vt, uint32_t p, int depth, uint32_t* base, uint32_t* size) {
    uint32_t start = gw32(h + 0x1C), sz = gw32(h + 0x20);
    if (p - start >= sz) return false;
    if (depth < 8) { /* a child heap's arena is one block of its parent: look in the child first */
        uint32_t head = h + 0x28, cnt = gw32(h + 0x30), off = gw32(h + 0x34), node = gw32(head + 4);
        for (uint32_t k = 0; k < cnt && k < 4096 && node != head; k++) {
            uint32_t ch = node - off;
            uint32_t cs = gw32(ch + 0x1C), cz = gw32(ch + 0x20);
            if (p - cs < cz) return gw32(ch + 0xC) == vt && exp_block(ch, vt, p, depth + 1, base, size);
            node = gw32(node + 4);
        }
    }
    uint32_t head = h + 0xA8, cnt = gw32(h + 0xB0), off = gw32(h + 0xB4), node = gw32(head + 4);
    for (uint32_t k = 0; k < cnt && k < (1u << 20) && node != head; k++) {
        if (node - start >= sz) return false; /* not a block of this heap: stop */
        uint32_t blk = node - off, n = gw32(blk + 8), data = blk + 0x10;
        if (p - data < n) { *base = data; *size = n; return true; }
        node = gw32(node + 4);
    }
    return false;
}
/* a sead ExpHeap header at h: the object sits at the start of its own arena, the size is plausible and the used-block
 * list head links into the arena (or to itself when empty) */
static bool exp_heap_valid(uint32_t h) {
    uint32_t start = gw32(h + 0x1C), sz = gw32(h + 0x20), first = gw32(h + 0xAC);
    return h - start < 0x1000 && sz >= 0x1000 && sz <= 0x40000000u && (first == h + 0xA8 || first - start < sz);
}
/* the root heaps: `observeheap` lines / -observe-heap; without any, the shared agl world's root ExpHeap 0x6A000000 when
 * a valid ExpHeap header is there (so no spec needs a line for it) */
static const std::vector<uint32_t>& obs_heaps() {
    static const std::vector<uint32_t> world = {0x6A000000u}, none;
    if (!g_obs_heaps.empty()) return g_obs_heaps;
    return exp_heap_valid(world[0]) ? world : none;
}
/* the object p points into: 1 heap block, 2 static object, 0 none */
static int pointee_block(uint32_t p, uint32_t* base, uint32_t* size) {
    for (uint32_t h : obs_heaps())
        if (p - gw32(h + 0x1C) < gw32(h + 0x20)) return exp_block(h, gw32(h + 0xC), p, 0, base, size) ? 1 : 0;
    if (g_statics.size() >= 2 && p >= g_statics.front() && p < g_statics.back()) {
        auto it = std::upper_bound(g_statics.begin(), g_statics.end(), p);
        *base = *(it - 1);
        *size = *it - *(it - 1);
        return 2;
    }
    return 0;
}
static bool observeptr_declares(uint32_t target, int reg) {
    auto op = g_observeptr_specs.find(target);
    if (op == g_observeptr_specs.end()) return false;
    for (const ObservePtr& o : op->second)
        if ((int)o.reg == reg) return true;
    return false;
}
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
/* `align=rN` in place of ALIGN: the alignment is the call's own argument rN (e.g. MEMAllocFromFrmHeapEx r5), checked
 * per call (a power of two; a negative value, which allocates from the heap's tail, is not modelled and aborts) */
struct AllocSpec { unsigned sizereg; uint32_t base, align, fixed; unsigned alignreg; }; /* sizereg 0: fixed size (allocfixed) */
static std::unordered_map<uint32_t, AllocSpec> g_alloc_specs;
static std::unordered_map<uint32_t, uint32_t> g_alloc_cursor;
/* `allocfail TARGET K[,K...]` (2026-10-26, AGL#17 nested OOM edges): per input one K is picked from the list (the same on
 * both sides); the K-th steered call of TARGET in the run (1-based, top level and nested counted together) returns NULL
 * without advancing the cursor, i.e. that allocation fails as on an exhausted heap. K = 0: no failure. */
static std::unordered_map<uint32_t, std::vector<uint32_t>> g_alloc_fail;
static std::unordered_map<uint32_t, uint32_t> g_alloc_calls;
/* `allocnested TARGET`: the alloc/allocfixed steering of TARGET also answers its calls made inside `real` callees */
static std::set<uint32_t> g_alloc_nested;
/* `retarg TARGET rN`: TARGET's generated mock returns its own incoming rN in r3 (after its other mock effects) */
static std::unordered_map<uint32_t, int> g_retarg;
/* `setup FUNC|* ADDR [rN=VALUE ...]`: before the tested function, the `real` function ADDR runs once (both sides,
 * same entry state, own stack 0x8000 below the entry SP, nested depth: its calls are answered silently); its memory
 * effects become the starting state (e.g. an original constructor that builds a singleton the function consumes) */
/* register operand of a (post)setup line: VALUE, *ADDR (the word at ADDR when the line runs) or *(*ADDR+OFF) */
struct SetupReg { int reg; uint32_t value; int deref; uint32_t off; bool fp; }; /* fp: fN operand, value = f32 bits */
struct SetupStack { uint32_t off, value; }; /* sp+OFF=VALUE: a word in the caller frame (stack parameters) */
struct SetupSpec { uint32_t func, target; std::vector<SetupReg> regs; std::vector<SetupStack> stack; bool store; uint32_t store_addr; };
static std::vector<SetupSpec> g_setup_specs;
/* postsetup (2026-10-24, AGL#3): same ABI as setup, but runs after every poke line and before FUNC, for original
 * routines whose input is state that a setup constructor would clear */
static std::vector<SetupSpec> g_postsetup_specs;
/* `setupsnapshot` (unit file, 2026-10-26, AGL#3): the `setup` lines run once per tested function, and every input
 * starts from the captured post-setup state (see setup_snapshot_* before run()) */
static bool g_setup_snapshot = false;
/* -snapcheck N (2026-10-28): besides the first input of a run, every input whose index is a multiple of N runs the
 * setups for real and checks the snapshot (default 500, as before; 0: the first input only) */
static long g_snapcheck = 500;
/* `poke FUNC|* ADDR|rN+OFF u8|u16|u32|f32 VALUE`: after the setups and before FUNC runs (both sides), the value is
 * stored there, e.g. to select a registered mode in an object a setup constructor initialised */
struct PokeSpec { uint32_t func; int base_reg; uint32_t off; uint32_t size; std::vector<uint32_t> values; };
static std::vector<PokeSpec> g_poke_specs;
/* `nestedret TARGET VALUE`: inside a real callee, a nested call to TARGET returns VALUE in r3
 * instead of a random answer */
static std::unordered_map<uint32_t, uint32_t> g_nestedret;
/* `ret!` / `fret!`: steering that always applies (no 1-in-8 escape), also for nested calls inside
 * real callees */
static std::set<uint32_t> g_ret_strict, g_fret_strict;
/* fretf64[!] ADDR v1,...: exact double f1 results of a mocked callee (never narrowed to single) */
static std::unordered_map<uint32_t, std::vector<double>> g_fret64_picks;
static std::set<uint32_t> g_fret64_strict;
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
/* mockset TARGET rDST rVAL rSIZE: the generated mock of a byte fill (OSBlockSet C0009990, memset) really stores the low
 * byte of VAL SIZE times at DST and returns DST, at top level and inside `real` callees (AGL#3/AGL9 requests,
 * 2026-10-24) */
struct SetSpec { unsigned dst, val, size; };
static std::unordered_map<uint32_t, SetSpec> g_set_specs;
/* mockzero TARGET rDST rSIZE: the generated mock of DCZeroRange (coreinit C00088C8) really zeroes the cache lines it
 * covers, with the shipping port's formula (runtime/src/hle/coreinit_mem.cpp): from DST&~31 for
 * ((DST&31)+SIZE+31)&~31 bytes. Top level and inside `real` callees (AGL#14 request, 2026-10-24). */
struct ZeroSpec { unsigned dst, size; };
static std::unordered_map<uint32_t, ZeroSpec> g_zero_specs;
/* mocksurfacesize TARGET: GX2CalcSurfaceSizeAndAlignment (C00060D0). The mock rewrites the 0x74-byte GX2Surface at r3
 * with the shipping port's own formula (runtime/third_party/cemu/gx2_surface_calc.cpp via src/surface_calc.cpp):
 * tileMode, numLevels, swizzle, imageSize, alignment, pitch, mipSize, mipOffset. Top level and nested (AGL#8
 * request, 2026-10-24). Descriptors outside the modelled domain abort the input (surface_calc.cpp): dim > 7,
 * tileMode > 16, format type 0 or unknown flag bits, aa > 3, width/height > 16384, depth 0 or > 2048, and numLevels > 1
 * with width == height == 0 (non-3D; the level count would become 0 and the outputs stay unwritten). Width/height 0 are
 * modelled since 2026-10-30: the formula clamps each level's extent to max(x >> level, 1), so
 * the sizes equal the 1-pixel case and the width/height fields stay 0. */
static std::set<uint32_t> g_surface_size;
/* mocklookup TARGET rIDX BASE COUNT: the generated mock of a read-only word-table lookup returns in r3 the big-endian
 * word at BASE + 4*rIDX (guest memory, normally the image), at top level and inside `real` callees. An index >= COUNT
 * aborts the input ("outside the declared table"). For one-load helpers that stay opaque (2026-10-24). */
struct LookupSpec { unsigned idx; uint32_t base, count; };
static std::unordered_map<uint32_t, LookupSpec> g_lookup_specs;
extern "C" int verify_calc_surface(uint8_t* bytes); /* surface_calc.cpp: 0 = done, else outside the modelled domain */
/* mockcopysurface TARGET: GX2CopySurface (C0006120; r3 src surface, r4 srcMip, r5 srcSlice, r6 dst surface,
 * r7 dstMip, r8 dstSlice). The mock re-tiles the source level into the destination level in guest memory with the
 * shipping port's CPU path (runtime/src/gfx/metal_surfaces.mm copy_surface_impl, via src/surface_calc.cpp
 * verify_copy_surface): per element (y outer, x inner) bytesPerBlock bytes from the source offset to the destination
 * offset, each computed by the Cemu address library for the surface's own tiling and swizzle. Opt-in, top level and
 * nested, like mocksurfacesize: the descriptors and the source bytes are read before the call's epoch, the destination
 * bytes written after it (copies overlapping their own output read the bytes written by earlier elements, as the
 * port's in-place memcpy loop does). Outside the modelled domain the input aborts: a null descriptor, a descriptor
 * outside the mocksurfacesize domain, a level above 13 or a level base address of 0; a copy larger than the guest
 * operation budget aborts as an over-budget memmove does. */
static std::set<uint32_t> g_copy_surface;
extern "C" int verify_copy_surface(const uint8_t* src74, uint32_t srcMip, uint32_t srcSlice, const uint8_t* dst74, uint32_t dstMip,
                                   uint32_t dstSlice, uint32_t* sbase, uint32_t* dbase, uint32_t* bytesPerBlock,
                                   void (*emit)(void*, uint32_t, uint32_t), void* ctx);
struct CopySurfacePlan { std::vector<std::pair<uint32_t, uint8_t>> writes; std::vector<uint32_t> from; };
static uint64_t g_set_calls, g_set_bytes, g_surface_calls;
/* mockretain TARGET rN SIZE: the callee keeps the pointer rN of a call and, on its next generated call, writes
 * SIZE deterministic bytes through the pointer kept from the previous one (a callback/descriptor retained
 * across calls). Both sides write through their own retained pointer, so stack objects at different addresses
 * are normalised. Per input run; nested calls are not affected. */
struct RetainSpec { unsigned reg, size; };
static std::unordered_map<uint32_t, RetainSpec> g_retain_specs;
static std::unordered_map<uint32_t, uint32_t> g_retained; /* target -> pointer kept from its previous call */
static uint64_t g_move_calls, g_move_bytes, g_utf16_fail, g_utf16_length[4097];


/* a mock model's domain check rejected the call to TARGET: the
 * side ends here as an abort (as before: `why`, aborts on both sides never count), but the run remembers which call the
 * model rejected. The top-level call is already recorded (a nested one ends inside its top-level `real` call), so the
 * comparison sees the calls up to and including it: a rejection on one side only is a call difference (an ordinary
 * call-argument difference, or `argument outside the modelled domain on the candidate side only`), and when both sides
 * rejected, a difference in the calls before the rejection is reported instead of being hidden by the double abort.
 * In setups the old abort only. */
[[noreturn]] static void model_reject(uint32_t target, const char* why) {
    V.abort_reason = why;
    if (V.can_abort && !g_setup_what) {
        V.mrej = true;
        V.mrej_target = target;
        V.mrej_depth = V.depth;
        V.mrej_in = V.real_chain.empty() ? 0 : V.real_chain.back();
    }
    longjmp(V.abort_jmp, 1);
}
/* mocklookup: r3 = table word (read at call time; the table is read-only image data) */
static void apply_lookup(Cpu* c, uint32_t target, const uint32_t* argr) {
    auto lk = g_lookup_specs.find(target);
    if (lk == g_lookup_specs.end()) return;
    uint32_t i = argr[lk->second.idx];
    if (i >= lk->second.count) model_reject(target, "mocklookup: index outside the declared table");
    uint32_t a = lk->second.base + 4 * i;
    V.depth++;
    uint32_t w = (uint32_t)peek8(a) << 24 | (uint32_t)peek8(a + 1) << 16 | (uint32_t)peek8(a + 2) << 8 | peek8(a + 3);
    V.depth--;
    c->r[3] = w;
    if (g_trace) printf("        mocklookup %08X index %X -> %08X\n", target, i, w);
}
/* mockthreadspecific SET GET THREAD [CUR]: the shipping
 * port's HLE (runtime/src/threads.cpp:67 kSpecific = 0x57C, :763-764). OSSetThreadSpecific(key, value) (SET, C000A090)
 * stores r4 big-endian at THREAD+0x57C+4*key, OSGetThreadSpecific(key) (GET, C0009CC8; also its stub 028FDF40) returns
 * that word, and OSGetCurrentThread (CUR, C0009BC8, optional) returns THREAD. THREAD is a fixture-owned OSThread block,
 * the one current thread of every input. Each role takes a comma-separated target list (an import slot and its stubs).
 * At top level after the call's clobber epoch, and inside `real` callees, setups and postsetups. The slots are plain
 * guest memory (setupsnapshot, snapcache/refcache and the net effect cover them) and are kept `stable` (no mock epoch
 * clobbers them; stores to them stay compared). A key >= 16 is outside the modelled domain (a model rejection: the
 * input aborts when both sides make the call). */
static const uint32_t kTlsSpecific = 0x57C, kTlsKeys = 16;
static std::unordered_map<uint32_t, int> g_tls_role; /* target -> 0 SET, 1 GET, 2 CUR */
static uint32_t g_tls_thread;
static bool apply_tls(Cpu* c, uint32_t target, const uint32_t* argr) {
    auto it = g_tls_role.find(target);
    if (it == g_tls_role.end()) return false;
    if (it->second == 2) {
        c->r[3] = g_tls_thread;
        if (g_trace) printf("        mockthreadspecific %08X current thread -> %08X\n", target, g_tls_thread);
        return true;
    }
    uint32_t key = argr[3];
    if (key >= kTlsKeys) model_reject(target, "mockthreadspecific: thread-specific key outside 0..15");
    uint32_t a = g_tls_thread + kTlsSpecific + 4 * key;
    V.depth++;
    if (it->second == 0) {
        uint32_t v = argr[4];
        for (int j = 0; j < 4; j++) patch8(a + j, (uint8_t)(v >> (24 - 8 * j)));
        V.depth--;
        if (g_trace) printf("        mockthreadspecific %08X set key %u <- %08X (at %08X)\n", target, key, v, a);
    } else {
        uint32_t w = (uint32_t)peek8(a) << 24 | (uint32_t)peek8(a + 1) << 16 | (uint32_t)peek8(a + 2) << 8 | peek8(a + 3);
        V.depth--;
        c->r[3] = w;
        if (g_trace) printf("        mockthreadspecific %08X get key %u -> %08X (at %08X)\n", target, key, w, a);
    }
    return true;
}
/* the `opbudget` cap of one mockset/mockzero/memmove operation; under -v every operation over 1 MB is listed */
static void op_budget(const char* op, uint32_t target, uint32_t dst, uint32_t size, uint32_t lr) {
    if (g_verbose && size > 0x100000u)
        fprintf(stderr, "opbudget: %s %08X dst %08X size %X lr %08X side=%s depth=%d seed=%llu%s\n", op, target, dst, size, lr,
                V.is_candidate ? "cand" : "orig", V.depth, (unsigned long long)V.seed, size > g_op_budget ? " (over the budget)" : "");
    if (size > g_op_budget) { V.abort_reason = std::string(op) + " exceeds guest operation budget"; longjmp(V.abort_jmp, 1); }
}
/* mockzero: whole cache lines zeroed (after the call's mock epoch at top level) */
static void apply_zero(uint32_t target, const uint32_t* argr, uint32_t lr) {
    auto zs = g_zero_specs.find(target);
    if (zs == g_zero_specs.end()) return;
    uint32_t dst = argr[zs->second.dst], size = argr[zs->second.size];
    op_budget("mockzero", target, dst, size, lr);
    uint32_t start = dst & ~31u, len = ((dst & 31u) + size + 31u) & ~31u;
    V.depth++;
    for (uint32_t j = 0; j < len; j++) { tick(); patch8(start + j, 0); }
    V.depth--;
    if (!V.is_candidate) { g_set_calls++; g_set_bytes += len; }
    if (g_trace) printf("        mockzero %08X lines %08X size %X\n", target, start, len);
}
/* mockset: the byte fill itself (after the call's mock epoch at top level) */
static void apply_set(Cpu* c, uint32_t target, const uint32_t* argr) {
    auto ss = g_set_specs.find(target);
    if (ss == g_set_specs.end()) return;
    uint32_t dst = argr[ss->second.dst], size = argr[ss->second.size];
    uint8_t v = (uint8_t)argr[ss->second.val];
    op_budget("memset", target, dst, size, c->lr);
    V.depth++;
    for (uint32_t j = 0; j < size; j++) { tick(); patch8(dst + j, v); }
    V.depth--;
    c->r[3] = dst; /* OSBlockSet / memset return the destination */
    if (!V.is_candidate) { g_set_calls++; g_set_bytes += size; }
    if (g_trace) printf("        mockset %08X dst %08X val %02X size %X\n", target, dst, v, size);
}
/* mocksurfacesize: read the descriptor (before the epoch at top level), compute, write all 0x74 bytes back */
static bool surface_read(uint32_t target, uint32_t at, uint8_t* bytes) {
    if (!g_surface_size.count(target)) return false;
    if (at == 0) model_reject(target, "mocksurfacesize: null surface");
    V.depth++;
    for (uint32_t j = 0; j < 0x74; j++) bytes[j] = peek8(at + j);
    V.depth--;
    if (verify_calc_surface(bytes) != 0) model_reject(target, "mocksurfacesize: surface descriptor outside the modelled domain");
    return true;
}
static void surface_write(uint32_t target, uint32_t at, const uint8_t* bytes) {
    V.depth++;
    for (uint32_t j = 0; j < 0x74; j++) patch8(at + j, bytes[j]);
    V.depth--;
    if (!V.is_candidate) g_surface_calls++;
    if (g_trace) printf("        mocksurfacesize %08X surface %08X -> imageSize %X alignment %X pitch %X\n", target, at,
                        (unsigned)bytes[0x20] << 24 | bytes[0x21] << 16 | bytes[0x22] << 8 | bytes[0x23],
                        (unsigned)bytes[0x38] << 24 | bytes[0x39] << 16 | bytes[0x3A] << 8 | bytes[0x3B],
                        (unsigned)bytes[0x3C] << 24 | bytes[0x3D] << 16 | bytes[0x3E] << 8 | bytes[0x3F]);
}

/* mockcopysurface: the plan (destination bytes in write order) from the state before the epoch */
static bool copy_surface_read(uint32_t target, const uint32_t* argr, uint32_t lr, CopySurfacePlan& plan) {
    if (!g_copy_surface.count(target)) return false;
    uint32_t sa = argr[3], da = argr[6];
    if (sa == 0 || da == 0) model_reject(target, "mockcopysurface: null surface");
    uint8_t sb[0x74], db[0x74];
    V.depth++;
    for (uint32_t j = 0; j < 0x74; j++) { sb[j] = peek8(sa + j); db[j] = peek8(da + j); }
    V.depth--;
    std::vector<std::pair<uint32_t, uint32_t>> el;
    uint32_t sbase = 0, dbase = 0, bpb = 0;
    if (verify_copy_surface(sb, argr[4], argr[5], db, argr[7], argr[8], &sbase, &dbase, &bpb,
                            [](void* ctx, uint32_t so, uint32_t dofs) { ((std::vector<std::pair<uint32_t, uint32_t>>*)ctx)->emplace_back(so, dofs); },
                            &el) != 0)
        model_reject(target, "mockcopysurface: surface copy outside the modelled domain");
    op_budget("memmove", target, dbase, (uint32_t)std::min<uint64_t>((uint64_t)el.size() * bpb, 0xFFFFFFFFull), lr);
    std::unordered_map<uint32_t, uint8_t> written;
    plan.writes.reserve(el.size() * bpb);
    V.depth++;
    for (auto& [so, dofs] : el)
        for (uint32_t j = 0; j < bpb; j++) {
            tick();
            uint32_t from = sbase + so + j, to = dbase + dofs + j;
            auto w = written.find(from);
            uint8_t v = w != written.end() ? w->second : peek8(from);
            written[to] = v;
            plan.writes.emplace_back(to, v);
            if (g_trace) plan.from.push_back(from);
        }
    V.depth--;
    return true;
}
static void copy_surface_write(uint32_t target, const CopySurfacePlan& plan) {
    V.depth++;
    for (auto& [a, v] : plan.writes) { tick(); patch8(a, v); }
    V.depth--;
    if (g_trace) { /* every byte (source -> destination = value) up to 0x4000 bytes (copysurface_test.py reads them) */
        printf("        mockcopysurface %08X %zu bytes\n", target, plan.writes.size());
        if (plan.writes.size() <= 0x4000)
            for (size_t i = 0; i < plan.writes.size(); i++)
                printf("        mockcopysurface byte %08X -> %08X = %02X\n", plan.from[i], plan.writes[i].first, plan.writes[i].second);
    }
}

/* alloc / allocfixed: distinct deterministic blocks from BASE in call order (identical on both sides) */
static void alloc_steer(Cpu* c, uint32_t target, const uint32_t* argr) {
    auto al = g_alloc_specs.find(target);
    if (al == g_alloc_specs.end()) return;
    uint32_t size = al->second.sizereg ? argr[al->second.sizereg] : al->second.fixed, a = al->second.align;
    if (al->second.alignreg) {
        a = argr[al->second.alignreg];
        if ((int32_t)a < 0) model_reject(target, "alloc alignment negative (tail allocation not modelled)");
        if (a == 0 || (a & (a - 1))) model_reject(target, "alloc alignment not a power of two");
    }
    if (size > 0x1000000u) model_reject(target, "alloc size exceeds 16 MB");
    if (auto af = g_alloc_fail.find(target); af != g_alloc_fail.end()) {
        uint32_t n = ++g_alloc_calls[target];
        uint64_t h = V.seed * 0x9E3779B97F4A7C15ull ^ (uint64_t)target * 0xBF58476D1CE4E5B9ull;
        h ^= h >> 31; h *= 0x94D049BB133111EBull; h ^= h >> 29;
        if (af->second[h % af->second.size()] == n) {
            c->r[3] = 0;
            if (g_trace) printf("        %salloc %08X -> 00000000 (allocfail: call %u)\n", V.depth ? "nested " : "", target, n);
            return;
        }
    }
    uint32_t& cur = g_alloc_cursor.try_emplace(target, al->second.base).first->second;
    uint64_t start = ((uint64_t)cur + a - 1) & ~(uint64_t)(a - 1);
    uint64_t end = start + (((uint64_t)size + a - 1) & ~(uint64_t)(a - 1));
    if (end > 0xFFFFFFFFull) { V.abort_reason = "alloc cursor past the end of the address space"; longjmp(V.abort_jmp, 1); }
    c->r[3] = (uint32_t)start;
    cur = (uint32_t)end;
    if (g_trace) printf("        %salloc %08X -> %08X (size %X align %X)\n", V.depth ? "nested " : "", target, c->r[3], size, a);
}
/* ------------------------------------------------------------------ osmem: the port HLE's OS/MEM backend (opt-in) */
/* `osmem hle [dataend=HEX] [sysinfo=HEX]` / `osmem PART...` (unit file; 2026-11-01): the shipping port's HLE answers for the coreinit MEM heaps, the loader data-import words, the
 * GX2 size/context imports the startup reads, and OSGetSystemInfo, so the ORIGINAL startup (hdAppMain 02005EA8 ->
 * Framework::run -> TaskMgr root prepare) can run without poked values. Parts (`hle` = all of them):
 *   heaps    coreinit MEM (runtime/src/hle/coreinit_mem.cpp): ExpHeap first-fit (alloc/take/release/largest/total_free,
 *            head and tail allocation, any size), FrmHeap head/tail, mem_setup_heaps' boot layout (default ExpHeap at
 *            (data_end + 0xFFF) & ~0xFFF up to kMem2End 0x50000000, MEM1 FrmHeap 0xF4000000 + 0x02000000, foreground
 *            FrmHeap 0xE0000000 + 0x02800000; runtime/src/runtime.h:14-23), the "EXPH"/"FRMH" header words create_exp /
 *            create_frm store, MEMGetBaseHeapHandle(arena) 0 -> MEM1, 1 -> default, 8 -> foreground, else 0, and the
 *            default-heap functions behind the data imports (default_alloc align 0x40, default_alloc_ex, default_free).
 *            Bookkeeping is host-side as in the port; it is part of the run state (setupsnapshot, -fastreset, -snapcache).
 *   data     the loader's data-import words (runtime/src/main.cpp init_data_imports + coreinit_mem.cpp
 *            mem_init_data_imports): value slots C1000000..C1009FFF (gen/table.c) are zero (the port's guest memory is
 *            zero-initialised) except MEMAllocFromDefaultHeap/Ex/MEMFreeToDefaultHeap = the host-function addresses
 *            register_host hands out first (C2000000, C2000008, C2000010: core.cpp kHleFuncBase, 8-byte steps, called in
 *            that order right after dispatch::init), __gh_FOPEN_MAX = 20, environ = its slot + 0x10.
 *   gx2      GX2CalcSurfaceSizeAndAlignment = the existing mocksurfacesize model; GX2CalcDepthBufferHiZInfo and
 *            GX2CalcColorBufferAuxInfo store 0x1000 at *r4 and 0x100 at *r5 (gx2_resources.cpp); GX2Init and
 *            GX2SetContextState have no guest effect; GX2SetupContextStateEx writes its two host-endian restore words at
 *            ctx+0x9800 (gx2_core.cpp); GX2GetContextStateDisplayList stores ctx+0x9800 / 8 through r4 / r5 when nonzero;
 *            GX2CalcTVSize / GX2CalcDRCSize store the scan-buffer size (w*h*4*max(buffering,1), TV 854x480 / 1280x720 /
 *            1920x1080 by mode) at *r6 and 0 at *r7 (gx2_core.cpp:889-899); the state setters the startup calls (GX2SetTVBuffer,
 *            DRCBuffer, TV/DRCScale, SwapInterval, TargetChannelMasks, StencilMask, PolygonControl, DepthStencilControl,
 *            ColorControl, BlendControl, BlendConstantColor, AlphaToMask, AlphaTest) have no guest effect and keep r3 while
 *            no display list is open (gx2_state.cpp set_reg -> gx2_core.cpp emit; inside a recording they stay generic).
 *   bss      the loader's zero fill of .bss (core.cpp load_rpx: memset of the NOBITS section 0x101FCE00 .. data_end): untouched
 *            bytes there start as 0 instead of generated words (not `stable`: game code writes it; field/fill lines win).
 *   zero     the port's zero-initialised guest memory (core.cpp: the 4 GB reservation is anonymous zeroed memory) for the
 *            heap arenas the backend hands out: the default heap past its header up to 0x50000000, MEM1 and the
 *            foreground bucket past their headers, and the runtime bump region. Untouched bytes start as 0, lazily.
 *   ghs      the GHS libc lock imports (runtime/src/threads.cpp:860-869, coreinit_misc.cpp:308): host-side locks with no guest
 *            effect (__ghsLock/Unlock, __ghs_flock_file/funlock_file/flock_destroy, __ghs_mtx_dst/lock/unlock, __gh_set_errno);
 *            __ghs_flock_ptr returns and __ghs_mtx_init stores (*r3) a fresh zeroed runtime_alloc block (4 / 8 bytes, align 16),
 *            here from a fixture bump region 0x6FF00000.. (the port's addresses depend on its boot order).
 *   os       the other OS imports the startup calls, as the port answers them: OSSetExceptionCallback -> 0 (threads.cpp:766),
 *            OSInitMutex stores "mUtX" and OSInitMessageQueue "mSgQ" at r3 (threads.cpp:844-853, 956-967; the rest is host
 *            state), OSLockMutex/OSUnlockMutex no guest effect, OSTryLockMutex -> 1 (one guest thread), FSInit /
 *            FSSetStateChangeNotification / ProcUIInit no effect, FSAddClient / SAVEInit -> 0 (hle/fs.cpp:188-194, 298;
 *            system_stubs.cpp:69), and the nn::olv constructors the port does not implement -> 0 (core.cpp hle_unimplemented).
 *   fs       (opt-in, NOT in `hle`) coreinit FS read-only over files declared with `osmemfile GUESTPATH SHA256|absent`, as
 *            runtime/src/hle/fs.cpp answers them (no content mods, no language-pack redirect); see the README row.
 *   uc       (opt-in, NOT in `hle`) UCOpen / UCClose / UCReadSysConfig as hle/coreinit_misc.cpp answers them with no
 *            language setting (cafe.language 1); see the README row.
 *   pad      (opt-in, NOT in `hle`) the vpad / vpadbase / padscore imports as hle/system_stubs.cpp and hle/padscore.cpp
 *            answer them (src/pad_model.h), the buttons from the `padsched` press schedule; `padpro` = the Pro Controller
 *            mode; see the README row.
 *   act      (opt-in, NOT in `hle`) the five nn::act imports as hle/system_stubs.cpp answers them (Initialize 0, GetSlotNo
 *            1, ...; r3 only); see the README row.
 *   dynload  (opt-in, NOT in `hle`) OSDynLoad_Acquire / FindExport / Release / SetAllocator as hle/coreinit_misc.cpp
 *            answers them, and the erreula exports FindExport hands out (register_host addresses C2000018.., answered by
 *            hle/erreula.cpp's state machine; src/dynload_model.h); see the README row.
 *   portconst (opt-in, NOT in `hle`) constant answers as the port's HLE gives them, r3 only (src/portans_model.h); see the
 *            README row.
 *   procui   (opt-in, NOT in `hle`) ProcUIProcessMessages -> 0 (foreground), as hle/system_stubs.cpp answers it, r3 only; see the
 *            README row.
 *   swkbd    (opt-in on top of `dynload`) module swkbd's exports: all handed out, the ones the route reaches answered as
 *            hle/swkbd.cpp does, a call to any other a model rejection; see the README row.
 *   sysinfo  OSGetSystemInfo (coreinit_misc.cpp) returns one 0x20-byte block: bus clock 248625000, core clock 1243125000,
 *            the rest zero. The port takes it from its runtime bump allocator; its address there depends on the boot
 *            order, so the block lives at a fixed runtime-region address here (default 0x6FFE0000, `sysinfo=HEX`).
 * Every modelled import leaves the registers as the HLE does (r3 = the HLE result, unchanged for void functions; no other
 * register is written), at top level (after the call's clobber epoch, like the other models) and inside `real` callees,
 * setups and postsetups. The initial words (heap headers, import words, sysinfo block) are input state, identical on both
 * sides, and `stable`. A spec may not name a modelled target in another mock directive (exit 2). */
static bool dl_recording(); /* mockdisplaylist: a display list is open (defined with the GX2 models below) */
namespace osm {
enum : unsigned { P_HEAPS = 1, P_DATA = 2, P_GX2 = 4, P_SYSINFO = 8, P_BSS = 16, P_GHS = 32, P_ZERO = 64, P_OS = 128, P_ALL = 255, P_FS = 256 /* not in `hle` */, P_UC = 512 /* not in `hle` */,
                P_THREADS = 1024 /* not in `hle`: gthr_sched.inc */, P_AX = 2048 /* not in `hle`, needs threads: ax_model.h */,
                P_PAD = 4096 /* not in `hle`: pad_model.h */, P_ACT = 8192 /* not in `hle`: dynload_model.h */,
                P_DYNLOAD = 16384 /* not in `hle`: dynload_model.h */, P_SWKBD = 32768 /* not in `hle`, needs dynload: dynload_model.h */,
                P_PROCUI = 65536 /* not in `hle`: dynload_model.h */,
                P_PORTCONST = 131072 /* not in `hle`: portans_model.h */,
                P_PORTMEM = 262144 /* not in `hle`, needs threads: portans_model.h */ };
static unsigned parts = 0;
static uint32_t data_end = 0x104DA1C8u;   /* cking.rpx: max end of the allocated MEM2 sections (core.cpp load_rpx); osmem_test.py checks it */
static uint32_t sysinfo = 0x6FFE0000u;
static const uint32_t bss_start = 0x101FCE00u;    /* cking.rpx .bss (NOBITS) 0x101FCE00 + 0x2DD3C8 = data_end; osmem_test.py checks it */
static const uint32_t kMem2End = 0x50000000u, kMem1 = 0xF4000000u, kMem1Size = 0x02000000u, kFg = 0xE0000000u,
                      kFgSize = 0x02800000u, kHeader = 0x40u;
static const uint32_t kHostAlloc = 0xC2000000u, kHostAllocEx = 0xC2000008u, kHostFree = 0xC2000010u;
/* function import slots (gen/imports.json) */
static const uint32_t S_CREATE_EXP = 0xC00096D8u, S_DESTROY_EXP = 0xC0009700u, S_ALLOC_EXP = 0xC0009698u, S_FREE_EXP = 0xC0009748u,
                      S_ALLOCATABLE_EXP = 0xC0009778u, S_TOTALFREE_EXP = 0xC00097D8u, S_CREATE_FRM = 0xC00096E0u,
                      S_DESTROY_FRM = 0xC0009708u, S_ALLOC_FRM = 0xC00096A0u, S_ALLOCATABLE_FRM = 0xC0009780u,
                      S_BASE_HANDLE = 0xC0009790u;
static const uint32_t S_SURFACE = 0xC00060D0u, S_HIZ = 0xC00060B0u, S_COLORAUX = 0xC00060A0u, S_GX2INIT = 0xC00063A8u,
                      S_SETUPCTX = 0xC0006A60u, S_SETCTX = 0xC00067E0u, S_GETCTXDL = 0xC00061F0u;
static const uint32_t S_SYSINFO = 0xC0009C80u;
static const uint32_t S_UCOPEN = 0xC000A430u, S_UCCLOSE = 0xC000A408u, S_UCREAD = 0xC000A448u; /* part `uc` */
static const uint32_t S_SET_ERRNO = 0xC000A770u, S_GHSLOCK = 0xC000A778u, S_GHSUNLOCK = 0xC000A780u, S_FLOCK_DESTROY = 0xC000A7A0u,
                      S_FLOCK_FILE = 0xC000A7A8u, S_FLOCK_PTR = 0xC000A7B0u, S_FUNLOCK_FILE = 0xC000A7C0u, S_MTX_DST = 0xC000A7C8u,
                      S_MTX_INIT = 0xC000A7D0u, S_MTX_LOCK = 0xC000A7D8u, S_MTX_UNLOCK = 0xC000A7E0u;
static const uint32_t S_TVSIZE = 0xC00060D8u, S_DRCSIZE = 0xC00060A8u;
/* GX2 state setters the startup calls: host register/scan-out state only (gx2_state.cpp set_reg -> emit, gx2_core.cpp
 * GX2SetTVBuffer/DRCBuffer/TVScale/DRCScale/SwapInterval); emit writes guest memory only into an open display list */
static const uint32_t S_GX2_SETTERS[] = {0xC00069C8u /* GX2SetTVBuffer */, 0xC00067F8u /* GX2SetDRCBuffer */, 0xC00069E0u /* GX2SetTVScale */,
    0xC0006818u /* GX2SetDRCScale */, 0xC00069C0u /* GX2SetSwapInterval */, 0xC00069F0u /* GX2SetTargetChannelMasks */,
    0xC0006990u /* GX2SetStencilMask */, 0xC0006930u /* GX2SetPolygonControl */, 0xC0006848u /* GX2SetDepthStencilControl */,
    0xC00067D0u /* GX2SetColorControl */, 0xC00067B0u /* GX2SetBlendControl */, 0xC00067A0u /* GX2SetBlendConstantColor */,
    0xC0006788u /* GX2SetAlphaToMask */, 0xC0006778u /* GX2SetAlphaTest */};
static bool gx2_setter(uint32_t t) { for (uint32_t s : S_GX2_SETTERS) if (s == t) return true; return false; }
static const uint32_t S_SETEXCB = 0xC000A018u, S_INITMUTEX = 0xC0009D68u, S_LOCKMUTEX = 0xC0009E40u, S_UNLOCKMUTEX = 0xC000A1C8u,
                      S_TRYLOCKMUTEX = 0xC000A198u, S_INITMSGQ = 0xC0009D58u, S_FSINIT = 0xC0008E38u, S_FSADDCLIENT = 0xC0008C78u,
                      S_FSSTATENOTIFY = 0xC0008F58u, S_PROCUIINIT = 0xC0007868u, S_SAVEINIT = 0xC0007450u,
                      S_OLV_POSTDATA = 0xC0004F18u, S_OLV_LISTPARAM = 0xC0004F50u, S_OLV_TOPICDATA = 0xC0004F20u;
/* the port's runtime bump allocator (core.cpp runtime_alloc: zeroed, align 16) for __ghs_flock_ptr / __ghs_mtx_init: its
 * addresses depend on the port's boot order, so a fixture region stands in (0x6FF00000 up to the sysinfo block) */
static const uint32_t kBumpBase = 0x6FF00000u;
/* port gx2/gx2_cmd.h:19-37 OP_SET_CONTEXT = 10 (the restore display list's command word: OP_SET_CONTEXT | 1 << 8) */
static const uint32_t op_set_context = 10;

struct ExpHeap { /* coreinit_mem.cpp struct ExpHeap, line for line (std::map for the block table: lookups only) */
    uint32_t start = 0, end = 0;
    std::map<uint32_t, uint32_t> free;
    std::map<uint32_t, std::pair<uint32_t, uint32_t>> blocks;
    void init(uint32_t s, uint32_t e) { start = s; end = e; free.clear(); blocks.clear(); free[s] = e - s; }
    uint32_t alloc(uint32_t size, int32_t align) {
        size = (size + 3) & ~3u;
        if (size == 0) size = 4;
        bool from_top = align < 0;
        uint32_t a = align < 0 ? 0u - (uint32_t)align : (uint32_t)align;
        if (a < 4) a = 4;
        if (!from_top) {
            for (auto it = free.begin(); it != free.end(); ++it) {
                uint32_t bs = it->first, bsz = it->second;
                uint32_t user = (bs + a - 1) & ~(a - 1);
                if (user + size > bs + bsz) continue;
                take(it, user, size);
                return user;
            }
        } else {
            for (auto it = free.rbegin(); it != free.rend(); ++it) {
                uint32_t bs = it->first, bsz = it->second;
                if (bsz < size) continue;
                uint32_t user = (bs + bsz - size) & ~(a - 1);
                if (user < bs) continue;
                take(std::prev(it.base()), user, size);
                return user;
            }
        }
        return 0;
    }
    void take(std::map<uint32_t, uint32_t>::iterator it, uint32_t user, uint32_t size) {
        uint32_t bs = it->first, bsz = it->second;
        free.erase(it);
        if (user > bs) free[bs] = user - bs;
        uint32_t tail = bs + bsz - (user + size);
        if (tail) free[user + size] = tail;
        blocks[user] = {user, size};
    }
    bool release(uint32_t user) {
        auto it = blocks.find(user);
        if (it == blocks.end()) return false;
        uint32_t s = it->second.first, sz = it->second.second;
        blocks.erase(it);
        auto n = free.emplace(s, sz).first;
        if (n != free.begin()) {
            auto p = std::prev(n);
            if (p->first + p->second == n->first) { p->second += n->second; free.erase(n); n = p; }
        }
        auto nx = std::next(n);
        if (nx != free.end() && n->first + n->second == nx->first) { n->second += nx->second; free.erase(nx); }
        return true;
    }
    uint32_t total_free() const { uint32_t t = 0; for (auto& f : free) t += f.second; return t; }
    uint32_t largest(uint32_t a) const {
        uint32_t best = 0;
        for (auto& f : free) {
            uint32_t user = (f.first + a - 1) & ~(a - 1);
            if (user < f.first + f.second) best = std::max(best, f.first + f.second - user);
        }
        return best & ~3u;
    }
};
/* ---- part `fs` (opt-in, not in `hle`): coreinit FS read-only over declared game files (runtime/src/hle/fs.cpp) ---- */
/* SHA-256 (FIPS 180-4) for the `osmemfile` hash check */
static std::string sha256_hex(const std::vector<uint8_t>& msg) {
    static const uint32_t K[64] = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01,
        0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
        0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da, 0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
        0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070, 0x19a4c116, 0x1e376c08,
        0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
        0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};
    uint32_t H[8] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};
    std::vector<uint8_t> m = msg;
    uint64_t bits = (uint64_t)msg.size() * 8;
    m.push_back(0x80);
    while (m.size() % 64 != 56) m.push_back(0);
    for (int i = 7; i >= 0; i--) m.push_back((uint8_t)(bits >> (8 * i)));
    auto rotr = [](uint32_t x, int n) { return (x >> n) | (x << (32 - n)); };
    for (size_t o = 0; o < m.size(); o += 64) {
        uint32_t w[64];
        for (int i = 0; i < 16; i++) w[i] = (uint32_t)m[o + 4 * i] << 24 | (uint32_t)m[o + 4 * i + 1] << 16 | (uint32_t)m[o + 4 * i + 2] << 8 | m[o + 4 * i + 3];
        for (int i = 16; i < 64; i++) {
            uint32_t s0 = rotr(w[i - 15], 7) ^ rotr(w[i - 15], 18) ^ (w[i - 15] >> 3), s1 = rotr(w[i - 2], 17) ^ rotr(w[i - 2], 19) ^ (w[i - 2] >> 10);
            w[i] = w[i - 16] + s0 + w[i - 7] + s1;
        }
        uint32_t a = H[0], b = H[1], c = H[2], d = H[3], e = H[4], f = H[5], g = H[6], h = H[7];
        for (int i = 0; i < 64; i++) {
            uint32_t S1 = rotr(e, 6) ^ rotr(e, 11) ^ rotr(e, 25), ch = (e & f) ^ (~e & g), t1 = h + S1 + ch + K[i] + w[i];
            uint32_t S0 = rotr(a, 2) ^ rotr(a, 13) ^ rotr(a, 22), mj = (a & b) ^ (a & c) ^ (b & c), t2 = S0 + mj;
            h = g; g = f; f = e; e = d + t1; d = c; c = b; b = a; a = t1 + t2;
        }
        H[0] += a; H[1] += b; H[2] += c; H[3] += d; H[4] += e; H[5] += f; H[6] += g; H[7] += h;
    }
    char out[65];
    for (int i = 0; i < 8; i++) snprintf(out + 8 * i, 9, "%08x", H[i]);
    return out;
}
/* fs.cpp host_path_exact with game_dir = GAME ("game" here); /vol/save has no model ("" = refused) */
static std::string fs_host_exact(const std::string& guest, const std::string& game) {
    std::string p = guest;
    auto map = [&](const char* prefix, const std::string& root) -> bool {
        size_t n = strlen(prefix);
        if (p.compare(0, n, prefix) == 0) { p = root + p.substr(n); return true; }
        return false;
    };
    if (map("/vol/content", game + "/content")) return p;
    if (map("/vol/code", game + "/code")) return p;
    if (map("/vol/meta", game + "/meta")) return p;
    if (p.compare(0, 9, "/vol/save") == 0) return "";
    if (!p.empty() && p[0] != '/') return game + "/content/" + p;
    return game + p;
}
static std::string fs_key(const std::string& host) { /* the volumes are case-insensitive (fs.cpp resolve_case) */
    std::string k = host;
    for (char& ch : k) ch = (char)tolower((unsigned char)ch);
    return k;
}
/* fs.cpp fill_stat: the 0x64-byte FSStat */
static void fs_stat_bytes(uint8_t* out, uint64_t size, bool dir) {
    memset(out, 0, 0x64);
    auto w = [&](int o, uint32_t v) { for (int j = 0; j < 4; j++) out[o + j] = (uint8_t)(v >> (24 - 8 * j)); };
    w(0x00, dir ? 0x80000000u : 0x01000000u);
    w(0x04, 0x666);
    w(0x10, dir ? 0 : (uint32_t)size);
    w(0x14, dir ? 0 : (uint32_t)size);
}
struct FsFile { std::string guest, host, key, sha; std::vector<uint8_t> data; bool absent = false; };
static std::vector<FsFile> fs_files;
static const FsFile* fs_find(const std::string& guest) {
    std::string h = fs_host_exact(guest, "game");
    if (h.empty()) return nullptr;
    std::string k = fs_key(h);
    for (const FsFile& f : fs_files) if (f.key == k) return &f;
    return nullptr;
}
/* fs.cpp FSReadFile: fread of size*count bytes from the position, r3 = bytes read / size */
static uint64_t fs_read_len(uint64_t file_size, uint64_t pos, uint32_t size, uint32_t count, uint32_t* ret) {
    uint64_t want = (uint64_t)size * count, have = pos < file_size ? file_size - pos : 0, n = std::min(want, have);
    *ret = (uint32_t)(n / size);
    return n;
}
static const int32_t FS_OK = 0, FS_NOT_FOUND = -6, FS_NOT_DIR = -8, FS_ACCESS_ERROR = -10;
static const uint32_t S_FSOPEN = 0xC0008E98u, S_FSCLOSE = 0xC0008CF8u, S_FSREAD = 0xC0008EC8u, S_FSSETPOS = 0xC0008F48u,
                      S_FSSTAT = 0xC0008DF0u, S_FSSTATFILE = 0xC0008E00u, S_FSINITCMD = 0xC0008E40u, S_FSCMDPRIO = 0xC0008F38u,
                      S_FSVOLSTATE = 0xC0008E30u, S_FSLASTERR = 0xC0008DB0u, S_FSLASTERRVIEW = 0xC0008DB8u, S_FSDELCLIENT = 0xC0008D08u,
                      S_FSSHUTDOWN = 0xC0008F68u, S_FSGETCWD = 0xC0008D58u, S_FSOPENDIR = 0xC0008E88u, S_FSREADDIR = 0xC0008EB8u,
                      S_FSCLOSEDIR = 0xC0008CE8u, S_FSWRITE = 0xC0008F98u;
struct FrmHeap { uint32_t start, end, head, tail; };
struct State {
    std::map<uint32_t, ExpHeap> exp;
    std::map<uint32_t, FrmHeap> frm;
    uint32_t def = 0, mem1 = 0, fg = 0;
    uint32_t bump = kBumpBase;
    std::map<uint32_t, std::pair<uint32_t, uint64_t>> fs_open; /* handle -> (fs_files index, position) */
    uint32_t fs_next = 1;                                     /* fs.cpp g_next_handle */
};
static State st;
/* `pad`: the press schedule (spec), the Pro Controller mode (`padpro`) and the HLE statics + read ordinals (run state) */
static padm::Sched pad_sched;
static bool pad_pro = false, pad_sched_seen = false;
static padm::State pad_st;
/* `dynload`: the OSDynLoad / register_host / erreula HLE statics (run state) */
static dlm::State dl_st;

static void put32(uint32_t a, uint32_t v) { /* big-endian guest store (st32) */
    V.depth++;
    for (int j = 0; j < 4; j++) patch8(a + j, (uint8_t)(v >> (24 - 8 * j)));
    V.depth--;
}
static uint32_t create_exp(uint32_t start, uint32_t size, bool store) {
    ExpHeap& h = st.exp[start];
    h = ExpHeap();
    h.init(start + kHeader, start + size);
    if (store) put32(start, 0x45585048u); /* "EXPH" */
    return start;
}
static uint32_t create_frm(uint32_t start, uint32_t size, bool store) {
    st.frm[start] = FrmHeap{start + kHeader, start + size, start + kHeader, start + size};
    if (store) put32(start, 0x46524D48u); /* "FRMH" */
    return start;
}
static uint32_t default_start() { return (data_end + 0xFFFu) & ~0xFFFu; }
/* mem_setup_heaps(data_end): the boot layout, before every run (the header words are input state, see input_words) */
static void boot() {
    st = State();
    pad_st = padm::State();
    dl_st = dlm::State();
    if (!(parts & P_HEAPS)) return;
    uint32_t s = default_start();
    st.def = create_exp(s, kMem2End - s, false);
    st.mem1 = create_frm(kMem1, kMem1Size, false);
    st.fg = create_frm(kFg, kFgSize, false);
}
static uint32_t exp_alloc(uint32_t heap, uint32_t size, int32_t align) {
    auto it = st.exp.find(heap);
    if (it == st.exp.end()) return 0;
    return it->second.alloc(size, align);
}
static uint32_t frm_alloc(uint32_t heap, uint32_t size, int32_t align) {
    auto it = st.frm.find(heap);
    if (it == st.frm.end()) return 0;
    FrmHeap* h = &it->second;
    uint32_t a = align < 0 ? 0u - (uint32_t)align : (uint32_t)align;
    if (a < 4) a = 4;
    size = (size + 3) & ~3u;
    if (align >= 0) {
        uint32_t p = (h->head + a - 1) & ~(a - 1);
        if (p + size > h->tail) return 0;
        h->head = p + size;
        return p;
    }
    uint32_t p = (h->tail - size) & ~(a - 1);
    if (p < h->head) return 0;
    h->tail = p;
    return p;
}
/* the run state as words (snapshot / -fastreset / -snapcache); empty when the heaps are off */
static std::vector<uint32_t> save() {
    std::vector<uint32_t> v;
    if (!(parts & (P_HEAPS | P_GHS | P_FS | P_PAD | P_DYNLOAD))) return v;
    v.push_back(st.bump);
    v.push_back(st.def); v.push_back(st.mem1); v.push_back(st.fg);
    v.push_back((uint32_t)st.exp.size());
    for (auto& [k, h] : st.exp) {
        v.push_back(k); v.push_back(h.start); v.push_back(h.end);
        v.push_back((uint32_t)h.free.size());
        for (auto& [a, n] : h.free) { v.push_back(a); v.push_back(n); }
        v.push_back((uint32_t)h.blocks.size());
        for (auto& [u, b] : h.blocks) { v.push_back(u); v.push_back(b.first); v.push_back(b.second); }
    }
    v.push_back((uint32_t)st.frm.size());
    for (auto& [k, f] : st.frm) { v.push_back(k); v.push_back(f.start); v.push_back(f.end); v.push_back(f.head); v.push_back(f.tail); }
    v.push_back(st.fs_next);
    v.push_back((uint32_t)st.fs_open.size());
    for (auto& [hd, o] : st.fs_open) { v.push_back(hd); v.push_back(o.first); v.push_back((uint32_t)o.second); v.push_back((uint32_t)(o.second >> 32)); }
    if (parts & P_PAD) {
        v.push_back((uint32_t)pad_st.vpad_reads); v.push_back((uint32_t)(pad_st.vpad_reads >> 32));
        v.push_back((uint32_t)pad_st.kpad_reads); v.push_back((uint32_t)(pad_st.kpad_reads >> 32));
        v.push_back(pad_st.vpad_last_hold); v.push_back(pad_st.kpad_last);
        v.push_back((uint32_t)pad_st.last_tx | (uint32_t)pad_st.last_ty << 16);
    }
    if (parts & P_DYNLOAD) dl_st.save(v);
    return v;
}
static void load(const std::vector<uint32_t>& v) {
    st = State();
    pad_st = padm::State();
    dl_st = dlm::State();
    if (v.empty()) return;
    size_t i = 0;
    auto nx = [&]() { return i < v.size() ? v[i++] : 0u; };
    st.bump = nx();
    st.def = nx(); st.mem1 = nx(); st.fg = nx();
    for (uint32_t n = nx(), k = 0; k < n; k++) {
        ExpHeap& h = st.exp[nx()];
        h.start = nx(); h.end = nx();
        for (uint32_t m = nx(), j = 0; j < m; j++) { uint32_t a = nx(); h.free[a] = nx(); }
        for (uint32_t m = nx(), j = 0; j < m; j++) { uint32_t u = nx(), s = nx(), z = nx(); h.blocks[u] = {s, z}; }
    }
    for (uint32_t n = nx(), k = 0; k < n; k++) {
        uint32_t key = nx();
        FrmHeap f;
        f.start = nx(); f.end = nx(); f.head = nx(); f.tail = nx();
        st.frm[key] = f;
    }
    st.fs_next = nx();
    for (uint32_t n = nx(), k = 0; k < n; k++) {
        uint32_t hd = nx(), fi = nx(), lo = nx(), hi = nx();
        st.fs_open[hd] = {fi, (uint64_t)hi << 32 | lo};
    }
    if (parts & P_PAD) {
        uint32_t lo = nx(), hi = nx();
        pad_st.vpad_reads = (uint64_t)hi << 32 | lo;
        lo = nx(), hi = nx();
        pad_st.kpad_reads = (uint64_t)hi << 32 | lo;
        pad_st.vpad_last_hold = nx();
        pad_st.kpad_last = nx();
        uint32_t t = nx();
        pad_st.last_tx = (uint16_t)t;
        pad_st.last_ty = (uint16_t)(t >> 16);
    }
    if (parts & P_DYNLOAD) dl_st.load(nx);
}
/* the targets a part models (parse-time conflict check, -nulltrap code targets) */
static bool models(uint32_t t) {
    if (parts & P_HEAPS)
        switch (t) {
        case S_CREATE_EXP: case S_DESTROY_EXP: case S_ALLOC_EXP: case S_FREE_EXP: case S_ALLOCATABLE_EXP: case S_TOTALFREE_EXP:
        case S_CREATE_FRM: case S_DESTROY_FRM: case S_ALLOC_FRM: case S_ALLOCATABLE_FRM: case S_BASE_HANDLE:
        case kHostAlloc: case kHostAllocEx: case kHostFree: return true;
        }
    if (parts & P_GX2)
        switch (t) {
        case S_SURFACE: case S_HIZ: case S_COLORAUX: case S_GX2INIT: case S_SETUPCTX: case S_SETCTX: case S_GETCTXDL:
        case S_TVSIZE: case S_DRCSIZE: return true;
        }
    if ((parts & P_GX2) && gx2_setter(t)) return true;
    if ((parts & P_UC) && (t == S_UCOPEN || t == S_UCCLOSE || t == S_UCREAD)) return true;
    if (parts & P_FS)
        switch (t) {
        case S_FSOPEN: case S_FSCLOSE: case S_FSREAD: case S_FSSETPOS: case S_FSSTAT: case S_FSSTATFILE: case S_FSINITCMD:
        case S_FSCMDPRIO: case S_FSVOLSTATE: case S_FSLASTERR: case S_FSLASTERRVIEW: case S_FSDELCLIENT: case S_FSSHUTDOWN:
        case S_FSGETCWD: case S_FSOPENDIR: case S_FSREADDIR: case S_FSCLOSEDIR: case S_FSWRITE:
        case S_FSINIT: case S_FSADDCLIENT: case S_FSSTATENOTIFY: return true;
        }
    if (parts & P_OS)
        switch (t) {
        case S_SETEXCB: case S_INITMUTEX: case S_LOCKMUTEX: case S_UNLOCKMUTEX: case S_TRYLOCKMUTEX: case S_INITMSGQ: case S_FSINIT:
        case S_FSADDCLIENT: case S_FSSTATENOTIFY: case S_PROCUIINIT: case S_SAVEINIT: case S_OLV_POSTDATA: case S_OLV_LISTPARAM:
        case S_OLV_TOPICDATA: return true;
        }
    if ((parts & P_SYSINFO) && t == S_SYSINFO) return true;
    if ((parts & P_THREADS) && gthr::models(t)) return true;
    if ((parts & P_AX) && axm::is_import(t)) return true;
    if ((parts & P_PAD) && padm::is_import(t)) return true;
    if ((parts & P_ACT) && dlm::act_slot(t)) return true;
    if ((parts & P_PROCUI) && dlm::procui_slot(t)) return true;
    if ((parts & P_PORTCONST) && portans::constant(t)) return true;
    if ((parts & P_PORTMEM) && portans::mem_slot(t)) return true;
    if ((parts & P_DYNLOAD) && (dlm::dyn_slot(t) || dl_st.host_index(t) >= 0)) return true; /* handed-out exports: at run time */
    if (parts & P_GHS)
        switch (t) {
        case S_SET_ERRNO: case S_GHSLOCK: case S_GHSUNLOCK: case S_FLOCK_DESTROY: case S_FLOCK_FILE: case S_FLOCK_PTR:
        case S_FUNLOCK_FILE: case S_MTX_DST: case S_MTX_INIT: case S_MTX_LOCK: case S_MTX_UNLOCK: return true;
        }
    return false;
}
/* the initial guest words (identical on both sides, `stable`): addr -> byte, applied with the generated input */
static std::map<uint32_t, uint8_t> init_bytes;
static std::vector<std::pair<uint32_t, std::vector<uint8_t>>> init_runs; /* the same, as contiguous runs (snap_range) */
static void add_word(uint32_t a, uint32_t v) { for (int j = 0; j < 4; j++) init_bytes[a + j] = (uint8_t)(v >> (24 - 8 * j)); }
static void add_zero(uint32_t a, uint32_t n) { g_zero_init.emplace_back(a, a + n); } /* lazily zero (initial_byte) */
static void build_runs() {
    init_runs.clear();
    for (auto& [a, b] : init_bytes) {
        if (init_runs.empty() || init_runs.back().first + init_runs.back().second.size() != a) init_runs.emplace_back(a, std::vector<uint8_t>());
        init_runs.back().second.push_back(b);
    }
}
static void build_init() {
    init_bytes.clear();
    g_zero_init.clear();
    if (parts & P_BSS) add_zero(bss_start, data_end - bss_start); /* load_rpx: memset of the NOBITS section */
    if (parts & P_ZERO) { /* the port's guest memory starts zeroed: the heap arenas and the runtime region stand-ins */
        add_zero(default_start() + kHeader, kMem2End - default_start() - kHeader);
        add_zero(kMem1 + kHeader, kMem1Size - kHeader);
        add_zero(kFg + kHeader, kFgSize - kHeader);
        add_zero(kBumpBase, 0xE0000u);
    }
    if (parts & P_HEAPS) { /* the three boot heaps' header blocks: the magic word, the rest zero (guest memory starts zeroed) */
        add_zero(default_start(), kHeader); add_word(default_start(), 0x45585048u);
        add_zero(kMem1, kHeader); add_word(kMem1, 0x46524D48u);
        add_zero(kFg, kHeader); add_word(kFg, 0x46524D48u);
        g_stable.emplace_back(default_start(), kHeader);
        g_stable.emplace_back(kMem1, kHeader);
        g_stable.emplace_back(kFg, kHeader);
    }
    if (parts & P_DATA) { /* value slots C1000000..C1009FFF (gen/table.c, 0x1000 apart) */
        add_zero(0xC1000000u, 0xA000);
        if (parts & P_HEAPS) { add_word(0xC1000000u, kHostAlloc); add_word(0xC1001000u, kHostAllocEx); add_word(0xC1002000u, kHostFree); }
        add_word(0xC1006000u, 20);                    /* __gh_FOPEN_MAX */
        add_word(0xC1009000u, 0xC1009000u + 0x10);    /* environ: an empty list */
        g_stable.emplace_back(0xC1000000u, 0xA000);
    }
    if (parts & P_THREADS) { /* the main thread's OSThread block as threads.cpp run_main/create_thread write it (id 1, core 1 = attr 2,
                              * priority 16, entry = the RPX entry 028EA120, the harness stack window as its stack) */
        add_zero(gthr::main_obj, gthrm::kSize);
        gthrm::create_words(gthr::main_obj, gthr::kRpxEntry, gthr::kMainStackTop, gthr::kMainStackSize, gthr::kMainPrio, 2, 1,
                            [](uint32_t a, int size, uint32_t v) {
                                if (!v) return;
                                for (int j = 0; j < size; j++) init_bytes[a + j] = (uint8_t)(v >> (8 * (size - 1 - j)));
                            });
        g_stable.emplace_back(gthr::main_obj, gthrm::kSize);
    }
    if (parts & P_THREADS) /* the alarm service thread's OSThread block (6FFE1800) and guest stack (6FFE3000..6FFFF000): zeroed */
        add_zero(0x6FFE1800u, 0x6FFFF000u - 0x6FFE1800u);
    if (parts & P_AX) /* the port's host_alloc region stand-in: GamePad aux buffers, the AX frame thread's block and stack */
        add_zero(gthr::kAxHostLo, gthr::kAxHostHi - gthr::kAxHostLo);
    if (parts & P_SYSINFO) {
        add_zero(sysinfo, 0x20);
        add_word(sysinfo + 0, 248625000u);   /* bus clock */
        add_word(sysinfo + 4, 1243125000u);  /* core clock */
        g_stable.emplace_back(sysinfo, 0x20);
    }
    build_runs();
    std::sort(g_zero_init.begin(), g_zero_init.end());
    for (size_t i = 1; i < g_zero_init.size(); i++) /* merge overlaps (sysinfo= may sit inside another range) */
        if (g_zero_init[i].first < g_zero_init[i - 1].second) {
            g_zero_init[i - 1].second = std::max(g_zero_init[i - 1].second, g_zero_init[i].second);
            g_zero_init.erase(g_zero_init.begin() + i--);
        }
}
static uint32_t bump_alloc(uint32_t target, uint32_t size, uint32_t align = 16) { /* runtime_alloc(size, align): zeroed */
    uint32_t p = (st.bump + align - 1) & ~(align - 1);
    if ((uint64_t)p + size > kBumpBase + 0xE0000u) model_reject(target, "osmem: runtime bump region (0x6FF00000 + 0xE0000) exhausted");
    st.bump = p + size;
    V.depth++;
    for (uint32_t j = 0; j < size; j++) patch8(p + j, 0);
    V.depth--;
    return p;
}
/* part `uc` (opt-in, not in `hle`): coreinit_misc.cpp UCOpen -> 1, UCClose -> 0, UCReadSysConfig(handle, count, items):
 * per 0x54-byte UCSysConfig item (name[64], access, dataType, error +0x48, dataSize +0x4C, dataPtr +0x50) the value by
 * name (cafe.language = console_language() with no WWHD_LANGUAGE / saved setting = 1 English, cafe.cntry_reg 49,
 * cafe.eula_agree 1, cafe.initial_launch 2, parent.enable 0, p_acct1.network_launcher 0, any other name 0); when dataPtr
 * and dataSize are nonzero the data is zero-filled and the value stored big-endian as u8 (size 1), u16 (size 2) or u32
 * (size >= 4; size 3 stays zero); error = 0; r3 = 0 */
static uint32_t uc_value(const std::string& name) {
    if (name == "cafe.language") return 1;
    if (name == "cafe.cntry_reg") return 49;
    if (name == "cafe.eula_agree") return 1;
    if (name == "cafe.initial_launch") return 2;
    return 0; /* parent.enable, p_acct1.network_launcher and every other name */
}
static std::string guest_cstr(uint32_t target, uint32_t ea) { /* mem::read_cstr */
    std::string s;
    if (!ea) return s;
    V.depth++;
    for (uint32_t j = 0;; j++) {
        if (j >= 4096) { V.depth--; model_reject(target, "osmem fs: path string longer than 4096 bytes"); }
        uint8_t b = peek8(ea + j);
        if (!b) break;
        s.push_back((char)b);
    }
    V.depth--;
    return s;
}
static uint32_t fs_call(uint32_t target, const uint32_t* a, bool* ret) {
    *ret = true;
    switch (target) {
    case S_FSOPEN: { /* (client, block, path, mode, *handle) */
        std::string path = guest_cstr(target, a[5]), mode = guest_cstr(target, a[6]);
        if (mode.find_first_of("wa+") != std::string::npos) model_reject(target, "osmem fs: write access not modelled");
        const FsFile* f = fs_find(path);
        if (!f) model_reject(target, "osmem fs: path not declared with osmemfile");
        if (f->absent) return (uint32_t)FS_NOT_FOUND;
        uint32_t hd = st.fs_next++;
        st.fs_open[hd] = {(uint32_t)(f - fs_files.data()), 0};
        put32(a[7], hd);
        return FS_OK;
    }
    case S_FSCLOSE: st.fs_open.erase(a[5]); return FS_OK;
    case S_FSREAD: { /* (client, block, dst, size, count, handle, flag) */
        auto it = st.fs_open.find(a[8]);
        if (it == st.fs_open.end() || a[6] == 0) return 0;
        const FsFile& f = fs_files[it->second.first];
        uint32_t r;
        uint64_t n = fs_read_len(f.data.size(), it->second.second, a[6], a[7], &r);
        if (n > 0x10000000ull) model_reject(target, "osmem fs: read larger than 256 MB");
        V.depth++;
        for (uint64_t j = 0; j < n; j++) { tick(); patch8(a[5] + (uint32_t)j, f.data[it->second.second + j]); }
        V.depth--;
        it->second.second += n;
        return r;
    }
    case S_FSSETPOS: { auto it = st.fs_open.find(a[5]); if (it == st.fs_open.end()) return (uint32_t)FS_ACCESS_ERROR; it->second.second = a[6]; return FS_OK; }
    case S_FSSTAT: case S_FSSTATFILE: {
        const FsFile* f = nullptr;
        if (target == S_FSSTAT) {
            f = fs_find(guest_cstr(target, a[5]));
            if (!f) model_reject(target, "osmem fs: path not declared with osmemfile");
            if (f->absent) return (uint32_t)FS_NOT_FOUND;
        } else {
            auto it = st.fs_open.find(a[5]);
            if (it == st.fs_open.end()) return (uint32_t)FS_NOT_FOUND;
            f = &fs_files[it->second.first];
        }
        uint8_t b[0x64];
        fs_stat_bytes(b, f->data.size(), false);
        V.depth++;
        for (int j = 0; j < 0x64; j++) patch8(a[6] + j, b[j]);
        V.depth--;
        return FS_OK;
    }
    case S_FSINITCMD: V.depth++; for (uint32_t j = 0; j < 0xA80; j++) patch8(a[3] + j, 0); V.depth--; *ret = false; return 0;
    case S_FSVOLSTATE: return 1;
    case S_FSCMDPRIO: case S_FSLASTERR: case S_FSLASTERRVIEW: case S_FSDELCLIENT: case S_FSADDCLIENT: return 0;
    case S_FSGETCWD: { /* mem::write_cstr(r5, "/vol/content", r6) */
        static const char cwd[] = "/vol/content";
        uint32_t n = (uint32_t)std::min<size_t>(sizeof cwd - 1, (size_t)(a[6] - 1));
        V.depth++;
        for (uint32_t j = 0; j < n; j++) patch8(a[5] + j, (uint8_t)cwd[j]);
        patch8(a[5] + n, 0);
        V.depth--;
        return FS_OK;
    }
    case S_FSOPENDIR: case S_FSREADDIR: case S_FSCLOSEDIR: model_reject(target, "osmem fs: directories not modelled");
    case S_FSWRITE: model_reject(target, "osmem fs: writes not modelled");
    default: *ret = false; return 0; /* FSInit, FSShutdown, FSSetStateChangeNotification */
    }
}
/* `pad`: guest memory as the HLE's ld16/st8/st16/st32/memset (callee effects, like put32) */
struct PadEnv {
    uint16_t ld16(uint32_t a) { V.depth++; uint16_t v = (uint16_t)(peek8(a) << 8 | peek8(a + 1)); V.depth--; return v; }
    void st8(uint32_t a, uint32_t v) { V.depth++; patch8(a, (uint8_t)v); V.depth--; }
    void st16(uint32_t a, uint32_t v) { V.depth++; patch8(a, (uint8_t)(v >> 8)); patch8(a + 1, (uint8_t)v); V.depth--; }
    void st32(uint32_t a, uint32_t v) { put32(a, v); }
    void zero(uint32_t a, uint32_t n) { V.depth++; for (uint32_t j = 0; j < n; j++) patch8(a + j, 0); V.depth--; }
};
static bool pad_call(Cpu* c, uint32_t target, const uint32_t* a) {
    PadEnv e;
    uint32_t r = 0;
    const uint64_t vk = pad_st.vpad_reads, kk = pad_st.kpad_reads;
    int k = padm::call(e, pad_st, pad_sched, pad_pro, target, a, &r);
    if (k < 0) return false;
    c->r[3] = k ? r : a[3];
    if (g_trace) {
        if (pad_st.vpad_reads != vk || pad_st.kpad_reads != kk)
            printf("        %spad %s read #%llu (chan %u, buffer %08X) hold %08X -> %08X\n", V.depth ? "nested " : "", padm::import_name(target),
                   (unsigned long long)(pad_st.vpad_reads != vk ? vk : kk), a[3], a[4],
                   pad_st.vpad_reads != vk ? pad_st.vpad_last_hold : pad_st.kpad_last, c->r[3]);
        else if (k) printf("        %spad %s r3=%08X r4=%08X r5=%08X -> %08X\n", V.depth ? "nested " : "", padm::import_name(target), a[3], a[4], a[5], r);
        else printf("        %spad %s r3=%08X r4=%08X r5=%08X (void)\n", V.depth ? "nested " : "", padm::import_name(target), a[3], a[4], a[5]);
    }
    return true;
}
/* `act` / `dynload`: guest memory as the HLE's ld8 / st8 / st32 / runtime_alloc (callee effects, like put32) */
struct DynEnv {
    uint32_t target;
    uint8_t ld8(uint32_t a) { V.depth++; uint8_t v = peek8(a); V.depth--; return v; }
    void st8(uint32_t a, uint32_t v) { V.depth++; patch8(a, (uint8_t)v); V.depth--; }
    void st32(uint32_t a, uint32_t v) { put32(a, v); }
    uint32_t runtime_alloc(uint32_t size) { return bump_alloc(target, size); }
    [[noreturn]] void reject(const char* why) { model_reject(target, why); }
};
static bool dyn_call(Cpu* c, uint32_t target, const uint32_t* a) {
    DynEnv e{target};
    uint32_t r = 0;
    const size_t nh = dl_st.hosts.size();
    const int hk = dl_st.host_index(target);
    int k = dlm::call(e, dl_st, (parts & P_ACT) != 0, (parts & P_DYNLOAD) != 0, (parts & P_SWKBD) != 0, target, a, &r);
    if (k < 0) return false;
    c->r[3] = k ? r : a[3];
    if (g_trace) {
        const char* n = V.depth ? "nested " : "";
        if (hk >= 0) printf("        %sdynload export %08X %s r3=%08X r4=%08X -> %08X%s (erreula state %u, decided %d)\n", n, target,
                            dlm::host_name(dl_st, target).c_str(), a[3], a[4], c->r[3], k ? "" : " (void)", dl_st.err_state, (int)dl_st.err_decided);
        else if (dl_st.hosts.size() != nh)
            printf("        %sdynload %s r3=%08X r4=%08X r5=%08X r6=%08X -> %08X (new export %08X)\n", n, dlm::import_name(target), a[3], a[4], a[5], a[6],
                   c->r[3], dlm::kHleFuncBase + 8 * (dlm::kFirstHost + (uint32_t)nh));
        else printf("        %s%s %s r3=%08X r4=%08X r5=%08X r6=%08X -> %08X%s\n", n, dlm::act_slot(target) ? "act" : "dynload", dlm::import_name(target),
                    a[3], a[4], a[5], a[6], c->r[3], k ? "" : " (void)");
    }
    return true;
}
/* `portmem`: guest memory and the clock as the HLE's (callee effects, like put32) */
struct PortMemEnv {
    uint32_t target;
    uint8_t ld8(uint32_t a) { V.depth++; uint8_t v = peek8(a); V.depth--; return v; }
    uint32_t ld32(uint32_t a) { V.depth++; uint32_t v = (uint32_t)peek8(a) << 24 | (uint32_t)peek8(a + 1) << 16 | (uint32_t)peek8(a + 2) << 8 | peek8(a + 3); V.depth--; return v; }
    void st32(uint32_t a, uint32_t v) { put32(a, v); }
    void st64(uint32_t a, uint64_t v) { put32(a, (uint32_t)(v >> 32)); put32(a + 4, (uint32_t)v); }
    uint64_t now() { return gthr::clock_now(); }
    [[noreturn]] void reject(const char* why) { model_reject(target, why); }
};
/* one modelled call: false when TARGET is not modelled (the caller answers it as before). Registers: r3 only, as the HLE */
static bool call(Cpu* c, uint32_t target, const uint32_t* a) {
    if (!parts || !models(target)) return false;
    if (parts & P_PORTMEM)
        if (uint32_t pm = portans::mem_slot(target)) {
            PortMemEnv e{target};
            uint32_t r = 0;
            int k = portans::mem_call(e, pm, a, c->f[1].ps0, &r);
            if (k) c->r[3] = r;
            if (g_trace) printf("        %sportmem %08X r3=%08X r4=%08X -> %08X%s\n", V.depth ? "nested " : "", pm, a[3], a[4], c->r[3], k ? "" : " (r3 kept)");
            return true;
        }
    if ((parts & P_THREADS) && gthr::call(c, target, a)) return true; /* `threads`: before the os/ghs parts' single-thread answers */
    if ((parts & P_AX) && gthr::ax_call(c, target, a)) return true;   /* `ax`: snd_core */
    if ((parts & P_PAD) && pad_call(c, target, a)) return true;       /* `pad`: vpad / padscore */
    if ((parts & (P_ACT | P_DYNLOAD)) && dyn_call(c, target, a)) return true; /* `act` / `dynload`: nn::act, OSDynLoad, erreula */
    if (gx2_setter(target) && dl_recording()) return false; /* inside a recording the generic/mockdisplaylist handling stays */
    uint32_t r = c->r[3];
    bool ret = true;
    if ((parts & P_PROCUI) && dlm::procui_slot(target)) r = dlm::kProcUIForeground;
    else if ((parts & P_PORTCONST) && portans::constant(target)) r = portans::constant(target)->r3;
    else if ((parts & P_FS) && (target >= 0xC0008C78u && target <= 0xC0008F98u)) r = fs_call(target, a, &ret);
    else switch (target) {
    case S_CREATE_EXP: r = create_exp(a[3], a[4], true); break;
    case S_DESTROY_EXP: st.exp.erase(a[3]); r = a[3]; break;
    case S_ALLOC_EXP: r = exp_alloc(a[3], a[4], (int32_t)a[5]); break;
    case S_FREE_EXP: { auto it = st.exp.find(a[3]); if (it != st.exp.end() && a[4]) it->second.release(a[4]); ret = false; break; }
    case S_ALLOCATABLE_EXP: {
        auto it = st.exp.find(a[3]);
        int32_t al = (int32_t)a[4];
        r = it == st.exp.end() ? 0 : it->second.largest((al < 0 ? 0u - (uint32_t)al : (uint32_t)al) | 4);
        break;
    }
    case S_TOTALFREE_EXP: { auto it = st.exp.find(a[3]); r = it == st.exp.end() ? 0 : it->second.total_free(); break; }
    case S_CREATE_FRM: r = create_frm(a[3], a[4], true); break;
    case S_DESTROY_FRM: st.frm.erase(a[3]); r = a[3]; break;
    case S_ALLOC_FRM: r = frm_alloc(a[3], a[4], (int32_t)a[5]); break;
    case S_ALLOCATABLE_FRM: {
        auto it = st.frm.find(a[3]);
        if (it == st.frm.end()) { r = 0; break; }
        int32_t al = (int32_t)a[4];
        uint32_t x = al < 0 ? 0u - (uint32_t)al : (uint32_t)al; /* std::abs */
        if (x < 4) x = 4;
        uint32_t p = (it->second.head + x - 1) & ~(x - 1);
        r = p < it->second.tail ? it->second.tail - p : 0;
        break;
    }
    case S_BASE_HANDLE: r = a[3] == 0 ? st.mem1 : a[3] == 1 ? st.def : a[3] == 8 ? st.fg : 0; break;
    case kHostAlloc: r = exp_alloc(st.def, a[3], 0x40); break;
    case kHostAllocEx: r = exp_alloc(st.def, a[3], (int32_t)a[4]); break;
    case kHostFree: { auto it = st.exp.find(st.def); if (a[3] && it != st.exp.end()) it->second.release(a[3]); ret = false; break; }
    case S_SURFACE: return false; /* the mocksurfacesize model (g_surface_size) answers it */
    case S_HIZ: case S_COLORAUX: put32(a[4], 0x1000); put32(a[5], 0x100); ret = false; break;
    case S_GX2INIT: case S_SETCTX: ret = false; break;
    case S_SETUPCTX: {
        uint32_t w0 = op_set_context | 1u << 8, w1 = a[3], dl = a[3] + 0x9800;
        V.depth++;
        for (int j = 0; j < 4; j++) { patch8(dl + j, (uint8_t)(w0 >> 8 * j)); patch8(dl + 4 + j, (uint8_t)(w1 >> 8 * j)); } /* host-endian */
        V.depth--;
        ret = false;
        break;
    }
    case S_GETCTXDL: if (a[4]) put32(a[4], a[3] + 0x9800); if (a[5]) put32(a[5], 8); ret = false; break;
    case S_SYSINFO: r = sysinfo; break;
    case S_UCOPEN: r = 1; break;
    case S_UCCLOSE: r = 0; break;
    case S_UCREAD: {
        uint32_t count = a[4], items = a[5];
        if (count > 4096) model_reject(target, "osmem uc: more than 4096 items");
        auto ld = [](uint32_t ea) { V.depth++; uint32_t w = (uint32_t)peek8(ea) << 24 | (uint32_t)peek8(ea + 1) << 16 | (uint32_t)peek8(ea + 2) << 8 | peek8(ea + 3); V.depth--; return w; };
        for (uint32_t i = 0; i < count; i++) {
            uint32_t e = items + i * 0x54;
            uint32_t value = uc_value(guest_cstr(target, e)), size = ld(e + 0x4C), data = ld(e + 0x50);
            if (data && size) {
                if (size > 0x100000) model_reject(target, "osmem uc: item data larger than 1 MB");
                V.depth++;
                for (uint32_t j = 0; j < size; j++) patch8(data + j, 0);
                if (size == 1) patch8(data, (uint8_t)value);
                else if (size == 2) { patch8(data, (uint8_t)(value >> 8)); patch8(data + 1, (uint8_t)value); }
                V.depth--;
                if (size >= 4) put32(data, value);
            }
            put32(e + 0x48, 0);
        }
        r = 0;
        break;
    }
    case S_TVSIZE: { /* (mode, format, bufferingMode, u32* size, bool* scaleNeeded) */
        uint32_t mode = a[3], buffers = std::max<uint32_t>(a[5], 1);
        uint32_t w = mode >= 5 ? 1920 : mode <= 2 ? 854 : 1280, hgt = mode >= 5 ? 1080 : mode <= 2 ? 480 : 720;
        put32(a[6], w * hgt * 4 * buffers); put32(a[7], 0); ret = false; break;
    }
    case S_DRCSIZE: put32(a[6], 854u * 480u * 4u * std::max<uint32_t>(a[5], 1)); put32(a[7], 0); ret = false; break;
    case S_SETEXCB: case S_FSADDCLIENT: case S_SAVEINIT: r = 0; break;
    case S_OLV_POSTDATA: case S_OLV_LISTPARAM: case S_OLV_TOPICDATA: r = 0; break; /* not implemented in the port: hle_unimplemented, r3 = 0 */
    case S_TRYLOCKMUTEX: r = 1; break; /* the one guest thread always gets (or re-enters) the lock */
    case S_INITMUTEX: put32(a[3], 0x6D557458u); ret = false; break; /* "mUtX" */
    case S_INITMSGQ: put32(a[3], 0x6D536751u); ret = false; break;  /* "mSgQ" */
    case S_FLOCK_PTR: r = bump_alloc(target, 4); break;
    case S_MTX_INIT: put32(a[3], bump_alloc(target, 8)); ret = false; break;
    default: ret = false; break; /* __gh_set_errno, __ghsLock/Unlock, flock/funlock/destroy, mtx dst/lock/unlock: host-only */
    }
    c->r[3] = ret ? r : a[3];
    if (g_trace) {
        if (ret) printf("        %sosmem %08X r3=%08X r4=%08X r5=%08X -> %08X\n", V.depth ? "nested " : "", target, a[3], a[4], a[5], r);
        else printf("        %sosmem %08X r3=%08X r4=%08X r5=%08X (void)\n", V.depth ? "nested " : "", target, a[3], a[4], a[5]);
    }
    return true;
}
}  // namespace osm

/* GX2 query models (2026-10-26, AGL#3/AGL#18 setups): the port HLE's deterministic semantics for imports whose
 * results the original stores (runtime/src/gx2/gx2_resources.cpp, gx2_core.cpp).
 * `mockretfield TARGET rN OFF SHIFT MASK`: r3 = (word at rN+OFF >> SHIFT) & MASK (GX2Get*ShaderGPRs/StackEntries).
 * `mockdisplaylist BEGIN END GETCUR [BYTES]`: BEGIN(r3 = start, r4 = size) opens a recording; END returns the bytes
 * written and closes it: min(BYTES, open size) while a recording is open AND at least one GX2 import call (slots
 * GX2_IMPORT_LO..HI other than BEGIN/END/GETCUR, top level or nested) was made since its BEGIN, else 0 (BYTES defaults
 * to 0 = mocked GX2 calls emit nothing; a nonzero BYTES models the shipped nonzero command size, 2026-10-29; the
 * command condition 2026-10-29 : an empty recording stays 0; known limit: commands a mocked NON-import
 * callee would issue are not seen, such a recording reads empty); GETCUR writes *r3 = start, *r4 = size (0 when none, each
 * only if the pointer is nonzero) and returns start != 0. Applied at top level and inside `real` callees. */
struct RetField { int reg; uint32_t off, shift, mask; };
static std::unordered_map<uint32_t, RetField> g_retfield;
static uint32_t g_dl_begin = 0, g_dl_end = 0, g_dl_getcur = 0;
static uint32_t g_dl_start = 0, g_dl_size = 0, g_dl_bytes = 0, g_dl_cmds = 0;
/* gx2 import slots (gen/imports.json: lib gx2 = C0006090..C0006AE8, contiguous, 108 functions) */
static constexpr uint32_t GX2_IMPORT_LO = 0xC0006090u, GX2_IMPORT_HI = 0xC0006AE8u;
static bool dl_recording() { return g_dl_start != 0; }
static inline void dl_count_call(uint32_t target) {
    if (g_dl_start && target >= GX2_IMPORT_LO && target <= GX2_IMPORT_HI && target != g_dl_begin && target != g_dl_end &&
        target != g_dl_getcur)
        g_dl_cmds++;
}
static bool gx2_model(Cpu* c, uint32_t target, const uint32_t* argr) {
    if (auto rf = g_retfield.find(target); rf != g_retfield.end()) {
        uint32_t a = argr[rf->second.reg] + rf->second.off;
        uint32_t w = (uint32_t)peek8(a) << 24 | (uint32_t)peek8(a + 1) << 16 | (uint32_t)peek8(a + 2) << 8 | peek8(a + 3);
        c->r[3] = (w >> rf->second.shift) & rf->second.mask;
        if (g_trace) printf("        mockretfield %08X -> %08X\n", target, c->r[3]);
        return true;
    }
    if (target && target == g_dl_begin) { g_dl_start = argr[3]; g_dl_size = argr[4]; g_dl_cmds = 0; return true; }
    if (target && target == g_dl_end) {
        c->r[3] = g_dl_start && g_dl_cmds ? (g_dl_bytes < g_dl_size ? g_dl_bytes : g_dl_size) : 0;
        if (g_trace) printf("        mockdisplaylist END %08X -> %X (GX2 calls %u)\n", g_dl_start, c->r[3], g_dl_cmds);
        g_dl_start = g_dl_size = g_dl_cmds = 0;
        return true;
    }
    if (target && target == g_dl_getcur) {
        V.depth++;
        if (argr[3]) for (int j = 0; j < 4; j++) patch8(argr[3] + j, (uint8_t)(g_dl_start >> (24 - 8 * j)));
        uint32_t sz = g_dl_start ? g_dl_size : 0;
        if (argr[4]) for (int j = 0; j < 4; j++) patch8(argr[4] + j, (uint8_t)(sz >> (24 - 8 * j)));
        V.depth--;
        c->r[3] = g_dl_start != 0;
        return true;
    }
    return false;
}
static void apply_fetch_effects(Cpu* c, uint32_t target, const uint32_t* argr) {
    if (g_fetch_size.count(target)) {
        uint32_t n = argr[3];
        uint32_t cf = ((((n + 15) / 16) + 1) * 8 + 15) & ~15u;
        c->r[3] = std::max(cf + n * 16, 16 + n * 16);
    }
    if (g_fetch_init.count(target)) {
        uint32_t n = argr[5];
        if (n > 16) model_reject(target, "fetch shader stream count > 16");
        uint64_t acc = hash3(n, 0xFE7C, 0);
        V.depth++;
        for (uint32_t j = 0; j < n * 32; j++) acc = hash3(acc, peek8(argr[6] + j), j);
        for (uint32_t j = 0; j < 32; j++) patch8(argr[3] + j, (uint8_t)(hash3(acc, j, 1) >> 17));
        for (uint32_t j = 0; j < 16 + n * 16; j++) patch8(argr[4] + j, (uint8_t)(hash3(acc, j, 2) >> 17));
        V.depth--;
    }
}
static std::string callee_name(uint32_t t);
/* mockfield effects of one generated call (top level; nested too for `nestedmockfield` targets) */
static std::set<uint32_t> g_nested_mockfield;
static void apply_mockfield(uint32_t target, const uint32_t* argr, uint64_t h) {
    auto mfs = g_mockfield.find(target);
    if (mfs != g_mockfield.end()) {
        V.depth++;
        int k = 0;
        for (const MockField& mf : mfs->second) {
            uint64_t hk = hash3(h, 0x6D66, k++);
            uint32_t base = mf.reg ? argr[mf.reg] : 0;
            if (mf.reg && base == 0) continue; /* a null object: nothing to fill */
            uint32_t dst = base + mf.off;
            if (mf.deref) { /* the value goes through the pointer stored at rN+OFF */
                uint32_t ptr = (uint32_t)peek8(dst) << 24 | (uint32_t)peek8(dst + 1) << 16 | (uint32_t)peek8(dst + 2) << 8 | peek8(dst + 3);
                if (ptr == 0) continue;
                dst = ptr + mf.off2;
            }
            double v = !mf.pick.empty() ? mf.pick[(hk >> 7) % mf.pick.size()]
                     : mf.is_float ? mf.lo + (double)((hk >> 11) % 1000001) / 1000000.0 * (mf.hi - mf.lo)
                                   : mf.lo + (double)((hk >> 11) % (uint64_t)(mf.hi - mf.lo + 1));
            uint32_t bits;
            if (mf.is_float) { float fv = (float)v; memcpy(&bits, &fv, 4); }
            else bits = (uint32_t)(int64_t)v;
            for (int b = 0; b < mf.size; b++) patch8(dst + b, (uint8_t)(bits >> (8 * (mf.size - 1 - b))));
            if (g_trace && V.depth > 1) printf("        nested mockfield %08X %08X <- %0*X\n", target, dst, mf.size * 2, bits);
        }
        V.depth--;
    }
}

/* Call budget (2026-10-24, zl1_heap setBtp / dungeon-map witnesses): a loop whose count comes from generated data
 * can make hundreds of thousands of mocked calls while staying far below the access budget. Every mocked call opens
 * a clobber epoch and every later byte load re-checks the epochs since it was last current, so such a run takes
 * hours and gigabytes. More calls than this abort the input (an abort never counts as a detected difference). */
static const uint64_t kCallBudget = 20000;
/* Depth budget (2026-10-31, AGL#8 027931EC: after a page-0 load without -nulltrap the original's real heap routine
 * 027540AC called itself with r3 = 0 about 6000 deep, below the call and page budgets, until the HOST stack overflowed:
 * SIGSEGV, a dead shard). Every `real` callee entered through a call (top level or nested) counts one level of
 * V.real_chain; entering one more than this many nested `real` callees aborts the input on the side that ran, like the
 * access, call, page and time budgets (an abort never counts as a detected difference). `depthbudget N` raises it. */
static size_t g_depth_budget = 1024;
[[gnu::cold, gnu::noinline]] static void depth_exceeded() {
    V.abort_reason = "depth budget exceeded (" + std::to_string(g_depth_budget) + " nested real calls)";
    longjmp(V.abort_jmp, 1);
}
namespace refc { static uint16_t call_mask(const CallEv& x, const CallEv* y); } /* gthr::compare: the same argument rule */
#include "gthr_sched.inc" /* `osmem ... threads`: the cooperative guest-thread model (namespace gthr) */
/* `callbudget N` (unit file): raise the call budget for a unit whose functions legitimately make more calls
 * (d_a_obj_apzl randamize_piece, d_a_bwdg wave_cont: shuffle/wave loops with many RNG/math calls). */
static uint64_t g_call_budget = kCallBudget;

/* Time budget (2026-10-24, hd_picto_pictograph_alias_order: the original side looped for 12 h inside 026FCB00 with
 * no guest call and no memory access, below both other budgets). Every run (setup, pokes, function) gets
 * kRunCpuSeconds of process CPU time (ITIMER_VIRTUAL, so machine load does not shorten it); past that the input
 * aborts as "time budget exceeded" on the side that ran (an abort never counts as a detected difference). */
static int kRunCpuSeconds = 30; /* `timebudget S` (unit file, 2026-10-26) raises it */
static volatile sig_atomic_t g_run_timed_out = 0;
/* process CPU time when the run's timer was armed. macOS delivers spurious ITIMER_VIRTUAL expiries once the process has a
 * second thread (2026-10-27: with the progress watchdog, about 1 run in 40 got SIGVTALRM right after arming, with 0 s of
 * CPU used: "time budget exceeded" on one-store functions, and SIGBUS after a longjmp out of harness internals). The
 * handler therefore checks the CPU time really used (CLOCK_PROCESS_CPUTIME_ID, user + system >= the timer's user time,
 * so a genuine expiry always passes) and re-arms the timer for the rest of the budget on an early one. */
static volatile double g_run_cpu0 = 0;
static double proc_cpu_seconds() {
    struct timespec t;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t);
    return (double)t.tv_sec + t.tv_nsec / 1e9;
}
static void run_timer(bool on) {
    struct itimerval t = {};
    t.it_value.tv_sec = on ? kRunCpuSeconds : 0;
    if (on) g_run_cpu0 = proc_cpu_seconds();
    setitimer(ITIMER_VIRTUAL, &t, nullptr);
}
static void run_timeout_handler(int) {
    if (!V.can_abort) return;
    double left = kRunCpuSeconds - (proc_cpu_seconds() - g_run_cpu0);
    if (left > 0.001) { /* an early (spurious) expiry: keep running, timer for the rest */
        struct itimerval t = {};
        t.it_value.tv_sec = (time_t)left;
        t.it_value.tv_usec = (suseconds_t)((left - (double)(time_t)left) * 1e6) + 1000;
        if (t.it_value.tv_usec >= 1000000) { t.it_value.tv_sec++; t.it_value.tv_usec -= 1000000; }
        setitimer(ITIMER_VIRTUAL, &t, nullptr);
        return;
    }
    g_run_timed_out = 1;
    longjmp(V.abort_jmp, 1);
}

/* ------------------------------------------------------------------ progress, stuck inputs, wall-clock hang limit
 *. A watchdog thread, started
 * after the start-up (and after -forkserver's fork) and with every signal blocked, reads the atomics below that the
 * main loop sets and
 *   - every WWHD_VERIFY_PROGRESS_INTERVAL seconds (default 10, -progress-interval S) prints one `[progress] ...` line to
 *     stderr: function k/N, inputs done/total, inputs/s, ETA, the last input's seed;
 *   - rewrites the progress file (JSON, tmp + rename) in WWHD_VERIFY_PROGRESS_DIR (verify.py: the unit's build or scratch
 *     directory + /progress; default /tmp/wwhd-verify-progress) at most every WWHD_VERIFY_PROGRESS_FILE_INTERVAL seconds
 *     (verify.py: 10 on a WWHD_VERIFY_SCRATCH scratch directory, 30 on the SSD) with an `updated` time stamp;
 *   - prints `[progress] STUCK ...` (seed, input index, side) when one input has run longer than WWHD_VERIFY_STUCK seconds
 *     (default 60, -stuck S, 0 = never), again at every further multiple;
 *   - per-run wall-clock hang limit WWHD_VERIFY_HANG_LIMIT / -hanglimit S (default 120, 0 = off, never below twice the CPU
 *     time budget unless set explicitly): a run (one side of one input) still going after that many wall seconds while
 *     using under 5% of a CPU and found not runnable in >= 95% of the watchdog's run-state samples (blocked or sleeping:
 *     a hang the CPU time budget cannot see; a CPU-starved run stays runnable) gets SIGUSR1 on the main
 *     thread, whose handler ends the run exactly like the CPU time budget (an abort, reason "wall-clock limit exceeded").
 *     A computing run (a guest loop, also a two-instruction one) is ended by the CPU time budget (ITIMER_VIRTUAL, 30 s
 *     CPU) as before, so no verdict depends on the machine's load.
 * Every input whose run ended by a time limit prints `[progress] HANG ...` naming function, seed and side (candidate
 * only: a FAIL row; original side: the inputs drive the original into a loop, a domain/fixture issue).
 * WWHD_VERIFY_PROGRESS=0 / -noprogress: none of this (no thread; the wall limit too), the old behaviour.
 * WWHD_VERIFY_PROGRESS_STDERR=0: no stderr lines at all (file only; callers that keep merged logs), =alerts: only the
 * STUCK/HANG lines (verify.py -shards prints the merged progress itself). stdout is never touched. */
#include <atomic>
#include <pthread.h>
#include <time.h>
#include <mach/mach.h>
#include <sys/resource.h>
namespace prog {
static mach_port_t main_mach = MACH_PORT_NULL; /* the main thread, for its run state (wall-clock limit) */
/* 1: the main thread is running or runnable (waiting for a CPU), 0: blocked/sleeping/stopped, -1: unknown */
static int main_runnable() {
    if (main_mach == MACH_PORT_NULL) return -1;
    thread_basic_info_data_t bi;
    mach_msg_type_number_t n = THREAD_BASIC_INFO_COUNT;
    if (thread_info(main_mach, THREAD_BASIC_INFO, (thread_info_t)&bi, &n) != KERN_SUCCESS) return -1;
    return bi.run_state == TH_STATE_RUNNING ? 1 : 0;
}
static bool on = true, file_on = true;
static int to_stderr = 2; /* 0 none, 1 alerts only, 2 everything */
static double interval = 10, file_interval = 30, stuck = 60, hang_limit = 120;
static std::string dir, path, tmp, registry, reglink;
static std::atomic<uint32_t> func{0};
static std::atomic<const char*> name{nullptr};
static std::atomic<int> fidx{0}, ftotal{0}, phase{0}, side{0};
static std::atomic<long> done{0}, total{0}, all_done{0}, all_total{0}, cur_idx{-1}, hangs{0};
static std::atomic<uint64_t> seed{0}, input_t0{0}, run_t0{0}, run_gen{0}, t_start{0};
static std::atomic<double> run_cpu0{0}; /* process CPU seconds at the run's start */
static std::atomic<bool> stop{false};
static volatile sig_atomic_t wall_gen_hit = 0; /* low bits of the run generation the watchdog interrupted */
static volatile sig_atomic_t wall_fired = 0;
static pthread_t main_thread, thread;
static bool started = false;
static bool hang_explicit = false; /* set by env or -hanglimit: taken as given (no CPU-budget floor) */
static pthread_mutex_t mu = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cv = PTHREAD_COND_INITIALIZER;
static const char* const phase_names[] = {"start", "setupsnapshot", "pre-runs", "generated", "recorded", "done"};

static uint64_t now_ns() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (uint64_t)t.tv_sec * 1000000000ull + (uint64_t)t.tv_nsec;
}
static double getenv_num(const char* k, double d) {
    const char* v = getenv(k);
    if (!v || !*v) return d;
    char* e;
    double x = strtod(v, &e);
    return *e || x < 0 ? d : x;
}
static void fmt_dur(char* b, size_t n, double s) {
    if (s < 0 || s > 1e9) { snprintf(b, n, "?"); return; }
    long t = (long)(s + 0.5);
    if (t >= 3600) snprintf(b, n, "%ldh%02ldm", t / 3600, t / 60 % 60);
    else if (t >= 60) snprintf(b, n, "%ldm%02lds", t / 60, t % 60);
    else snprintf(b, n, "%lds", t);
}
static std::string json_str(const char* s) {
    std::string o = "\"";
    for (; s && *s; s++) {
        unsigned char c = (unsigned char)*s;
        if (c == '"' || c == '\\') { o += '\\'; o += (char)c; }
        else if (c < 0x20) { char b[8]; snprintf(b, sizeof b, "\\u%04x", c); o += b; }
        else o += (char)c;
    }
    return o + "\"";
}
static void say(const char* line, bool alert) {
    if (to_stderr == 0 || (to_stderr == 1 && !alert)) return;
    flockfile(stderr); /* never inside another writer's fprintf */
    fputs(line, stderr);
    fflush(stderr);
    funlockfile(stderr);
}
static void write_file(const char* state, double in_elapsed, bool is_stuck) {
    if (!file_on) return;
    uint64_t t = now_ns(), t0 = t_start.load();
    double el = t0 ? (t - t0) / 1e9 : 0;
    long ad = all_done.load(), at = all_total.load();
    double rate = el > 0 ? ad / el : 0;
    char b[2048];
    snprintf(b, sizeof b,
             "{\"tool\": \"harness\", \"pid\": %d, \"state\": \"%s\", \"updated\": %ld, \"elapsed_s\": %.1f, \"func\": \"%08X\", "
             "\"name\": %s, \"func_index\": %d, \"func_total\": %d, \"phase\": \"%s\", \"done\": %ld, \"total\": %ld, "
             "\"all_done\": %ld, \"all_total\": %ld, \"rate\": %.2f, \"eta_s\": %.0f, \"seed\": %llu, \"input\": %ld, "
             "\"side\": \"%s\", \"input_elapsed_s\": %.1f, \"stuck\": %s, \"hangs\": %ld, \"stuck_after_s\": %.0f, "
             "\"hang_limit_s\": %.0f}\n",
             (int)getpid(), state, (long)time(nullptr), el, func.load(), json_str(name.load()).c_str(), fidx.load(), ftotal.load(),
             phase_names[phase.load()], done.load(), total.load(), ad, at, rate, rate > 0 ? (at > ad ? (at - ad) / rate : 0.0) : -1.0,
             (unsigned long long)seed.load(), cur_idx.load(), side.load() ? "candidate" : "original", in_elapsed,
             is_stuck ? "true" : "false", hangs.load(), stuck, hang_limit);
    FILE* f = fopen(tmp.c_str(), "w");
    if (!f) return;
    fputs(b, f);
    fclose(f);
    rename(tmp.c_str(), path.c_str());
}
static void* watchdog(void*) {
    uint64_t last_line = now_ns(), last_file = 0, stuck_input = 0;
    bool file_now = false;
    int stuck_n = 0;
    uint64_t fired_gen = 0;
    uint64_t sampled_gen = 0;
    long samples = 0, runnable = 0; /* run-state samples of the current run: a starved process stays runnable */
    pthread_mutex_lock(&mu);
    while (!stop.load()) {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += 1;
        pthread_cond_timedwait(&cv, &mu, &ts);
        if (stop.load()) break;
        uint64_t t = now_ns();
        /* wall-clock hang limit of the current run */
        uint64_t g1 = run_gen.load(), r0 = run_t0.load();
        double c0 = run_cpu0.load();
        uint64_t g2 = run_gen.load();
        double wall = r0 ? (t - r0) / 1e9 : 0;
        if (r0 && g1 == g2) {
            if (g1 != sampled_gen) { sampled_gen = g1; samples = runnable = 0; }
            int rs = main_runnable();
            samples++;
            runnable += rs != 0; /* unknown counts as runnable: never end a run on a guess */
        }
        /* only a run that is not computing is ended here: under 5% of one CPU since it started AND its thread found
         * blocked or sleeping (not runnable) in at least 95% of the once-a-second samples. A computing run (a guest loop)
         * is left to the CPU time budget, and a run starved of CPU under heavy load stays runnable (2026-10-27: at load
         * ~104 an AGL#12 input used < 5% CPU for 120 s and was ended as a false candidate-only timeout), so no verdict
         * depends on the machine's load */
        if (hang_limit > 0 && r0 && g1 == g2 && g1 != fired_gen && wall > hang_limit && proc_cpu_seconds() - c0 < 0.05 * wall &&
            samples >= 3 && runnable * 20 <= samples) {
            fired_gen = g1;
            wall_gen_hit = (sig_atomic_t)(g1 & 0x3FFFFFFF);
            pthread_kill(main_thread, SIGUSR1);
        }
        uint64_t i0 = input_t0.load();
        double in_el = i0 ? (t - i0) / 1e9 : 0;
        bool is_stuck = stuck > 0 && in_el > stuck;
        char line[1024], eta[32], ie[32];
        if (is_stuck) {
            int k = (int)(in_el / stuck);
            if (i0 != stuck_input) { stuck_input = i0; stuck_n = 0; }
            if (k > stuck_n) {
                stuck_n = k;
                fmt_dur(ie, sizeof ie, in_el);
                snprintf(line, sizeof line, "[progress] STUCK on input seed=%llu (%s input %ld) of %08X %s for %s (%s side, function %d/%d)\n",
                         (unsigned long long)seed.load(), phase_names[phase.load()], cur_idx.load(), func.load(),
                         name.load() ? name.load() : "", ie, side.load() ? "candidate" : "original", fidx.load(), ftotal.load());
                say(line, true);
                file_now = true;
            }
        }
        if ((t - last_line) / 1e9 >= interval) {
            last_line = t;
            double el = (t - t_start.load()) / 1e9;
            long ad = all_done.load(), at = all_total.load();
            double rate = el > 0 ? ad / el : 0;
            fmt_dur(eta, sizeof eta, rate > 0 && at >= ad ? (at - ad) / rate : -1);
            fmt_dur(ie, sizeof ie, in_el);
            long h = hangs.load();
            char hb[48] = "";
            if (h) snprintf(hb, sizeof hb, ", %ld hang(s)", h);
            snprintf(line, sizeof line, "[progress] harness pid %d: function %d/%d %08X %s, %s %ld/%ld, all %ld/%ld inputs, %.1f inputs/s, "
                     "ETA %s, last seed %llu (current input %s)%s\n",
                     (int)getpid(), fidx.load(), ftotal.load(), func.load(), name.load() ? name.load() : "", phase_names[phase.load()],
                     done.load(), total.load(), ad, at, rate, eta, (unsigned long long)seed.load(), ie, hb);
            say(line, false);
        }
        if (!last_file || (t - last_file) / 1e9 >= file_interval || file_now) {
            last_file = t;
            file_now = false;
            write_file("running", in_el, is_stuck);
        }
    }
    pthread_mutex_unlock(&mu);
    return nullptr;
}
static void wall_handler(int) {
    /* main thread only (pthread_kill): end the run whose generation the watchdog named, nothing else */
    if (!V.can_abort || !run_t0.load() || wall_gen_hit != (sig_atomic_t)(run_gen.load() & 0x3FFFFFFF)) return;
    wall_fired = 1;
    longjmp(V.abort_jmp, 1);
}
static void finish() {
    if (!started) return;
    started = false;
    pthread_mutex_lock(&mu);
    stop.store(true);
    pthread_cond_signal(&cv);
    pthread_mutex_unlock(&mu);
    pthread_join(thread, nullptr);
    if (file_on) {
        unlink(path.c_str());
        if (!reglink.empty()) unlink(reglink.c_str());
    }
}
static void configure() {
    const char* e = getenv("WWHD_VERIFY_PROGRESS");
    if (e && !strcmp(e, "0")) on = false;
    e = getenv("WWHD_VERIFY_PROGRESS_STDERR");
    if (e) to_stderr = !strcmp(e, "0") ? 0 : !strcmp(e, "alerts") ? 1 : 2;
    interval = getenv_num("WWHD_VERIFY_PROGRESS_INTERVAL", interval);
    stuck = getenv_num("WWHD_VERIFY_STUCK", stuck);
    hang_limit = getenv_num("WWHD_VERIFY_HANG_LIMIT", hang_limit);
    hang_explicit = getenv("WWHD_VERIFY_HANG_LIMIT") && *getenv("WWHD_VERIFY_HANG_LIMIT");
    const char* d = getenv("WWHD_VERIFY_PROGRESS_DIR");
    file_interval = getenv_num("WWHD_VERIFY_PROGRESS_FILE_INTERVAL", file_interval);
    registry = getenv("WWHD_PROGRESS_REGISTRY") ? getenv("WWHD_PROGRESS_REGISTRY") : "/tmp/wwhd-verify-progress";
    dir = d && *d ? d : registry;
}
/* after option parsing and the -forkserver fork: block every signal in the watchdog (the CPU timer, SIGSEGV/SIGBUS and
 * the nulltrap path stay on the main thread), start it */
static void start(long n_funcs) {
    if (!on) return;
    if (interval < 1) interval = 1;
    if (!hang_explicit && hang_limit > 0 && hang_limit < 2.0 * kRunCpuSeconds) hang_limit = 2.0 * kRunCpuSeconds;
    ftotal.store((int)n_funcs);
    t_start.store(now_ns());
    main_thread = pthread_self();
    main_mach = pthread_mach_thread_np(main_thread);
    signal(SIGUSR1, wall_handler);
    file_on = !dir.empty() && (mkdir(dir.c_str(), 0777) == 0 || errno == EEXIST);
    if (file_on) {
        path = dir + "/harness-" + std::to_string(getpid()) + ".json";
        tmp = path + ".tmp";
        if (dir != registry && (mkdir(registry.c_str(), 0777) == 0 || errno == EEXIST)) {
            reglink = registry + "/harness-" + std::to_string(getpid()) + ".json";
            unlink(reglink.c_str());
            if (symlink(path.c_str(), reglink.c_str())) reglink.clear();
        }
    }
    sigset_t all, old;
    sigfillset(&all);
    pthread_sigmask(SIG_BLOCK, &all, &old);
    started = pthread_create(&thread, nullptr, watchdog, nullptr) == 0;
    pthread_sigmask(SIG_SETMASK, &old, nullptr);
    if (started) atexit(finish);
}
static void run_begin(uint64_t s, bool cand) {
    seed.store(s, std::memory_order_relaxed);
    side.store(cand, std::memory_order_relaxed);
    run_t0.store(0);
    run_gen.fetch_add(1);
    run_cpu0.store(proc_cpu_seconds());
    run_t0.store(now_ns());
    wall_fired = 0;
}
static void run_end() { run_t0.store(0); }
static void input_begin(int ph, long idx) {
    phase.store(ph, std::memory_order_relaxed);
    cur_idx.store(idx, std::memory_order_relaxed);
    input_t0.store(now_ns(), std::memory_order_relaxed);
}
static void input_end(bool counted) {
    input_t0.store(0, std::memory_order_relaxed);
    if (counted) { done.fetch_add(1, std::memory_order_relaxed); all_done.fetch_add(1, std::memory_order_relaxed); }
}
}  // namespace prog

/* -nulltrap: is TARGET an address an indirect call or branch (bctrl/bctr/blrl, gabi::call_ptr/tail_ptr) can run on the
 * console (2026-10-29,; AGL#3 02779BFC: a stale method word led a nested call through slot +0x14 to 0,
 * which the harness answered with a generic mock)? Code addresses:
 *   - inside a loaded image section below 0x10000000 (the RPX code segment: .syscall 02000000 and .text from 02000020;
 *     mkimage.py dumps exactly the loaded sections, .rodata/.data and later lie at 0x10000000 and above), not page 0;
 *   - a function import slot (imports.json kind "f"; mkunit.py emits the sorted list as unit_import_slots);
 *   - any target the unit itself declares as callable: a callee entry (game code, imports, `callee ADDR fixture`
 *     mock-only targets) or a target named by a mock spec line (`ret`, `fret`, `mockset`, `mockzero`, ...).
 * PPC ignores the low two bits of CTR/LR targets: the word address counts. Direct calls (bl, gabi::call) have static
 * targets and are not checked. */
static bool code_target(uint32_t target) {
    uint32_t t = target & ~3u;
    if (t < 0x1000) return false;
    for (const Region& r : V.image)
        if (r.addr < 0x10000000u && t - r.addr < r.size) return true;
    if (std::binary_search(unit_import_slots, unit_import_slots + unit_nimport_slots, t)) return true;
    if (callee_info(target) || callee_info(t)) return true;
    if (osm::parts && osm::models(t)) return true; /* `osmem`: modelled imports and the default-heap host functions */
    for (uint32_t a : {target, t})
        if (g_ret_specs.count(a) || g_ret_picks.count(a) || g_fret_specs.count(a) || g_fret64_picks.count(a) || g_retarg.count(a) ||
            g_nestedret.count(a) || g_set_specs.count(a) || g_zero_specs.count(a) || g_lookup_specs.count(a) || g_tls_role.count(a) ||
            g_surface_size.count(a) || g_copy_surface.count(a) || g_move_specs.count(a) || g_fetch_size.count(a) || g_fetch_init.count(a) ||
            g_utf16_specs.count(a) || g_retain_specs.count(a) || g_mockfield.count(a) || g_alloc_specs.count(a) ||
            g_callcr_specs.count(a) || g_observe_specs.count(a) || g_observeptr_specs.count(a) || g_nested_specs.count(a) || g_retfield.count(a) ||
            g_stop_specs.count(a))
            return true;
    return false;
}
/* -nulltrap, candidate-only rule: the original's indirect calls to non-code targets of this
 * input (target values in call order, any depth; Result::badfetch, also in -refcache records) keep the generic mock; the
 * candidate may make the same calls (same target value, at most as often per target) with the same mock, and any other
 * candidate call to a non-code target is an instruction-fetch fault. Without provenance tracking a fill-derived target
 * (unsteered memory) cannot be told from program state, so the original never faults on a fetch; equivalent code makes the
 * same calls as the original and decides alike */
static std::vector<uint32_t> g_badfetch;                     /* this run's non-code indirect targets (original side) */
static std::unordered_map<uint32_t, int> g_orig_badfetch;    /* candidate side: the original's, still unmatched */
static bool g_orig_badfetch_on = false;                       /* set for a candidate run by the comparison loop */
[[gnu::cold, gnu::noinline]] static void fetch_check(uint32_t target) {
    if (!V.is_candidate) {
        g_badfetch.push_back(target);
        if (g_trace) printf("        fetch %08X (original side, depth %d): non-code target, generic mock\n", target, V.depth);
        return;
    }
    if (g_orig_badfetch_on) {
        auto it = g_orig_badfetch.find(target);
        if (it != g_orig_badfetch.end() && it->second > 0) {
            it->second--;
            if (g_trace) printf("        fetch %08X (candidate side, depth %d): the original made this call too, generic mock\n", target, V.depth);
            return;
        }
    }
    if (g_trace) printf("        fetch %08X (candidate side, depth %d): candidate-only non-code target, fault\n", target, V.depth);
    nulltrap_hit('X', target);
}
static void do_call(Cpu* c, uint32_t target, int kind, int nint, int nflt) {
    /* -nulltrap: an indirect call or branch to page 0 or a non-code address is an instruction-fetch fault (ISI) on the
     * console. A page-0 target (word address below 0x1000: a NULL function pointer or vtable slot) faults on EITHER side,
     * at any depth, exactly like a page-0 DSI (kind 'X', ea = target; 2026-11-02 : a page-0 word can only
     * come from program state or a zero fixture, never from random fill, so the original's call there is a fixture
     * defect like its page-0 load). Other non-code targets keep the candidate-only rule (fetch_check): a candidate call
     * the original did not make is the trap event. Without the option such targets get the generic mock as before;
     * -p0exclude notes the original's page-0 call like its page-0 access */
    if (__builtin_expect(g_nulltrap | g_p0exclude, 0) && kind == VM_CALL_INDIRECT) {
        if ((target & ~3u) < 0x1000) {
            if (g_nulltrap) nulltrap_hit('X', target);
            else p0_note();
        } else if (g_nulltrap && !code_target(target)) fetch_check(target);
    }
    if (++V.ncalls > g_call_budget && V.can_abort) {
        V.abort_reason = "call budget exceeded (" + std::to_string(g_call_budget) + " guest calls in one run)";
        longjmp(V.abort_jmp, 1);
    }
    dl_count_call(target); /* mockdisplaylist: GX2 commands issued into an open recording (top level and nested) */
    const CalleeInfo* ci = callee_info(target);
    /* a callee with a known pointee size (unit/global fact rN=SIZE or outSIZE): the candidate's gabi::Local passed
     * there must hold SIZE bytes, else the real callee reads/writes past it (game test: initTextureScroll 0x1C vs 0x40,
     * setSight 0x18 vs 0x28, dProcTool 12 vs 0x14) */
    if (V.is_candidate && V.depth == 0 && ci)
        for (int i = 3; i <= 10; i++) {
            uint32_t need = ci->ptrsz[i];
            bool from_ext = false;
            if (need == 255 || need == 0xFFFE) need = 0;
            /* a dynamic extent (rN=dynK*rM[:CAP], rN=cstr[:MAX]) needs only this call's extent, not the cap
             * (2026-11-01: hd_inf_render_1 r5=dyn4*r4:heap64 with a 4-word uniform in a 0x10 Local) */
            if (need && ci->dynsrc[i]) need = pointee_size(ci, i, c);
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
        /* `observenested`: record this call with the enclosing top-level call (V.calls is empty during the setups) */
        if (__builtin_expect(!g_nested_specs.empty(), 0) && !V.calls.empty())
            if (auto ns = g_nested_specs.find(target); ns != g_nested_specs.end()) {
                std::vector<uint8_t> bytes;
                for (const ObservePtr& o : ns->second) {
                    size_t at = bytes.size();
                    bytes.push_back(0);
                    if (span_bytes(c, o, bytes)) bytes[at] = 1;
                }
                if (g_trace) {
                    printf("        observenested %08X in call #%zu:", target, V.calls.size() - 1);
                    for (uint8_t b : bytes) printf(" %02X", b);
                    printf("\n");
                }
                V.calls.back().ex().nested.emplace_back(target, std::move(bytes));
            }
        uint32_t nargr[11];
        for (int i = 3; i <= 10; i++) nargr[i] = c->r[i];
        if (g_trace) printf("        nested call %08X r3=%08X r4=%08X r5=%08X r6=%08X%s\n", target, c->r[3], c->r[4], c->r[5], c->r[6],
                            ci && ci->real ? " (real)" : "");
        if (__builtin_expect(gthr::on, 0)) /* `threads`: a unit function reached from a guest thread runs this side's code */
            if (auto ufn = gthr::unit_fn(target)) {
                if (V.real_chain.size() >= g_depth_budget && V.can_abort) depth_exceeded();
                V.real_chain.push_back(target);
                ufn(c);
                V.real_chain.pop_back();
                if (g_trace) printf("        nested return %08X r3=%08X (unit function in guest thread T%d)\n", target, c->r[3], gthr::S.cur);
                return;
            }
        if (ci && ci->real) { /* a callee the unit lists as `real` (pure helper) also runs for real when nested */
            if (V.real_chain.size() >= g_depth_budget && V.can_abort) depth_exceeded();
            V.real_chain.push_back(target);
            ci->real(c);
            V.real_chain.pop_back();
            if (g_trace) printf("        nested return %08X r3=%08X\n", target, c->r[3]);
            return;
        }
        struct ThrLog { /* `threads`: the per-thread call log of a guest thread's mocked/modelled calls (r3 after the answer) */
            int i; Cpu* c;
            ~ThrLog() { gthr::log_end(i, c); }
        } thr_log{gthr::on ? gthr::log_begin(c, target, kind, nint, nflt) : -1, c};
        if (osm::parts && osm::call(c, target, nargr)) return; /* `osmem`: the port HLE's answer, also when nested (and in setups) */
        if (apply_tls(c, target, nargr)) return; /* declared mockthreadspecific also when nested (and in setups) */
        if (g_set_specs.count(target)) { apply_set(c, target, nargr); return; } /* declared mockset also when nested */
        if (g_zero_specs.count(target)) { apply_zero(target, nargr, c->lr); return; } /* declared mockzero also when nested */
        if (g_lookup_specs.count(target)) { apply_lookup(c, target, nargr); return; } /* declared mocklookup also when nested */
        if (g_surface_size.count(target)) { /* declared mocksurfacesize also when nested */
            uint8_t sb[0x74];
            surface_read(target, nargr[3], sb);
            surface_write(target, nargr[3], sb);
            return;
        }
        if (g_copy_surface.count(target)) { /* declared mockcopysurface also when nested */
            CopySurfacePlan plan;
            copy_surface_read(target, nargr, c->lr, plan);
            copy_surface_write(target, plan);
            return;
        }
        if (auto nr = g_nestedret.find(target); nr != g_nestedret.end()) { c->r[3] = nr->second; return; }
        if (gx2_model(c, target, nargr)) return;
        if (g_fetch_size.count(target) || g_fetch_init.count(target)) { /* declared fetch-shader imports also when nested */
            uint32_t argr[11];
            for (int i = 3; i <= 10; i++) argr[i] = c->r[i];
            apply_fetch_effects(c, target, argr);
            return;
        }
        if (auto mv = g_move_specs.find(target); mv != g_move_specs.end()) { /* declared mockmove also when nested */
            uint32_t size = c->r[mv->second.size], dst = c->r[mv->second.dst], src = c->r[mv->second.src];
            op_budget("memmove", target, dst, size, c->lr);
            std::vector<uint8_t> bytes(size);
            for (uint32_t j = 0; j < size; j++) { tick(); bytes[j] = peek8(src + j); } /* capture first: overlap-safe */
            for (uint32_t j = 0; j < size; j++) { tick(); patch8(dst + j, bytes[j]); }
            c->r[3] = dst;
            return;
        }
        uint64_t h = hash3(V.seed, target, 0xABCDEF + V.per_target[target]++);
        if (!ci || ci->defr >> 3 & 1) c->r[3] = shape_ret(ci, typed_ret(h));
        if (!ci || ci->deff >> 1 & 1) c->f[1].ps0 = c->f[1].ps1 = rand_float(h >> 3);
        if (g_trace && ci && target >= 0xC0000000u && !(ci->defr >> 3 & 1) && !(ci->deff >> 1 & 1)) { /* `importvoid` */
            uint64_t f1bits;
            memcpy(&f1bits, &c->f[1].ps0, 8);
            printf("        importvoid %08X: r3 %08X f1 %016llX\n", target, c->r[3], (unsigned long long)f1bits);
        }
        if (g_strictmocks && g_setup_what && target >= 0xC0000000u && (!ci || (ci->defr >> 3 & 1) || (ci->deff >> 1 & 1)) &&
            !g_ret_strict.count(target) && !g_retarg.count(target) && !g_alloc_nested.count(target) && !g_nested_mockfield.count(target) &&
            !g_fret_strict.count(target) && !g_fret64_strict.count(target)) {
            if (g_strictmocks_report) {
                auto& hit = (*g_strictmocks_hits)[std::make_tuple(std::string(g_setup_what), g_setup_target, target,
                                                                  V.real_chain.empty() ? 0u : V.real_chain.back(), (uint32_t)c->lr)];
                if (hit.first.empty()) hit.first = callee_name(target);
                if (!hit.second++) /* the first hit of a key at once (a run that dies later still lists it), the counts at exit */
                    fprintf(stderr, "strictmocks report: first: %s %08X calls import %s from %08X (lr %08X)\n", g_setup_what, g_setup_target,
                            callee_name(target).c_str(), V.real_chain.empty() ? 0u : V.real_chain.back(), c->lr);
            } else {
            fprintf(stderr, "strictmocks: %s %08X calls import %s%s from %08X (lr %08X): its answer would be generated; state it in the "
                    "spec (ret!/retarg/mockset/an osmem part)\n", g_setup_what, g_setup_target, callee_name(target).c_str(),
                    ci ? "" : " (no declaration: treated as value-returning)", V.real_chain.empty() ? 0u : V.real_chain.back(), c->lr);
            exit(2);
            }
        }
        if (g_ret_strict.count(target)) {
            if (auto rp = g_ret_picks.find(target); rp != g_ret_picks.end()) c->r[3] = (uint32_t)rp->second[(h >> 13) % rp->second.size()];
            else if (auto rs = g_ret_specs.find(target); rs != g_ret_specs.end())
                c->r[3] = (uint32_t)(rs->second.first + (int64_t)((h >> 13) % (uint64_t)(rs->second.second - rs->second.first + 1)));
        }
        if (auto fs = g_fret_specs.find(target); fs != g_fret_specs.end() && g_fret_strict.count(target)) {
            uint64_t k2 = (h >> 17) % 1000003;
            c->f[1].ps0 = c->f[1].ps1 = (double)(float)(fs->second.first + (double)k2 / 1000003.0 * (fs->second.second - fs->second.first));
        }
        if (auto f6 = g_fret64_picks.find(target); f6 != g_fret64_picks.end() && g_fret64_strict.count(target))
            c->f[1].ps0 = c->f[1].ps1 = f6->second[(h >> 19) % f6->second.size()];
        if (auto ra = g_retarg.find(target); ra != g_retarg.end()) {
            c->r[3] = nargr[ra->second];
            if (g_trace) printf("        retarg %08X -> %08X\n", target, c->r[3]);
        }
        if (g_alloc_nested.count(target)) alloc_steer(c, target, nargr);
        if (g_nested_mockfield.count(target)) apply_mockfield(target, nargr, h); /* `nestedmockfield`: also inside real callees */
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
            uint32_t n = ci && ci->ptrsz[i] ? (ci->dynsrc[i] ? pointee_size(ci, i, c) : ci->ptrsz[i]) : 4;
            if (n == 0xFFFE) n = 4; /* explicit `rN=0` fact: captured like an unknown pointer; compare() accepts equal raw values first (scalar) */
            if (n == 0 && ci && ci->dynsrc[i]) { ev.ex().sref[i].push_back(0); continue; } /* dynamic extent of 0 bytes: nothing to compare (the count register itself is compared) */
            if (n == 255 || (ci && (ci->outonly >> i & 1))) { ev.ex().sref[i].push_back(0); continue; } /* constructor storage / output-only: not an input */
            CallEv::Extra& E = ev.ex();
            for (uint32_t j = 0; j < n; j++) E.sref[i].push_back(peek8(c->r[i] + j));
            E.sflt[i].assign(n, 0);
            /* float-stored words overlapping the object: only starts in [r-3, r+n) can overlap (the stack range is far
             * from 0 and 0xFFFFFFFF, so no wrap); the same marks as scanning every entry (2026-10-25, AGL#18 speed) */
            uint32_t r0 = c->r[i];
            if (V.fany) {
                for (uint32_t w = r0 >= 3 ? r0 - 3 : 0; w < r0 + n; w++)
                    if (fmark_test(false, w))
                        for (uint32_t k = 0; k < 4; k++)
                            if (w + k - r0 < n) E.sflt[i][w + k - r0] = 1;
                for (uint32_t w = r0; w < r0 + n; w++)
                    if (fmark_test(true, w) && n - (w - r0) >= 8) E.sflt[i][w - r0] = 2;
            }
        }
    if (ci && ci->heapref) /* rN=heapSIZE: the callee reads the object through a heap pointer (JS-JParticle#2: &dc) */
        for (int i = 3; i <= 10; i++)
            if ((ci->heapref >> i & 1) && !in_stack(c->r[i]) && c->r[i] && ci->ptrsz[i] && ci->ptrsz[i] < 0xFFFE) {
                CallEv::Extra& E = ev.ex();
                for (uint32_t j = 0, n = pointee_size(ci, i, c); j < n; j++) E.href[i].push_back(peek8(c->r[i] + j));
            }
    if (g_observe_pointees) { /* -observe-pointees: undeclared heap/static pointer arguments, their whole object */
        uint32_t mask = ci && !ci->declared ? ci->imask : 0x7F8u;
        for (int i = 3; i <= 10; i++) {
            uint32_t pv = c->r[i], base = 0, size = 0;
            if (!(mask >> i & 1) || !pv || in_stack(pv)) continue;
            if (ci && (ci->ptrsz[i] || (ci->heapref >> i & 1) || ci->dynsrc[i])) continue; /* a declared pointee wins */
            if (observeptr_declares(target, i)) continue;
            int kind = pointee_block(pv, &base, &size);
            if (!kind || !size) continue;
            uint32_t from = base, n = size;
            if (size > g_obs_cap) { /* a cap-sized window around the pointer, inside the object */
                uint64_t off = pv - base, st = off > g_obs_cap / 2 ? off - g_obs_cap / 2 : 0;
                if (st + g_obs_cap > size) st = size - g_obs_cap;
                from = base + (uint32_t)(st & ~3ull);
                n = g_obs_cap;
            }
            CallEv::Extra& E = ev.ex();
            E.pbase[i] = base; E.psize[i] = size; E.pfrom[i] = from; E.pkind[i] = (uint8_t)kind;
            E.pref[i].resize(n);
            for (uint32_t j = 0; j < n; j++) E.pref[i][j] = peek8(from + j);
            if (!V.is_candidate) { g_obs_n[kind]++; g_obs_trunc += n < size; g_obs_bytes += n; }
        }
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
    if (auto op = g_observeptr_specs.find(target); op != g_observeptr_specs.end()) {
        uint32_t ord = V.obs_ord[target]++; /* this call's 0-based position among the run's top-level calls to target */
        if (g_trace)
            for (const ObservePtr& o : op->second)
                if (o.direct && (o.site >= 0 || o.creg)) { /* the values the per-site forms select on */
                    printf("        observearg site %08X @%u lr=%08X r3=%08X r4=%08X r5=%08X r6=%08X\n", target, ord, c->lr, c->r[3],
                           c->r[4], c->r[5], c->r[6]);
                    break;
                }
        for (size_t oi = 0; oi < op->second.size(); oi++) {
            const ObservePtr& o = op->second[oi];
            uint32_t at = c->r[o.reg] + o.off;
            std::vector<uint8_t> bytes;
            if (o.deref || (o.direct && (o.site >= 0 || o.creg))) { /* per-site / conditional / dereference: kept apart, decided in compare */
                if (o.site >= 0 && ord != (uint32_t)o.site) continue;
                bool have = span_bytes(c, o, bytes);
                if (g_trace && o.deref) {
                    printf("        observearg %08X %s @%u:", target, span_name(o).c_str(), ord);
                    if (!have) printf(" (null)");
                    for (uint8_t b : bytes) printf(" %02X", b);
                    printf("\n");
                }
                ev.ex().sobserved.emplace_back((uint32_t)oi, std::move(bytes));
                continue;
            }
            if (o.direct) {
                if (c->r[o.reg])
                    for (uint32_t j = 0; j < o.size; j++) bytes.push_back(peek8(at + j));
                ev.ex().observed.emplace_back(0xFE000000u | o.reg << 16 | (o.off & 0xFFFF), std::move(bytes));
                continue;
            }
            uint32_t ptr = c->r[o.reg] ? ((uint32_t)peek8(at) << 24 | (uint32_t)peek8(at + 1) << 16 | (uint32_t)peek8(at + 2) << 8 | peek8(at + 3)) : 0;
            if (ptr)
                for (uint32_t j = 0; j < o.size; j++) bytes.push_back(peek8(ptr + j));
            ev.ex().observed.emplace_back(0xFFFF0000u | o.reg << 8 | (o.off & 0xFF), std::move(bytes));
        }
    }
    if (auto cc = g_callcr_specs.find(target); cc != g_callcr_specs.end()) {
        CallEv::Extra& E = ev.ex();
        E.crmask = cc->second;
        for (int b = 0; b < 32; b++)
            if (cc->second >> b & 1 && c->cr[b]) E.crbits |= 1u << b;
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
    ev.lr = c->lr;
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
    if (V.depth == 0 && V.can_abort && !g_stop_specs.empty()) { /* `stopat`: the terminal call ends the run */
        auto st = g_stop_specs.find(g_cur_func);
        if (st != g_stop_specs.end() && st->second.target == target && ++V.stop_count == st->second.n) longjmp(V.abort_jmp, 2);
    }

    /* the candidate's k-th top-level call to the same target as the original's k-th call passes the original's
     * return address: a real callee that saves LR into the caller's frame then
     * stores the same word on both sides (otherwise event bytes shifted into the LR save word differ). After the call record, so `framexact` still compares the candidate's own LR. */
    if (V.is_candidate && V.depth == 0 && k < V.orig_seq.size() && V.orig_seq[k].first == target && k < V.orig_lr.size())
        c->lr = V.orig_lr[k];

    if (ci && ci->real) {
        if (V.real_chain.size() >= g_depth_budget && V.can_abort) depth_exceeded();
        V.depth++;
        V.real_chain.push_back(target);
        ci->real(c);
        V.real_chain.pop_back();
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
        op_budget("memmove", target, move_dst, size, c->lr);
        move_bytes.reserve(size);
        V.depth++;
        for (uint32_t j = 0; j < size; j++) {
            tick();
            move_bytes.push_back(peek8(c->r[move->second.src] + j));
        }
        V.depth--;
    }
    uint8_t surface_bytes[0x74];
    uint32_t surface_at = c->r[3];
    bool surface = surface_read(target, surface_at, surface_bytes); /* inputs as before the call's epoch, like mockmove */
    CopySurfacePlan copy_plan;
    bool copy_surface;
    {
        uint32_t cargr[11];
        for (int i = 3; i <= 10; i++) cargr[i] = c->r[i];
        copy_surface = copy_surface_read(target, cargr, c->lr, copy_plan); /* source bytes as before the epoch */
    }
    /* a listed constructor (ctor_sizes.tsv) whose storage mkunit marked output-only without a random fill
     *: the storage of a mocked constructor is not an input; its output is modelled as
     * zeros, written BEFORE the mocked call on both sides (nothing is written after it). Zeros, not a non-zero
     * byte: the original packs and reuses frame slots while candidate Locals have their own layout, so a non-zero
     * fill would leave layout-dependent stale bytes (d_snap draw, d_a_ep). `stackfill` still covers the bytes
     * outside sizeof; explicit `rN=outSIZE` facts keep their random fill; `real` constructors are never filled. */
    if (ci && V.depth == 0 && (ci->outonly >> 3 & 1) && !(ci->fill >> 3 & 1) && ci->ptrsz[3] && ci->ptrsz[3] < 0xFFFE &&
        in_stack(c->r[3]) && g_ctor_size.count(target)) {
        V.depth++;
        for (uint32_t j = 0; j < ci->ptrsz[3]; j++) patch8(c->r[3] + j, 0);
        V.depth--;
    }
    /* generated mock: answer in the registers the real callee may write; memory it may have
     * changed is modelled by the new epoch */
    V.epoch++;
    uint64_t h = hash3(V.seed, target, V.per_target[target]++);
    uint32_t argr[11];
    for (int i = 3; i <= 10; i++) argr[i] = c->r[i]; /* output pointers as passed (the mock's results overwrite r3/r4) */
    if (osm::parts && osm::call(c, target, argr)) { /* `osmem`: the port HLE's registers and effects only, after the epoch */
        if (V.depth == 0) { for (int i = 4; i <= 12; i++) V.post_r[i] = c->r[i]; V.have_post = true; }
        return;
    }
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
    if (auto f6 = g_fret64_picks.find(target); f6 != g_fret64_picks.end() && ((h >> 45) % 8 != 0 || g_fret64_strict.count(target)))
        c->f[1].ps0 = c->f[1].ps1 = f6->second[(h >> 19) % f6->second.size()];
    apply_fetch_effects(c, target, argr);
    if (V.depth == 0) { for (int i = 4; i <= 12; i++) V.post_r[i] = c->r[i]; V.have_post = true; }
    alloc_steer(c, target, argr);
    gx2_model(c, target, argr);
    if (auto ra = g_retarg.find(target); ra != g_retarg.end()) {
        c->r[3] = argr[ra->second];
        if (g_trace) printf("        retarg %08X -> %08X\n", target, c->r[3]);
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
    apply_set(c, target, argr);
    apply_zero(target, argr, c->lr);
    apply_lookup(c, target, argr);
    apply_tls(c, target, argr); /* after the epoch; GET/CUR replace the generated r3 */
    if (surface) surface_write(target, surface_at, surface_bytes);
    if (copy_surface) copy_surface_write(target, copy_plan);
    if (auto rt = g_retain_specs.find(target); rt != g_retain_specs.end()) {
        if (auto kept = g_retained.find(target); kept != g_retained.end()) {
            V.depth++;
            for (uint32_t j = 0; j < rt->second.size; j++) patch8(kept->second + j, (uint8_t)(hash3(h, j, 0x4E7A) >> 17));
            V.depth--;
        }
        g_retained[target] = argr[rt->second.reg];
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
    apply_mockfield(target, argr, h);
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
    bool exact; /* argf64: fN values are placed as IEEE-754 doubles, without narrowing to single */
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
/* fill FUNC|* ADDR|rN+OFF SIZE BYTE: SIZE bytes from ADDR all set to BYTE in every generated input (both sides),
 * before the `field` steering (fields of the same bytes override it); a background for large regions that per-word
 * `field` lines would need thousands of lines for (e.g. a 512 KB all-0xFF table that reaches a scan cap) */
struct FillSpec {
    uint32_t func;
    int base_reg;
    uint32_t off;
    uint32_t size;
    uint8_t byte;
};
static std::vector<FillSpec> g_fill_specs;
/* `cr FUNC|* BIT V[,V...]`: entry CR bit BIT (0..31, bit 6 = cr1.eq) of FUNC's generated inputs is one of
 * the values (0/1), picked per input. Generated inputs otherwise start with every CR bit clear. Use: a
 * variadic callee saves f1-f8 only when the caller set CR bit 6 (PPC EABI: floats passed in FPRs). */
struct CrSpec {
    uint32_t func;
    int bit;
    std::vector<uint8_t> pick;
};
static std::vector<CrSpec> g_cr_specs;
/* `copy FUNC|* DST|rN+OFF SRC SIZE`: SIZE bytes of the loaded game image at SRC are placed at DST in every generated input of
 * FUNC (both sides), after `fill` and before `field` (field lines over the same bytes win). SRC..SRC+SIZE must lie in one image
 * region (checked after the image is loaded). `blob FUNC|* DST|rN+OFF FILE OFFSET SIZE`: the same with SIZE bytes read at run
 * time from `game/FILE` (relative to the game link; never committed) at OFFSET. */
struct CopySpec {
    uint32_t func;
    int base_reg;
    uint32_t off, src, size;
    std::vector<uint8_t> data; /* blob: the file bytes; copy: filled from the image after load */
    bool blob;
};
static std::vector<CopySpec> g_copy_specs;
static bool parse_dst(const char* w, CopySpec* cs) {
    if (w[0] == 'r') {
        char* plus;
        cs->base_reg = (int)strtol(w + 1, &plus, 10);
        cs->off = *plus == '+' ? (uint32_t)strtoul(plus + 1, nullptr, 0) : 0;
        return cs->base_reg >= 0 && cs->base_reg <= 31;
    }
    char* e;
    cs->base_reg = -1;
    cs->off = (uint32_t)strtoul(w, &e, 16);
    return *e == 0;
}
static std::vector<std::string> g_stage_names; /* tools/verify/stage_names.txt, loaded on first @stages */
static std::string g_spec_dir = ".";

/* tools/verify of a spec path: the part before the last "/units/" (nested companion specs live in
 * units/<unit>/<profile>.txt, so "<spec dir>/.." is not enough); fall back to "<spec dir>/.." */
/* tools/verify of this harness source (2026-11-01): an expanded spec (a unit with `include`, written by specfile.py
 * into the build or verify-scratch directory) has no /units/ in its path, so its stage_names.txt was looked up beside
 * the build directory and `fieldstr ... @stages` exited 2. __FILE__ is the path the harness was compiled from
 * (verify.py: tools/verify/src/harness.cpp, relative to the clone root it runs in). */
static std::string harness_verify_dir() {
    std::string f = __FILE__;
    size_t k = f.rfind("/src/");
    return k == std::string::npos ? std::string("tools/verify") : f.substr(0, k);
}
/* the directory beside a spec path (the old lookup; kept for reference by stage_names_dir) */
static std::string verify_dir(const char* spec_path) {
    std::string p = spec_path;
    size_t u = p.rfind("/units/");
    if (u != std::string::npos) return p.substr(0, u);
    if (p.compare(0, 6, "units/") == 0) return ".";
    size_t k = p.rfind('/');
    return (k == std::string::npos ? std::string(".") : p.substr(0, k)) + "/..";
}
/* stage_names.txt and ctor_sizes.tsv: beside the unit spec, or tools/verify for an expanded spec */
static std::string stage_names_dir(const char* spec_path) {
    std::string p = spec_path;
    if (p.rfind("/units/") != std::string::npos || p.compare(0, 6, "units/") == 0) return verify_dir(spec_path);
    return harness_verify_dir();
}
static std::string g_verify_dir = ".";

static void load_ctor_sizes(const char* spec_path) {
    /* units/X.txt and units/X/Y.txt -> tools/verify/ctor_sizes.tsv; an expanded spec (a unit with `include`) -> the
     * harness's tools/verify too (2026-11-01, : before, expanded specs found no
     * ctor_sizes.tsv beside their build directory and ran without the constructor facts) */
    std::string p = stage_names_dir(spec_path) + "/ctor_sizes.tsv";
    FILE* f = fopen(p.c_str(), "r");
    if (!f) { /* always said (2026-11-02): a silent miss left 668 expanded specs without the constructor facts */
        fprintf(stderr, "ctor_sizes: %s missing\n", p.c_str());
        return;
    }
    if (getenv("WWHD_REPORT_CTOR_SIZES")) fprintf(stderr, "ctor_sizes: %s loaded\n", p.c_str());
    char line[256];
    while (fgets(line, sizeof line, f)) {
        unsigned a, n;
        if (line[0] != '#' && sscanf(line, "%x %x", &a, &n) == 2 && n) g_ctor_size[a] = n;
    }
    fclose(f);
    if (getenv("WWHD_REPORT_CTOR_SIZES")) fprintf(stderr, "ctor_sizes: %zu constructors\n", g_ctor_size.size());
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

/* Spec value tokens must parse completely (2026-10-27): strtoll/strtod/strtoul without an end check turned a token such
 * as `*0x5E100008` (a dereference other directives accept) into 0, so `arg`/`field`/`ret` lines silently steered 0.
 * A token that is not a whole number exits 2 naming the spec file, line, directive and token. */
static const char* g_spec_path = "";
static int g_spec_lineno = 0;
static const char* g_spec_line = "";
[[noreturn]] static void spec_bad_token(const char* kw, const char* tok, const char* what) {
    fprintf(stderr, "%s:%d: invalid %s %s \"%s\" (not a complete number): %s", g_spec_path, g_spec_lineno, kw, what, tok,
            g_spec_line);
    if (!*g_spec_line || g_spec_line[strlen(g_spec_line) - 1] != '\n') fputc('\n', stderr);
    exit(2);
}
static long long spec_int(const char* t, const char* kw) { /* decimal, 0x hex or 0 octal, as strtoll(t, .., 0) */
    char* e;
    errno = 0;
    long long v = strtoll(t, &e, 0);
    if (e == t || *e) spec_bad_token(kw, t, "value");
    return v;
}
static double spec_num(const char* t, bool is_float, const char* kw) {
    if (!is_float) return (double)spec_int(t, kw);
    char* e;
    double v = strtod(t, &e);
    if (e == t || *e) spec_bad_token(kw, t, "value");
    return v;
}
static unsigned long spec_uint(const char* t, int base, const char* kw, const char* what) {
    char* e;
    unsigned long v = strtoul(t, &e, base);
    if (e == t || *e) spec_bad_token(kw, t, what);
    return v;
}
static uint32_t spec_func(const char* t, const char* kw) { /* FUNC: `*` (0) or a hex address */
    return strcmp(t, "*") ? (uint32_t)spec_uint(t, 16, kw, "function address") : 0;
}

/* an observe span token: `rN`, `rN+OFF` (the argument's own bytes) or `[rN+OFF]+OFF2` / `[rN]+OFF2` / `[rN+OFF]` (through
 * the word stored at rN+OFF); r3..r10, offsets <= 0xFFFF. Returns an error text or nullptr. */
static const char* parse_span(const char* tok, ObservePtr& o) {
    bool br = tok[0] == '[';
    const char* q = tok + (br ? 1 : 0);
    if (q[0] != 'r' || !isdigit((unsigned char)q[1])) return br ? "dereference form [rN+OFF]+OFF2" : "argument register r3..r10";
    char* e = nullptr;
    unsigned long reg = strtoul(q + 1, &e, 10);
    unsigned long off = 0, off2 = 0;
    if (*e == '+') {
        const char* ot = e + 1;
        off = strtoul(ot, &e, 0);
        if (e == ot) return "offset";
    }
    if (br) {
        if (*e != ']') return "dereference form [rN+OFF]+OFF2";
        e++;
        if (*e == '+') {
            const char* ot = e + 1;
            off2 = strtoul(ot, &e, 0);
            if (e == ot) return "pointee offset";
        }
    }
    if (*e) return br ? "dereference form [rN+OFF]+OFF2" : "argument register r3..r10, optional +OFF";
    if (reg < 3 || reg > 10 || off > 0xFFFF || off2 > 0xFFFF) return "r3..r10, offsets <= 0xFFFF";
    o.reg = (unsigned)reg; o.off = (uint32_t)off; o.off2 = (uint32_t)off2; o.deref = br;
    return nullptr;
}

static void load_specs(const char* path) {
    g_spec_path = path;
    g_spec_lineno = 0;
    {
        std::string p = path;
        size_t k = p.rfind('/');
        g_spec_dir = k == std::string::npos ? std::string(".") : p.substr(0, k);
        g_verify_dir = stage_names_dir(path);
    }
    load_ctor_sizes(path);
    load_livein();
    FILE* f = fopen(path, "r");
    if (!f) return;
    static char line[16384];
    static char line_copy[16384];
    while (fgets(line, sizeof line, f)) {
        g_spec_lineno++;
        memcpy(line_copy, line, sizeof line_copy);
        g_spec_line = line_copy; /* strtok below cuts `line` */
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
        if (!strcmp(kw, "mockset")) { // mockset TARGET rDST rVAL rSIZE
            unsigned target, dst, val, size;
            if (sscanf(line, "%*s %x r%u r%u r%u", &target, &dst, &val, &size) != 4 ||
                dst < 3 || dst > 10 || val < 3 || val > 10 || size < 3 || size > 10) {
                fprintf(stderr, "invalid mockset directive: %s", line); exit(2);
            }
            g_set_specs[target] = {dst, val, size};
            continue;
        }
        if (!strcmp(kw, "callbudget")) { // callbudget N: this unit's guest-call budget per run
            unsigned long long nb; char extra[8];
            if (sscanf(line, "%*s %llu %7s", &nb, extra) != 1 || nb < 1000 || nb > 100000000ull) {
                fprintf(stderr, "invalid callbudget directive (1000..100000000): %s", line); exit(2);
            }
            g_call_budget = nb;
            continue;
        }
        if (!strcmp(kw, "mockretfield")) { // mockretfield TARGET rN OFF SHIFT MASK
            unsigned target, reg, off, shift, mask; char extra[8];
            if (sscanf(line, "%*s %x r%u %i %u %i %7s", &target, &reg, (int*)&off, &shift, (int*)&mask, extra) != 5 || reg < 3 || reg > 10 || shift > 31) {
                fprintf(stderr, "invalid mockretfield directive (TARGET rN OFF SHIFT MASK): %s", line); exit(2);
            }
            g_retfield[target] = RetField{(int)reg, off, shift, mask};
            continue;
        }
        if (!strcmp(kw, "mockdisplaylist")) { // mockdisplaylist BEGIN END GETCUR [BYTES] (last one wins)
            unsigned b, e, gc, by = 0; char extra[8];
            int k = sscanf(line, "%*s %x %x %x %x %7s", &b, &e, &gc, &by, extra);
            if ((k != 3 && k != 4) || !b || !e || !gc) {
                fprintf(stderr, "invalid mockdisplaylist directive (BEGIN END GETCUR [BYTES]): %s", line); exit(2);
            }
            g_dl_begin = b; g_dl_end = e; g_dl_getcur = gc; g_dl_bytes = k == 4 ? by : 0;
            continue;
        }
        if (!strcmp(kw, "strictmocks")) { /* strictmocks: generated import answers inside setups end the run (g_strictmocks) */
            char extra[8];
            char more[8];
            int k = sscanf(line, "%*s %7s %7s", extra, more);
            if (k == 2 || (k == 1 && strcmp(extra, "report"))) {
                fprintf(stderr, "invalid strictmocks directive (strictmocks [report]): %s", line); exit(2);
            }
            g_strictmocks = true;
            if (k == 1 && !g_strictmocks_report) {
                g_strictmocks_report = true;
                g_strictmocks_hits = new std::map<std::tuple<std::string, uint32_t, uint32_t, uint32_t, uint32_t>, std::pair<std::string, uint64_t>>();
                atexit(strictmocks_print);
            }
            continue;
        }
        if (!strcmp(kw, "nulltrap")) { // nulltrap: page-0 guest accesses are DSI trap events (the -nulltrap option)
            char extra[8];
            if (sscanf(line, "%*s %7s", extra) == 1) {
                fprintf(stderr, "invalid nulltrap directive (takes no operands): %s", line); exit(2);
            }
            g_nulltrap = true;
            g_nulltrap_unit = true;
            continue;
        }
        if (!strcmp(kw, "depthbudget")) { // depthbudget N: this unit's nested `real` call depth per run
            unsigned long long nb; char extra[8];
            if (sscanf(line, "%*s %llu %7s", &nb, extra) != 1 || nb < 64ull || nb > 2048ull) {
                fprintf(stderr, "invalid depthbudget directive (64..2048): %s", line); exit(2);
            }
            g_depth_budget = nb;
            continue;
        }
        if (!strcmp(kw, "pagebudget")) { // pagebudget N: this unit's new-guest-page budget per run
            unsigned long long nb; char extra[8];
            if (sscanf(line, "%*s %llu %7s", &nb, extra) != 1 || nb < 4096ull || nb > 1048576ull) {
                fprintf(stderr, "invalid pagebudget directive (4096..1048576): %s", line); exit(2);
            }
            g_page_budget = nb;
            continue;
        }
        if (!strcmp(kw, "opbudget")) { // opbudget BYTES: the per-operation byte cap of mockset/mockzero/memmove (once per spec)
            unsigned long long nb = 0; char num[32], extra[8], *e = nullptr;
            bool ok = sscanf(line, "%*s %31s %7s", num, extra) == 1;
            if (ok) {
                bool hex = (num[0] == '0' && (num[1] == 'x' || num[1] == 'X'));
                const char* d = hex ? num + 2 : num;
                ok = *d && *d != '-' && *d != '+';
                if (ok) { errno = 0; nb = strtoull(d, &e, hex ? 16 : 10); ok = !errno && !*e; }
            }
            if (!ok || nb < 1 || nb > 1073741824ull || g_op_budget_set) {
                fprintf(stderr, "invalid opbudget directive (opbudget BYTES, decimal or 0x-hex, 1..1073741824; once per spec): %s", line);
                exit(2);
            }
            g_op_budget = (uint32_t)nb;
            g_op_budget_set = true;
            continue;
        }
        if (!strcmp(kw, "hostmembudget")) { // hostmembudget MB: this unit's host RSS budget (WWHD_VERIFY_HOSTMEM_MB overrides it)
            unsigned long long nb; char extra[8];
            if (sscanf(line, "%*s %llu %7s", &nb, extra) != 1 || nb < 64ull || nb > 65536ull) {
                fprintf(stderr, "invalid hostmembudget directive (64..65536): %s", line); exit(2);
            }
            if (!g_hostmem_env) g_hostmem_budget_mb = nb;
            continue;
        }
        if (!strcmp(kw, "accessbudget")) { // accessbudget N: this unit's memory-access budget per run
            unsigned long long nb; char extra[8];
            if (sscanf(line, "%*s %llu %7s", &nb, extra) != 1 || nb < 1000000ull || nb > 1000000000ull) {
                fprintf(stderr, "invalid accessbudget directive (1000000..1000000000): %s", line); exit(2);
            }
            g_access_budget = nb;
            continue;
        }
        if (!strcmp(kw, "setupsnapshot")) { // setupsnapshot: the `setup` lines run once per function, inputs start from their state
            char extra[8];
            if (sscanf(line, "%*s %7s", extra) == 1) {
                fprintf(stderr, "invalid setupsnapshot directive (takes no operands): %s", line); exit(2);
            }
            g_setup_snapshot = true;
            continue;
        }
        if (!strcmp(kw, "timebudget")) { // timebudget S: this unit's CPU seconds per run
            int sec; char extra[8];
            if (sscanf(line, "%*s %d %7s", &sec, extra) != 1 || sec < 30 || sec > 600) {
                fprintf(stderr, "invalid timebudget directive (30..600): %s", line); exit(2);
            }
            kRunCpuSeconds = sec;
            continue;
        }
        if (!strcmp(kw, "allocfail")) { // allocfail TARGET K[,K...]: after its alloc/allocfixed line
            unsigned target; char list[512] = "", extra[8];
            bool ok = sscanf(line, "%*s %x %511s %7s", &target, list, extra) == 2 && g_alloc_specs.count(target);
            std::vector<uint32_t> ks;
            for (char* t = strtok(list, ","); ok && t; t = strtok(nullptr, ",")) {
                char* e; unsigned long k = strtoul(t, &e, 0);
                ok = *e == 0 && k <= 100000; ks.push_back((uint32_t)k);
            }
            if (!ok || ks.empty()) { fprintf(stderr, "invalid allocfail directive (TARGET K[,K...] after its alloc line): %s", line); exit(2); }
            g_alloc_fail[target] = ks;
            continue;
        }
        if (!strcmp(kw, "mockzero")) { // mockzero TARGET rDST rSIZE
            unsigned target, dst, size; char extra[8];
            if (sscanf(line, "%*s %x r%u r%u %7s", &target, &dst, &size, extra) != 3 ||
                dst < 3 || dst > 10 || size < 3 || size > 10) {
                fprintf(stderr, "invalid mockzero directive: %s", line); exit(2);
            }
            g_zero_specs[target] = {dst, size};
            continue;
        }
        if (!strcmp(kw, "nestedmockfield")) { // nestedmockfield TARGET: its mockfield lines also apply inside `real` callees
            unsigned target; char extra[8];
            if (sscanf(line, "%*s %x %7s", &target, extra) != 1) { fprintf(stderr, "invalid nestedmockfield directive: %s", line); exit(2); }
            g_nested_mockfield.insert(target);
            continue;
        }
        if (!strcmp(kw, "mockthreadspecific")) { // mockthreadspecific SET GET THREAD [CUR]; SET/GET/CUR: comma lists of targets
            char ts[3][256] = {}, th[64], extra[8];
            int k = sscanf(line, "%*s %255s %255s %63s %255s %7s", ts[0], ts[1], th, ts[2], extra);
            char* end = nullptr;
            unsigned long thread = strtoul(th, &end, 16);
            bool ok = (k == 3 || k == 4) && end && *end == 0 && thread != 0 && thread + kTlsSpecific + 4 * kTlsKeys <= 0x100000000ul &&
                      (!g_tls_thread || g_tls_thread == (uint32_t)thread);
            for (int r = 0; ok && r < 3; r++) {
                if (r == 2 && k == 3) break;
                for (char* t = strtok(ts[r], ","); ok && t; t = strtok(nullptr, ",")) {
                    char* e2 = nullptr;
                    unsigned long tg = strtoul(t, &e2, 16);
                    auto prev = g_tls_role.find((uint32_t)tg);
                    if (!e2 || *e2 || tg == 0 || tg > 0xFFFFFFFFul || (prev != g_tls_role.end() && prev->second != r)) ok = false;
                    else g_tls_role[(uint32_t)tg] = r;
                }
            }
            if (!ok) {
                fprintf(stderr, "invalid mockthreadspecific directive (SET GET THREAD [CUR], targets hex, comma lists; one THREAD): %s", line);
                exit(2);
            }
            if (!g_tls_thread) g_stable.emplace_back((uint32_t)thread + kTlsSpecific, 4 * kTlsKeys); /* the slots: never clobbered */
            g_tls_thread = (uint32_t)thread;
            continue;
        }
        if (!strcmp(kw, "mocklookup")) { // mocklookup TARGET rIDX BASE COUNT
            unsigned target, idx, base, count; char extra[8];
            if (sscanf(line, "%*s %x r%u %x %x %7s", &target, &idx, &base, &count, extra) != 4 || idx < 3 || idx > 10 || count == 0) {
                fprintf(stderr, "invalid mocklookup directive: %s", line); exit(2);
            }
            g_lookup_specs[target] = {idx, base, count};
            continue;
        }
        if (!strcmp(kw, "osmem")) { // osmem hle|PART... [dataend=HEX] [sysinfo=HEX]: the port HLE's OS/MEM backend (namespace osm)
            static char buf[512];
            snprintf(buf, sizeof buf, "%s", line);
            bool ok = true;
            unsigned np = 0;
            char* save = nullptr;
            strtok_r(buf, " \t\r\n", &save); /* the keyword */
            for (char* w = strtok_r(nullptr, " \t\r\n", &save); w && ok; w = strtok_r(nullptr, " \t\r\n", &save)) {
                if (w[0] == '#') break;
                char* e = nullptr;
                if (!strcmp(w, "hle")) np |= osm::P_ALL;
                else if (!strcmp(w, "heaps")) np |= osm::P_HEAPS;
                else if (!strcmp(w, "data")) np |= osm::P_DATA;
                else if (!strcmp(w, "gx2")) np |= osm::P_GX2;
                else if (!strcmp(w, "sysinfo")) np |= osm::P_SYSINFO;
                else if (!strcmp(w, "bss")) np |= osm::P_BSS;
                else if (!strcmp(w, "ghs")) np |= osm::P_GHS;
                else if (!strcmp(w, "zero")) np |= osm::P_ZERO;
                else if (!strcmp(w, "os")) np |= osm::P_OS;
                else if (!strcmp(w, "fs")) np |= osm::P_FS;
                else if (!strcmp(w, "uc")) np |= osm::P_UC;
                else if (!strcmp(w, "threads")) np |= osm::P_THREADS;
                else if (!strcmp(w, "ax")) np |= osm::P_AX;
                else if (!strcmp(w, "pad")) np |= osm::P_PAD;
                else if (!strcmp(w, "padpro")) osm::pad_pro = true;
                else if (!strcmp(w, "act")) np |= osm::P_ACT;
                else if (!strcmp(w, "dynload")) np |= osm::P_DYNLOAD;
                else if (!strcmp(w, "swkbd")) np |= osm::P_SWKBD;
                else if (!strcmp(w, "procui")) np |= osm::P_PROCUI;
                else if (!strcmp(w, "portconst")) np |= osm::P_PORTCONST;
                else if (!strcmp(w, "portmem")) np |= osm::P_PORTMEM;
                else if (!strcmp(w, "nodrain")) gthr::drain_on = false;
                else if (!strncmp(w, "mainthread=", 11)) {
                    unsigned long v = strtoul(w + 11, &e, 16);
                    ok = e != w + 11 && !*e && v >= 0x1000 && !(v & 7) && v + gthrm::kSize <= 0x100000000ul;
                    gthr::main_obj = (uint32_t)v;
                } else if (!strncmp(w, "threadsched=", 12)) {
                    gthr::sched_seed = strtoull(w + 12, &e, 0);
                    ok = e != w + 12 && !*e;
                } else if (!strncmp(w, "threadclock=", 12)) {
                    gthr::clock_origin = strtoull(w + 12, &e, 16);
                    ok = e != w + 12 && !*e;
                } else if (!strncmp(w, "threadbudget=", 13)) {
                    gthr::budget = strtoull(w + 13, &e, 0);
                    ok = e != w + 13 && !*e && gthr::budget >= 1;
                } else if (!strncmp(w, "threadcap=", 10)) {
                    unsigned long v = strtoul(w + 10, &e, 0);
                    ok = e != w + 10 && !*e && v >= 2 && v <= 1024;
                    gthr::cap = (uint32_t)v;
                } else if (!strncmp(w, "ostime=", 7)) { /* os_time start_bias(WWHD_OS_TIME=<seconds since 1970>) */
                    long long sec = strtoll(w + 7, &e, 10);
                    ok = e != w + 7 && !*e && sec >= gthrm::kUnixTo2000;
                    gthr::os_bias = (sec - gthrm::kUnixTo2000) * gthrm::kTicksPerSec;
                } else if (!strncmp(w, "threadslice=", 12)) {
                    gthr::slice_len = strtoull(w + 12, &e, 0);
                    ok = e != w + 12 && !*e && gthr::slice_len >= 16;
                } else if (!strncmp(w, "threadrate=", 11)) { /* clock ticks per modelled time read (0: the clock only jumps) */
                    gthr::clock_rate = strtoull(w + 11, &e, 0);
                    ok = e != w + 11 && !*e && gthr::clock_rate <= 62156250;
                } else if (!strncmp(w, "threadlog=", 10)) { /* call-log entries per run */
                    gthr::log_cap = strtoull(w + 10, &e, 0);
                    ok = e != w + 10 && !*e && gthr::log_cap >= 1;
                }
                else if (!strncmp(w, "dataend=", 8)) {
                    unsigned long v = strtoul(w + 8, &e, 16);
                    ok = e != w + 8 && !*e && v > osm::bss_start && v < osm::kMem2End;
                    osm::data_end = (uint32_t)v;
                } else if (!strncmp(w, "sysinfo=", 8)) {
                    unsigned long v = strtoul(w + 8, &e, 16);
                    ok = e != w + 8 && !*e && v && !(v & 0xF) && v + 0x20 <= 0x100000000ul;
                    osm::sysinfo = (uint32_t)v;
                } else ok = false;
            }
            bool thr_keys = strstr(line, "mainthread=") || strstr(line, "threadsched=") || strstr(line, "threadclock=") ||
                            strstr(line, "threadbudget=") || strstr(line, "threadcap=") || strstr(line, "threadslice=") || strstr(line, "ostime=") ||
                            strstr(line, "threadrate=") || strstr(line, "threadlog=") ||
                            strstr(line, " nodrain");
            if ((np & osm::P_AX) && !(np & osm::P_THREADS)) ok = false; /* the AX frame runs on a guest service thread */
            if (osm::pad_pro && !(np & osm::P_PAD)) ok = false;
            if ((np & osm::P_SWKBD) && !(np & osm::P_DYNLOAD)) ok = false; /* its exports exist only through FindExport */
            if ((np & osm::P_PORTMEM) && !(np & osm::P_THREADS)) ok = false; /* GX2Sample*: the thread model's clock */
            if (!ok || !np || osm::parts || (thr_keys && !(np & osm::P_THREADS))) {
                fprintf(stderr, "invalid osmem directive (osmem hle|heaps|data|gx2|sysinfo|bss|ghs|zero|os|fs|uc|threads|ax (ax needs threads)|pad [padpro]|act|dynload [swkbd]|procui|portconst|portmem (needs threads)... [dataend=HEX] "
                        "[sysinfo=HEX] [mainthread=HEX threadsched=N threadclock=HEX threadbudget=N threadcap=N threadslice=N threadrate=N threadlog=N ostime=SECONDS nodrain: "
                        "with threads]; once per spec): %s", line);
                exit(2);
            }
            osm::parts = np;
            gthr::on = (np & osm::P_THREADS) != 0;
            continue;
        }
        if (!strcmp(kw, "padsched")) { // padsched FRAME[-LAST] BUTTONS [LX LY RX RY] [touch=X,Y]: `osmem ... pad` press schedule
            static char buf[512];
            snprintf(buf, sizeof buf, "%s", line);
            char* save = nullptr;
            std::vector<char*> tok;
            strtok_r(buf, " \t\r\n", &save);
            for (char* w = strtok_r(nullptr, " \t\r\n", &save); w && w[0] != '#'; w = strtok_r(nullptr, " \t\r\n", &save)) tok.push_back(w);
            bool ok = true;
            padm::PadIn p;
            unsigned long long first = 0, last = 0;
            auto num = [&](const char* w, float lo, float hi, float* out) {
                char* e = nullptr;
                float v = strtof(w, &e);
                if (e == w || *e || !std::isfinite(v) || v < lo || v > hi) return false;
                *out = v;
                return true;
            };
            if (tok.size() < 2) ok = false;
            else {
                char* e = nullptr;
                if (!isdigit((unsigned char)tok[0][0])) ok = false;
                else {
                    first = strtoull(tok[0], &e, 10);
                    last = first;
                    if (*e == '-' && isdigit((unsigned char)e[1])) { const char* f = e + 1; last = strtoull(f, &e, 10); }
                    if (*e || last < first) ok = false;
                }
                const char* b = tok[1];
                if (ok && (!strcmp(b, "-") || !strcmp(b, "0"))) p.buttons = 0;
                else if (ok && (!strncmp(b, "0x", 2) || !strncmp(b, "0X", 2))) {
                    unsigned long v = strtoul(b + 2, &e, 16);
                    ok = e != b + 2 && !*e && !(v & ~(unsigned long)padm::kButtonMask);
                    p.buttons = (uint32_t)v;
                } else if (ok) {
                    static const struct { const char* n; uint32_t bit; } names[] = {
                        {"A", padm::kA}, {"B", padm::kB}, {"X", padm::kX}, {"Y", padm::kY}, {"L", padm::kL}, {"R", padm::kR},
                        {"ZL", padm::kZL}, {"ZR", padm::kZR}, {"PLUS", padm::kPlus}, {"MINUS", padm::kMinus}, {"HOME", padm::kHome},
                        {"UP", padm::kUp}, {"DOWN", padm::kDown}, {"LEFT", padm::kLeft}, {"RIGHT", padm::kRight},
                        {"STICKL", padm::kStickL}, {"STICKR", padm::kStickR}};
                    char nb[256];
                    snprintf(nb, sizeof nb, "%s", b);
                    char* s2 = nullptr;
                    for (char* n = strtok_r(nb, "+", &s2); n && ok; n = strtok_r(nullptr, "+", &s2)) {
                        bool found = false;
                        for (auto& x : names) if (!strcmp(n, x.n)) { p.buttons |= x.bit; found = true; }
                        ok = found;
                    }
                    if (b[0] == '+' || b[strlen(b) - 1] == '+' || strstr(b, "++")) ok = false;
                }
                size_t i = 2;
                if (ok && tok.size() >= 6 && strncmp(tok[2], "touch=", 6)) {
                    ok = num(tok[2], -1, 1, &p.lx) && num(tok[3], -1, 1, &p.ly) && num(tok[4], -1, 1, &p.rx) && num(tok[5], -1, 1, &p.ry);
                    i = 6;
                }
                if (ok && i < tok.size() && !strncmp(tok[i], "touch=", 6)) {
                    char tb[128];
                    snprintf(tb, sizeof tb, "%s", tok[i] + 6);
                    char* comma = strchr(tb, ',');
                    if (!comma) ok = false;
                    else {
                        *comma = 0;
                        ok = num(tb, 0, 1, &p.tx) && num(comma + 1, 0, 1, &p.ty);
                        p.touch = true;
                    }
                    i++;
                }
                if (i != tok.size()) ok = false;
            }
            if (!ok) {
                fprintf(stderr, "invalid padsched directive (padsched FRAME[-LAST] BUTTONS [LX LY RX RY] [touch=X,Y]; FRAME = the read's "
                        "0-based ordinal; BUTTONS = 0, -, 0xHEX or names joined by + (A B X Y L R ZL ZR PLUS MINUS HOME UP DOWN LEFT "
                        "RIGHT STICKL STICKR); sticks -1..1, touch 0..1): %s", line);
                exit(2);
            }
            if (!osm::pad_sched.add(first, last, p)) {
                fprintf(stderr, "padsched: reads %llu..%llu overlap an earlier padsched line: %s", first, last, line);
                exit(2);
            }
            osm::pad_sched_seen = true;
            continue;
        }
        if (!strcmp(kw, "capture")) { /* capture NAME SHA256 (namespace capm) */
            char nm[512], hs[80], extra[8];
            if (capm::present || sscanf(line, "%*s %511s %79s %7s", nm, hs, extra) != 2 || strlen(hs) != 64 || strstr(nm, "..")) {
                fprintf(stderr, "invalid capture directive (capture NAME SHA256, once): %s", line); exit(2);
            }
            std::string store = getenv("WWHD_CAPTURE_STORE") ? getenv("WWHD_CAPTURE_STORE")
                                                             : std::string("build/capture");
            std::string path = store + "/" + nm;
            capm::dir = path.substr(0, path.rfind('/'));
            /* NAME.zst: zstd-compressed; the SHA-256 is of the decompressed WWCAPMEM bytes either way */
            const bool zst = path.size() > 4 && path.compare(path.size() - 4, 4, ".zst") == 0;
            struct stat zsb;
            if (stat(path.c_str(), &zsb) != 0 || path.find('\'') != std::string::npos) { fprintf(stderr, "capture %s: cannot open %s\n", nm, path.c_str()); exit(2); }
            FILE* fp = zst ? popen(("zstd -q -d -c '" + path + "'").c_str(), "r") : fopen(path.c_str(), "rb");
            if (!fp) { fprintf(stderr, "capture %s: cannot open %s\n", nm, path.c_str()); exit(2); }
            uint8_t buf[1 << 16];
            size_t k;
            capm::file.reserve(0x30000000u);
            while ((k = fread(buf, 1, sizeof buf, fp)) > 0) capm::file.insert(capm::file.end(), buf, buf + k);
            if (zst ? pclose(fp) != 0 : fclose(fp) != 0) { fprintf(stderr, "capture %s: read of %s failed\n", nm, path.c_str()); exit(2); }
            std::string sha = osm::sha256_hex(capm::file), want = hs;
            for (char& ch : want) ch = (char)tolower((unsigned char)ch);
            if (sha != want) { fprintf(stderr, "capture %s: SHA-256 %s, the spec says %s\n", nm, sha.c_str(), want.c_str()); exit(2); }
            const std::vector<uint8_t>& f = capm::file;
            auto u32le = [&](size_t o) { uint32_t v; memcpy(&v, &f[o], 4); return v; };
            if (f.size() < 16 || memcmp(f.data(), "WWCAPMEM", 8) || u32le(8) != 1 || u32le(12) != 0x1000 || (f.size() - 16) % 0x1004) {
                fprintf(stderr, "capture %s: not a version-1 WWCAPMEM file\n", nm); exit(2);
            }
            for (size_t o = 16; o < f.size(); o += 0x1004) capm::pages[u32le(o)] = &f[o + 4];
            capm::present = true;
            if (const char* rl = getenv("WWHD_CAPTURE_READLOG")) {
                static std::string rlpath = rl;
                capm::readlog = new std::set<uint32_t>();
                atexit([] { capm::dump_runs(*capm::readlog, rlpath); });
            }
            if (const char* gl = getenv("WWHD_CAPTURE_GENLOG")) {
                static std::string glpath = gl;
                capm::genlog = new std::set<uint32_t>();
                atexit([] { capm::dump_runs(*capm::genlog, glpath); });
            }
            continue;
        }
        if (!strcmp(kw, "capturemap")) { /* capturemap ADDR SIZE: bytes served from the capture */
            unsigned a, n; char extra[8];
            if (sscanf(line, "%*s %x %x %7s", &a, &n, extra) != 2 || !n || (uint64_t)a + n > 0x100000000ull) {
                fprintf(stderr, "invalid capturemap directive (capturemap ADDR SIZE): %s", line); exit(2);
            }
            if (!getenv("WWHD_CAPTURE_READLOG"))
                for (const auto& o : capm::kOff)
                    if (a < o.hi && o.lo < a + n) { fprintf(stderr, "capturemap %08X %X: overlaps %s, never a window\n", a, n, o.what); exit(2); }
            capm::map.emplace_back(a, a + n);
            std::sort(capm::map.begin(), capm::map.end());
            for (size_t i = 1; i < capm::map.size(); i++)
                if (capm::map[i].first < capm::map[i - 1].second) { fprintf(stderr, "capturemap ranges overlap: %s", line); exit(2); }
            continue;
        }
        if (!strcmp(kw, "capturecall")) { /* capturecall SHA256: the manifest beside the capture, the drift guard for arg! */
            char hs[80], extra[8];
            if (!capm::present || capm::call_func || sscanf(line, "%*s %79s %7s", hs, extra) != 1 || strlen(hs) != 64) {
                fprintf(stderr, "invalid capturecall directive (capturecall SHA256, once, after capture): %s", line); exit(2);
            }
            std::string path = capm::dir + "/capture.txt";
            FILE* fp = fopen(path.c_str(), "rb");
            if (!fp) { fprintf(stderr, "capturecall: cannot open %s\n", path.c_str()); exit(2); }
            std::vector<uint8_t> txt;
            uint8_t buf[1 << 14];
            size_t k;
            while ((k = fread(buf, 1, sizeof buf, fp)) > 0) txt.insert(txt.end(), buf, buf + k);
            fclose(fp);
            std::string sha = osm::sha256_hex(txt), want = hs;
            for (char& ch : want) ch = (char)tolower((unsigned char)ch);
            if (sha != want) { fprintf(stderr, "capturecall: %s SHA-256 %s, the spec says %s\n", path.c_str(), sha.c_str(), want.c_str()); exit(2); }
            std::string t(txt.begin(), txt.end());
            size_t pos = 0;
            while (pos < t.size()) {
                size_t e = t.find('\n', pos);
                if (e == std::string::npos) e = t.size();
                std::string l = t.substr(pos, e - pos);
                pos = e + 1;
                unsigned v; int r;
                if (sscanf(l.c_str(), "func %x", &v) == 1) capm::call_func = v;
                else if (sscanf(l.c_str(), "r%d %x", &r, &v) == 2 && r >= 0 && r < 32) capm::call_regs[r] = v;
            }
            if (!capm::call_func || capm::call_regs.size() != 32) { fprintf(stderr, "capturecall: %s is not a capture manifest\n", path.c_str()); exit(2); }
            continue;
        }
        if (!strcmp(kw, "osmemfile")) { // osmemfile GUESTPATH SHA256|absent: a game file the `osmem fs` model may read
            char gp[1024], hs[80], extra[8];
            if (sscanf(line, "%*s %1023s %79s %7s", gp, hs, extra) != 2 || (strcmp(hs, "absent") && strlen(hs) != 64)) {
                fprintf(stderr, "invalid osmemfile directive (osmemfile GUESTPATH SHA256|absent): %s", line); exit(2);
            }
            osm::FsFile f;
            f.guest = gp;
            f.host = osm::fs_host_exact(gp, "game");
            if (f.host.empty()) { fprintf(stderr, "osmemfile %s: /vol/save is not modelled\n", gp); exit(2); }
            f.key = osm::fs_key(f.host);
            for (const osm::FsFile& o : osm::fs_files)
                if (o.key == f.key) { fprintf(stderr, "osmemfile %s: declared twice\n", gp); exit(2); }
            /* fs.cpp resolve_case: case-insensitive component lookup under game/ (read only) */
            std::string cur, rest = f.host;
            bool found = true;
            for (size_t pos = 0; pos <= rest.size() && found;) {
                size_t e = rest.find('/', pos);
                if (e == std::string::npos) e = rest.size();
                std::string comp = rest.substr(pos, e - pos);
                pos = e + 1;
                if (comp.empty()) { if (e == rest.size()) break; continue; }
                std::string cand = cur.empty() ? comp : cur + "/" + comp;
                struct stat sb;
                if (stat(cand.c_str(), &sb) != 0) {
                    found = false;
                    if (DIR* d = opendir(cur.empty() ? "." : cur.c_str())) {
                        while (dirent* de = readdir(d))
                            if (!strcasecmp(de->d_name, comp.c_str())) { cand = (cur.empty() ? "" : cur + "/") + de->d_name; found = true; break; }
                        closedir(d);
                    }
                }
                cur = cand;
            }
            if (!strcmp(hs, "absent")) {
                struct stat sb;
                if (found && stat(cur.c_str(), &sb) == 0) { fprintf(stderr, "osmemfile %s: declared absent but %s exists\n", gp, cur.c_str()); exit(2); }
                f.absent = true;
            } else {
                FILE* fp = found ? fopen(cur.c_str(), "rb") : nullptr;
                if (!fp) { fprintf(stderr, "osmemfile %s: cannot open %s (read only, under the game link)\n", gp, f.host.c_str()); exit(2); }
                uint8_t buf[1 << 16];
                size_t k;
                while ((k = fread(buf, 1, sizeof buf, fp)) > 0) f.data.insert(f.data.end(), buf, buf + k);
                fclose(fp);
                f.sha = osm::sha256_hex(f.data);
                std::string want = hs;
                for (char& ch : want) ch = (char)tolower((unsigned char)ch);
                if (f.sha != want) { fprintf(stderr, "osmemfile %s: SHA-256 %s, the spec says %s\n", gp, f.sha.c_str(), want.c_str()); exit(2); }
            }
            osm::fs_files.push_back(std::move(f));
            continue;
        }
        if (!strcmp(kw, "mocksurfacesize")) { // mocksurfacesize TARGET
            unsigned target; char extra[8];
            if (sscanf(line, "%*s %x %7s", &target, extra) != 1) { fprintf(stderr, "invalid mocksurfacesize directive: %s", line); exit(2); }
            g_surface_size.insert(target);
            continue;
        }
        if (!strcmp(kw, "mockcopysurface")) { // mockcopysurface TARGET
            unsigned target; char extra[8];
            if (sscanf(line, "%*s %x %7s", &target, extra) != 1) { fprintf(stderr, "invalid mockcopysurface directive: %s", line); exit(2); }
            g_copy_surface.insert(target);
            continue;
        }
        if (!strcmp(kw, "mockmove")) { // mockmove TARGET rDST rSRC rSIZE
            unsigned target, dst, src, size;
            if (sscanf(line, "%*s %x r%u r%u r%u", &target, &dst, &src, &size) != 4 ||
                dst < 3 || dst > 10 || src < 3 || src > 10 || size < 3 || size > 10) {
                fprintf(stderr, "invalid mockmove directive: %s", line); exit(2);
            }
            g_move_specs[target] = {dst, src, size};
            continue;
        }
        if (!strcmp(kw, "mockretain")) { // mockretain TARGET rN SIZE
            unsigned target, reg, size;
            char extra[8];
            if (sscanf(line, "%*s %x r%u %i %7s", &target, &reg, (int*)&size, extra) != 3 || reg < 3 || reg > 10 ||
                size == 0 || size > 4096) {
                fprintf(stderr, "invalid mockretain directive (TARGET rN SIZE, SIZE 1..4096): %s", line); exit(2);
            }
            g_retain_specs[target] = {reg, size};
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
            if (m < 4 || (!strcmp(kw,"mockfield") && wh[0] != 'r' && wh[0] != '[')) { fprintf(stderr, "invalid mockfield directive: %s", line); exit(2); }
            if (!strcmp(kw, "mockglobal")) {
                char* end;
                mf.reg = 0;
                mf.off = (uint32_t)strtoul(wh, &end, 16);
                if (*end || (strcmp(tyb,"u8") && strcmp(tyb,"u16") && strcmp(tyb,"u32") && strcmp(tyb,"f32"))) {
                    fprintf(stderr,"invalid mockglobal directive: %s",line); exit(2);
                }
            } else if (wh[0] == '[') { /* [rN+OFF]+OFF2 */
                char* e = nullptr;
                if (wh[1] != 'r') { fprintf(stderr, "invalid mockfield directive: %s", line); exit(2); }
                mf.reg = (int)strtol(wh + 2, &e, 10);
                mf.off = *e == '+' ? (uint32_t)strtoul(e + 1, &e, 0) : 0;
                if (*e != ']') { fprintf(stderr, "invalid mockfield directive: %s", line); exit(2); }
                e++;
                mf.off2 = *e == '+' ? (uint32_t)strtoul(e + 1, &e, 0) : 0;
                if (*e) { fprintf(stderr, "invalid mockfield directive: %s", line); exit(2); }
                mf.deref = true;
            } else {
                mf.reg = (int)strtol(wh + 1, &plus, 10);
                mf.off = plus && *plus == '+' ? (uint32_t)strtoul(plus + 1, nullptr, 0) : 0;
            }
            mf.is_float = tyb[0] == 'f';
            mf.size = atoi(tyb + 1) / 8;
            if ((!strcmp(kw,"mockfield") && (mf.reg < 3 || mf.reg > 10)) || !(mf.size == 1 || mf.size == 2 || mf.size == 4)) { fprintf(stderr, "invalid mockfield directive: %s", line); exit(2); }
            if (strchr(vals, ',')) {
                for (char* t = strtok(vals, ","); t; t = strtok(nullptr, ","))
                    mf.pick.push_back(spec_num(t, mf.is_float, kw));
            } else {
                mf.lo = spec_num(vals, mf.is_float, kw);
                mf.hi = m >= 5 ? spec_num(hib, mf.is_float, kw) : mf.lo;
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
                        std::string sp = g_verify_dir + "/stage_names.txt";
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
        if (!strcmp(kw, "fill")) { /* fill FUNC|* ADDR|rN+OFF SIZE BYTE */
            FillSpec fs{};
            char fb[32], wb[64], sb[64], bb[64], extra[8];
            if (sscanf(line, "%*s %31s %63s %63s %63s %7s", fb, wb, sb, bb, extra) != 4) { fprintf(stderr, "invalid fill directive: %s", line); exit(2); }
            fs.func = spec_func(fb, kw);
            if (wb[0] == 'r') {
                char* plus;
                fs.base_reg = (int)strtol(wb + 1, &plus, 10);
                if (plus == wb + 1 || (*plus && *plus != '+')) spec_bad_token(kw, wb, "address");
                fs.off = *plus == '+' ? (uint32_t)spec_uint(plus + 1, 0, kw, "offset") : 0;
                if (fs.base_reg < 0 || fs.base_reg > 31) { fprintf(stderr, "invalid fill register: %s", line); exit(2); }
            } else {
                fs.base_reg = -1;
                fs.off = (uint32_t)spec_uint(wb, 16, kw, "address");
            }
            unsigned long sz = spec_uint(sb, 0, kw, "size"), by = spec_uint(bb, 0, kw, "byte");
            if (sz == 0 || sz > 0x1000000ul || by > 0xFF) { fprintf(stderr, "invalid fill size/byte (1..0x1000000, 0..0xFF): %s", line); exit(2); }
            fs.size = (uint32_t)sz;
            fs.byte = (uint8_t)by;
            g_fill_specs.push_back(fs);
            continue;
        }
        if (!strcmp(kw, "cr")) { /* cr FUNC|* BIT V[,V...] */
            CrSpec cs{};
            char fb[32], bb[16], vb[64], extra[8];
            if (sscanf(line, "%*s %31s %15s %63s %7s", fb, bb, vb, extra) != 3) { fprintf(stderr, "invalid cr directive: %s", line); exit(2); }
            cs.func = strcmp(fb, "*") ? (uint32_t)strtoul(fb, nullptr, 16) : 0;
            char* e;
            long bit = strtol(bb, &e, 10);
            if (*e || bit < 0 || bit > 31) { fprintf(stderr, "invalid cr bit (0..31): %s", line); exit(2); }
            cs.bit = (int)bit;
            for (char* t = strtok(vb, ","); t; t = strtok(nullptr, ",")) {
                if (strcmp(t, "0") && strcmp(t, "1")) { fprintf(stderr, "invalid cr value (0 or 1): %s", line); exit(2); }
                cs.pick.push_back((uint8_t)(t[0] - '0'));
            }
            if (cs.pick.empty()) { fprintf(stderr, "cr without values: %s", line); exit(2); }
            g_cr_specs.push_back(cs);
            continue;
        }
        if (!strcmp(kw, "copy") || !strcmp(kw, "blob")) { /* copy FUNC|* DST|rN+OFF SRC SIZE ; blob FUNC|* DST|rN+OFF FILE OFFSET SIZE */
            CopySpec cs{};
            cs.blob = kw[0] == 'b';
            char fb[32], db[64], sb[512], ob[64], zb[64], extra[8];
            int n = cs.blob ? sscanf(line, "%*s %31s %63s %511s %63s %63s %7s", fb, db, sb, ob, zb, extra)
                            : sscanf(line, "%*s %31s %63s %63s %63s %7s", fb, db, sb, zb, extra);
            if (n != (cs.blob ? 5 : 4)) { fprintf(stderr, "invalid %s directive: %s", kw, line); exit(2); }
            cs.func = strcmp(fb, "*") ? (uint32_t)strtoul(fb, nullptr, 16) : 0;
            if (!parse_dst(db, &cs)) { fprintf(stderr, "invalid %s destination: %s", kw, line); exit(2); }
            unsigned long sz = strtoul(zb, nullptr, 0);
            if (sz == 0 || sz > 0x1000000ul) { fprintf(stderr, "invalid %s size (1..0x1000000): %s", kw, line); exit(2); }
            cs.size = (uint32_t)sz;
            if (cs.blob) {
                std::string rel = sb;
                if (rel.empty() || rel[0] == '/' || rel.find("..") != std::string::npos) {
                    fprintf(stderr, "blob file must be a path inside game/ (no absolute path, no ..): %s", line); exit(2);
                }
                std::string path = "game/" + rel;
                FILE* bf = fopen(path.c_str(), "rb");
                if (!bf) { fprintf(stderr, "blob: cannot open %s (the game/ link must point at the user's game files): %s", path.c_str(), line); exit(2); }
                unsigned long off = strtoul(ob, nullptr, 0);
                cs.data.resize(cs.size);
                if (fseek(bf, (long)off, SEEK_SET) || fread(cs.data.data(), 1, cs.size, bf) != cs.size) {
                    fclose(bf);
                    fprintf(stderr, "blob: %s has fewer than OFFSET+SIZE bytes: %s", path.c_str(), line); exit(2);
                }
                fclose(bf);
            } else {
                char* e;
                cs.src = (uint32_t)strtoul(sb, &e, 16);
                if (*e) { fprintf(stderr, "invalid copy source: %s", line); exit(2); }
            }
            g_copy_specs.push_back(cs);
            continue;
        }
        if (!strcmp(kw, "blobfile")) { /* blobfile FUNC|* DST|rN+OFF FILE[:STEP...] [OFFSET SIZE] (src/blobfile.h) */
            CopySpec cs{};
            cs.blob = true;
            char fb[32], db[64], sb[1024], ob[64], zb[64], extra[8];
            int n = sscanf(line, "%*s %31s %63s %1023s %63s %63s %7s", fb, db, sb, ob, zb, extra);
            if (n != 3 && n != 5) { fprintf(stderr, "invalid blobfile directive (FUNC DST FILE[:STEP...] [OFFSET SIZE]): %s", line); exit(2); }
            cs.func = strcmp(fb, "*") ? (uint32_t)strtoul(fb, nullptr, 16) : 0;
            if (!parse_dst(db, &cs)) { fprintf(stderr, "invalid blobfile destination: %s", line); exit(2); }
            std::vector<std::string> chain;
            for (const char *p = sb, *q; ; p = q + 1) {
                q = strchr(p, ':');
                chain.emplace_back(p, q ? (size_t)(q - p) : strlen(p));
                if (!q) break;
            }
            for (const std::string& part : chain)
                if (part.empty()) { fprintf(stderr, "blobfile: empty path step: %s", line); exit(2); }
            const std::string& rel = chain[0];
            if (rel[0] == '/' || rel.find("..") != std::string::npos) {
                fprintf(stderr, "blobfile file must be a path inside game/ (no absolute path, no ..): %s", line); exit(2);
            }
            std::string path = "game/" + rel;
            FILE* bf = fopen(path.c_str(), "rb");
            if (!bf) { fprintf(stderr, "blobfile: cannot open %s (the game/ link must point at the user's game files): %s", path.c_str(), line); exit(2); }
            std::vector<uint8_t> cur;
            for (int ch; (ch = fgetc(bf)) != EOF;) cur.push_back((uint8_t)ch);
            fclose(bf);
            std::string err;
            for (size_t k = 1; k < chain.size(); k++)
                if (!blobfile::step(&cur, chain[k], &err)) { fprintf(stderr, "blobfile: %s: %s", err.c_str(), line); exit(2); }
            uint64_t off = 0, sz = cur.size();
            if (n == 5) {
                off = strtoull(ob, nullptr, 0);
                sz = strtoull(zb, nullptr, 0);
                if (off + sz > cur.size()) { fprintf(stderr, "blobfile: the file has 0x%zX bytes, fewer than OFFSET+SIZE: %s", cur.size(), line); exit(2); }
            }
            if (sz == 0 || sz > 0x1000000u) { fprintf(stderr, "invalid blobfile size 0x%llX (1..0x1000000): %s", (unsigned long long)sz, line); exit(2); }
            cs.size = (uint32_t)sz;
            cs.data.assign(cur.begin() + (size_t)off, cur.begin() + (size_t)(off + sz));
            if (const char* dump = getenv("WWHD_VERIFY_BLOBDUMP")) { /* self-test only (blobfile_test.py): the placed bytes */
                static int dumped = 0;
                std::string dp = std::string(dump) + "/blobfile_" + std::to_string(dumped++) + ".bin";
                if (FILE* df = fopen(dp.c_str(), "wb")) { fwrite(cs.data.data(), 1, cs.data.size(), df); fclose(df); }
            }
            g_copy_specs.push_back(cs);
            continue;
        }
        if (!strcmp(kw, "stopat")) { /* stopat FUNC TARGET N [live REG ...] */
            char fb[32], tb[32], nb[32], lw[16];
            int pos = 0;
            if (sscanf(line, "%*s %31s %31s %31s%n", fb, tb, nb, &pos) != 3) { fprintf(stderr, "invalid stopat directive: %s", line); exit(2); }
            char *e1, *e2, *e3;
            uint32_t f = (uint32_t)strtoul(fb, &e1, 16), t = (uint32_t)strtoul(tb, &e2, 16);
            long n = strtol(nb, &e3, 0);
            if (*e1 || *e2 || *e3 || !f || !t || n < 1 || n > 1000000) { fprintf(stderr, "invalid stopat FUNC TARGET N (N >= 1): %s", line); exit(2); }
            StopSpec st{t, (int)n};
            const char* q = line + pos; /* after N (%n counts from the start of the line) */
            int used = 0;
            if (sscanf(q, "%15s%n", lw, &used) == 1) {
                if (strcmp(lw, "live")) { fprintf(stderr, "invalid stopat FUNC TARGET N [live REG ...]: %s", line); exit(2); }
                q += used;
                char rb[16];
                int any = 0;
                while (sscanf(q, "%15s%n", rb, &used) == 1) {
                    q += used;
                    char* e;
                    long r = strtol(rb + 1, &e, 10);
                    if ((rb[0] != 'r' && rb[0] != 'f') || e == rb + 1 || *e || r < 0 || r > 31) {
                        fprintf(stderr, "invalid stopat live register %s (r0..r31, f0..f31): %s", rb, line); exit(2);
                    }
                    (rb[0] == 'r' ? st.live_r : st.live_f) |= 1u << r;
                    any = 1;
                }
                if (!any) { fprintf(stderr, "invalid stopat: `live` without a register: %s", line); exit(2); }
            }
            g_stop_specs[f] = st;
            continue;
        }
        if (!strcmp(kw, "stable")) { /* stable ADDR SIZE */
            char sa[64], sb[64];
            if (sscanf(line, "%*s %63s %63s", sa, sb) == 2)
                g_stable.emplace_back((uint32_t)strtoul(sa, nullptr, 16), (uint32_t)strtoul(sb, nullptr, 0));
            continue;
        }
        if (!strcmp(kw, "observearg")) { /* observearg TARGET[@N] rN+OFF SIZE [if rK=V | if lr=ADDR] */
            /* strict (2026-10-28): every token must parse completely; bad tokens exit 2 */
            char buf[512];
            snprintf(buf, sizeof buf, "%s", line);
            std::vector<char*> t;
            for (char* p = strtok(buf, " \t\r\n"); p; p = strtok(nullptr, " \t\r\n")) t.push_back(p);
            auto bad = [&](const char* why) {
                fprintf(stderr, "%s:%d: invalid observearg directive (%s): %s", g_spec_path, g_spec_lineno, why, g_spec_line);
                if (!*g_spec_line || g_spec_line[strlen(g_spec_line) - 1] != '\n') fputc('\n', stderr);
                exit(2);
            };
            if (t.size() != 4 && t.size() != 6)
                bad("expected TARGET[@N] rN+OFF SIZE [if rK=V | if lr=ADDR]");
            ObservePtr o{0, 0, 0}; o.direct = true;
            char* at = strchr(t[1], '@');
            if (at) {
                *at = 0;
                if (!at[1]) bad("empty call index after @");
                unsigned long n = spec_uint(at + 1, 10, "observearg", "call index");
                if (n > 0xFFFF) bad("call index 0..65535");
                o.site = (int)n;
            }
            uint32_t target = (uint32_t)spec_uint(t[1], 16, "observearg", "target address");
            if (const char* why = parse_span(t[2], o)) bad(why);
            o.size = (uint32_t)spec_uint(t[3], 10, "observearg", "size");
            if (o.size == 0 || o.size > 256) bad("size 1..256");
            if (t.size() == 6) {
                if (strcmp(t[4], "if")) bad("expected `if rK=V` or `if lr=ADDR`");
                char* eq = strchr(t[5], '=');
                if (!eq || !eq[1]) bad("condition rK=V or lr=ADDR");
                *eq = 0;
                if (!strcmp(t[5], "lr")) {
                    o.creg = 32;
                    unsigned long long v = (unsigned long long)spec_uint(eq + 1, 16, "observearg", "return address");
                    if (v > 0xFFFFFFFFull) bad("return address is 32 bits");
                    o.cval = (uint32_t)v;
                } else {
                    char* ce = nullptr;
                    if (t[5][0] != 'r' || !isdigit((unsigned char)t[5][1])) bad("condition register r3..r10 or lr");
                    unsigned long k = strtoul(t[5] + 1, &ce, 10);
                    if (*ce || k < 3 || k > 10) bad("condition register r3..r10 or lr");
                    o.creg = (int)k;
                    unsigned long long v = (unsigned long long)spec_uint(eq + 1, 0, "observearg", "condition value");
                    if (v > 0xFFFFFFFFull) bad("condition value is 32 bits");
                    o.cval = (uint32_t)v;
                }
            }
            /* two spans that can apply to the same call must not overlap: same register, overlapping bytes, sites that
             * can coincide (equal, or either unrestricted) and conditions that can hold together (not the same
             * condition register with different values) */
            for (const ObservePtr& p : g_observeptr_specs[target]) {
                if (!p.direct || p.reg != o.reg || p.deref != o.deref || (o.deref && p.off != o.off)) continue;
                uint32_t po = p.deref ? p.off2 : p.off, oo = o.deref ? o.off2 : o.off;
                if (!(oo < po + p.size && po < oo + o.size)) continue;
                bool sites = p.site < 0 || o.site < 0 || p.site == o.site;
                bool conds = !(p.creg && o.creg && p.creg == o.creg && p.cval != o.cval);
                if (sites && conds) {
                    fprintf(stderr, "%s:%d: observearg spans overlap: %s", g_spec_path, g_spec_lineno, g_spec_line); exit(2);
                }
            }
            g_observeptr_specs[target].push_back(o);
            continue;
        }
        if (!strcmp(kw, "observeptr")) { /* observeptr TARGET rN+OFF SIZE | observeptr TARGET [rN+OFF]+OFF2 SIZE */
            unsigned target, size; char wh[64], extra[8];
            if (sscanf(line, "%*s %x %63s %u %7s", &target, wh, &size, extra) != 3 || (wh[0] != 'r' && wh[0] != '[')) {
                fprintf(stderr, "invalid observeptr directive (observeptr TARGET rN+OFF SIZE or TARGET [rN+OFF]+OFF2 SIZE): %s", line); exit(2);
            }
            if (wh[0] == '[') { /* the pointee-offset form: the same span as `observearg TARGET [rN+OFF]+OFF2 SIZE` */
                ObservePtr o{0, 0, 0}; o.direct = true;
                const char* why = parse_span(wh, o);
                if (why || size == 0 || size > 256) {
                    fprintf(stderr, "invalid observeptr directive (%s): %s", why ? why : "size 1..256", line); exit(2);
                }
                o.size = size;
                for (const ObservePtr& p : g_observeptr_specs[target])
                    if (p.deref && p.reg == o.reg && p.off == o.off && o.off2 < p.off2 + p.size && p.off2 < o.off2 + o.size) {
                        fprintf(stderr, "%s:%d: observearg spans overlap: %s", g_spec_path, g_spec_lineno, g_spec_line); exit(2);
                    }
                g_observeptr_specs[target].push_back(o);
                continue;
            }
            char* e = nullptr;
            unsigned reg = (unsigned)strtoul(wh + 1, &e, 10);
            uint32_t off = *e == '+' ? (uint32_t)strtoul(e + 1, &e, 0) : 0;
            if (*e || reg < 3 || reg > 10 || off > 0xFF || size == 0 || size > 256) {
                fprintf(stderr, "invalid observeptr directive (r3..r10, offset <= 0xFF, size 1..256): %s", line); exit(2);
            }
            g_observeptr_specs[target].push_back({reg, off, size});
            continue;
        }
        if (!strcmp(kw, "observenested")) { /* observenested TARGET [SPAN SIZE]; SPAN = rN+OFF or [rN+OFF]+OFF2 */
            char buf[512];
            snprintf(buf, sizeof buf, "%s", line);
            std::vector<char*> t;
            for (char* p = strtok(buf, " \t\r\n"); p; p = strtok(nullptr, " \t\r\n")) t.push_back(p);
            auto bad = [&](const char* why) {
                fprintf(stderr, "%s:%d: invalid observenested directive (%s): %s", g_spec_path, g_spec_lineno, why, g_spec_line);
                if (!*g_spec_line || g_spec_line[strlen(g_spec_line) - 1] != '\n') fputc('\n', stderr);
                exit(2);
            };
            if (t.size() != 2 && t.size() != 4) bad("expected TARGET [rN+OFF SIZE | [rN+OFF]+OFF2 SIZE]");
            uint32_t target = (uint32_t)spec_uint(t[1], 16, "observenested", "target address");
            std::vector<ObservePtr>& v = g_nested_specs[target];
            if (t.size() == 4) {
                ObservePtr o{0, 0, 0}; o.direct = true;
                if (const char* why = parse_span(t[2], o)) bad(why);
                o.size = (uint32_t)spec_uint(t[3], 10, "observenested", "size");
                if (o.size == 0 || o.size > 256) bad("size 1..256");
                v.push_back(o);
            }
            continue;
        }
        if (!strcmp(kw, "callcr")) { /* callcr TARGET BIT[,BIT...] */
            unsigned target; char bb[128], extra[8];
            if (sscanf(line, "%*s %x %127s %7s", &target, bb, extra) != 2) { fprintf(stderr, "invalid callcr directive: %s", line); exit(2); }
            uint32_t mask = 0;
            for (char* t = strtok(bb, ","); t; t = strtok(nullptr, ",")) {
                char* e; long bit = strtol(t, &e, 10);
                if (*e || bit < 0 || bit > 31) { fprintf(stderr, "invalid callcr bit (0..31): %s", line); exit(2); }
                mask |= 1u << bit;
            }
            g_callcr_specs[target] |= mask;
            continue;
        }
        if (!strcmp(kw, "alloc")) { /* alloc TARGET rSIZE BASE [ALIGN | align=rN] */
            unsigned target, reg, base, align = 32, alignreg = 0;
            char at[32] = "", extra[8];
            int m = sscanf(line, "%*s %x r%u %x %31s %7s", &target, &reg, &base, at, extra);
            bool ok = m >= 3 && m <= 4 && reg >= 3 && reg <= 10;
            if (ok && m == 4) {
                char* e = nullptr;
                if (!strncmp(at, "align=r", 7)) {
                    alignreg = (unsigned)strtoul(at + 7, &e, 10);
                    ok = *e == 0 && alignreg >= 3 && alignreg <= 10 && alignreg != reg;
                } else {
                    align = (unsigned)strtoul(at, &e, 10);
                    ok = *e == 0;
                }
            }
            if (!ok || align == 0 || (align & (align - 1))) {
                fprintf(stderr, "invalid alloc directive: %s", line); exit(2);
            }
            g_alloc_specs[target] = {reg, base, align, 0, alignreg};
            continue;
        }
        if (!strcmp(kw, "poke")) { /* poke FUNC|* ADDR|rN+OFF u8|u16|u32|f32 VALUE */
            static char val[4096]; char pfn[32], where_[64], ty_[16], extra[8]; /* VALUE: a choice list may be long (AGL#18 padding companion) */
            if (sscanf(line, "%*s %31s %63s %15s %4095s %7s", pfn, where_, ty_, val, extra) != 4) {
                fprintf(stderr, "invalid poke directive (FUNC|* ADDR|rN+OFF TYPE VALUE): %s", line); exit(2);
            }
            PokeSpec pk{}; char* e;
            pk.func = strcmp(pfn, "*") ? (uint32_t)strtoul(pfn, &e, 16) : 0;
            bool ok = !strcmp(pfn, "*") || (*e == 0 && strlen(pfn) == 8);
            pk.base_reg = -1;
            if (where_[0] == 'r') {
                unsigned reg = 0; unsigned off = 0; char* plus = strchr(where_, '+');
                ok = ok && sscanf(where_, "r%u", &reg) == 1 && ((reg >= 3 && reg <= 10) || reg == 1);
                if (plus) { off = (unsigned)strtoul(plus + 1, &e, 0); ok = ok && *e == 0; }
                /* r1+OFF (2026-10-26, AGL#18 selector): a stack-argument word in the caller's frame at the entry SP;
                 * OFF >= 8 (below that are the back chain / LR save words) */
                if (reg == 1) ok = ok && plus && off >= 8;
                pk.base_reg = (int)reg; pk.off = off;
            } else {
                pk.off = (uint32_t)strtoul(where_, &e, 16); ok = ok && *e == 0 && strlen(where_) == 8;
            }
            if (!strcmp(ty_, "u8")) pk.size = 1; else if (!strcmp(ty_, "u16")) pk.size = 2;
            else if (!strcmp(ty_, "u32")) pk.size = 4; else if (!strcmp(ty_, "f32")) pk.size = 5; else ok = false;
            /* VALUE may be a comma-separated choice list: one value is picked per input (the same on both sides) */
            bool isf = pk.size == 5;
            if (isf) pk.size = 4;
            for (char* t = strtok(val, ","); ok && t; t = strtok(nullptr, ",")) {
                if (isf) { float f = strtof(t, &e); ok = *e == 0; uint32_t b; memcpy(&b, &f, 4); pk.values.push_back(b); }
                else { pk.values.push_back((uint32_t)strtoul(t, &e, 0)); ok = *e == 0; }
            }
            ok = ok && !pk.values.empty();
            if (!ok) { fprintf(stderr, "invalid poke directive (FUNC|* ADDR|rN+OFF u8|u16|u32|f32 VALUE): %s", line); exit(2); }
            g_poke_specs.push_back(pk);
            continue;
        }
        if (!strcmp(kw, "setup") || !strcmp(kw, "postsetup")) { /* (post)setup FUNC|* ADDR [rN=VALUE ...] */
            char sfn[32], tgt[32], rest[512] = "";
            int m = sscanf(line, "%*s %31s %31s %511[^\n]", sfn, tgt, rest);
            char* e1; char* e2;
            SetupSpec su{};
            su.func = strcmp(sfn, "*") ? (uint32_t)strtoul(sfn, &e1, 16) : 0;
            su.target = (uint32_t)strtoul(tgt, &e2, 16);
            bool ok = m >= 2 && (!strcmp(sfn, "*") || (*e1 == 0 && strlen(sfn) == 8)) && *e2 == 0 && strlen(tgt) == 8;
            for (char* t = strtok(rest, " \t\r"); ok && t; t = strtok(nullptr, " \t\r")) {
                unsigned reg; char val[64];
                if (t[0] == '-' && t[1] == '>') { /* ->ADDR: store the returned r3 there (0 exits 2) */
                    char* e4;
                    su.store_addr = (uint32_t)strtoul(t + 2, &e4, 16);
                    if (su.store || !t[2] || *e4 || (su.store_addr & 3)) { ok = false; break; }
                    su.store = true;
                    continue;
                }
                if (!strncmp(t, "sp+", 3)) { /* sp+OFF=VALUE: the word at the routine's entry SP + OFF */
                    char* e4; char* e5;
                    SetupStack st{(uint32_t)strtoul(t + 3, &e4, 0), 0};
                    if (*e4 != '=' || !e4[1] || st.off < 8 || st.off > 0x100 || (st.off & 3)) { ok = false; break; }
                    st.value = (uint32_t)strtoul(e4 + 1, &e5, 0);
                    if (*e5) { ok = false; break; }
                    su.stack.push_back(st);
                    continue;
                }
                if (t[0] == 'f') { /* fN=FLOAT or fN=*ADDR (an f32 word read when the line runs), N = 1..13 */
                    if (sscanf(t, "f%u=%63s", &reg, val) != 2 || reg < 1 || reg > 13) { ok = false; break; }
                    SetupReg sr{(int)reg, 0, 0, 0, true};
                    char* e3;
                    if (val[0] == '*') {
                        sr.deref = 1;
                        sr.value = (uint32_t)strtoul(val + 1, &e3, 16);
                    } else {
                        float fv = strtof(val, &e3);
                        memcpy(&sr.value, &fv, 4);
                    }
                    if (*e3 || e3 == val) { ok = false; break; }
                    su.regs.push_back(sr);
                    continue;
                }
                if (sscanf(t, "r%u=%63s", &reg, val) != 2 || reg < 3 || reg > 10) { ok = false; break; }
                SetupReg sr{(int)reg, 0, 0, 0, false};
                char* e3;
                if (val[0] == '*' && val[1] == '(' && val[2] == '*') { /* *(*ADDR+OFF) */
                    sr.deref = 2;
                    sr.value = (uint32_t)strtoul(val + 3, &e3, 16);
                    if (*e3 != '+' && *e3 != ')') { ok = false; break; }
                    if (*e3 == '+') sr.off = (uint32_t)strtoul(e3 + 1, &e3, 0);
                    if (strcmp(e3, ")")) { ok = false; break; }
                } else if (val[0] == '*') { /* *ADDR */
                    sr.deref = 1;
                    sr.value = (uint32_t)strtoul(val + 1, &e3, 16);
                    if (*e3) { ok = false; break; }
                } else {
                    sr.value = (uint32_t)strtoul(val, &e3, 0);
                    if (*e3) { ok = false; break; }
                }
                su.regs.push_back(sr);
            }
            if (!ok) { fprintf(stderr, "invalid %s directive (FUNC|* ADDR [rN=VALUE|rN=*ADDR|rN=*(*ADDR+OFF) ...] [fN=FLOAT|fN=*ADDR ...] [sp+OFF=VALUE ...] [->ADDR]): %s", kw, line); exit(2); }
            (strcmp(kw, "setup") ? g_postsetup_specs : g_setup_specs).push_back(su);
            continue;
        }
        if (!strcmp(kw, "retarg")) { /* retarg TARGET rN: the mock returns its incoming rN */
            unsigned target, reg; char extra[8];
            if (sscanf(line, "%*s %x r%u %7s", &target, &reg, extra) != 2 || reg < 3 || reg > 10) {
                fprintf(stderr, "invalid retarg directive (TARGET r3..r10): %s", line); exit(2);
            }
            g_retarg[target] = (int)reg;
            continue;
        }
        if (!strcmp(kw, "allocnested")) { /* allocnested TARGET: after its alloc/allocfixed line */
            unsigned target; char extra[8];
            if (sscanf(line, "%*s %x %7s", &target, extra) != 1 || !g_alloc_specs.count(target)) {
                fprintf(stderr, "invalid allocnested directive (TARGET, after its alloc/allocfixed line): %s", line); exit(2);
            }
            g_alloc_nested.insert(target);
            continue;
        }
        if (!strcmp(kw, "allocfixed")) { /* allocfixed TARGET SIZE BASE [ALIGN]: a factory whose request size is fixed */
            unsigned target, size, base, align = 32;
            char extra[8];
            int m = sscanf(line, "%*s %x %i %x %u %7s", &target, (int*)&size, &base, &align, extra);
            if (m < 3 || m > 4 || size == 0 || size > 0x1000000u || align == 0 || (align & (align - 1))) {
                fprintf(stderr, "invalid allocfixed directive: %s", line); exit(2);
            }
            g_alloc_specs[target] = {0, base, align, size, 0};
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
        if (!strcmp(kw, "observeheap")) { /* observeheap ADDR: a sead ExpHeap object whose blocks -observe-pointees bounds */
            unsigned ha;
            if (sscanf(line, "%*s %x", &ha) != 1) { fprintf(stderr, "invalid observeheap directive (ADDR): %s", line); exit(2); }
            if (std::find(g_obs_heaps.begin(), g_obs_heaps.end(), ha) == g_obs_heaps.end()) g_obs_heaps.push_back(ha);
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
            if (m < 1) { fprintf(stderr, "invalid %s directive (ADDR LO [HI] or ADDR v1,v2,...): %s", kw, line); exit(2); }
            uint32_t fa = (uint32_t)spec_uint(fn, 16, kw, "callee address");
            if (kw[3] == '!') g_ret_strict.insert(fa);
            if (m < 2) { fprintf(stderr, "invalid %s directive (ADDR LO [HI] or ADDR v1,v2,...): %s", kw, line); exit(2); }
            if (strchr(sa, ',')) { /* ret ADDR v1,v2,...: one of these values */
                if (m == 3) { fprintf(stderr, "invalid %s directive (a pick list takes no HI): %s", kw, line); exit(2); }
                for (char* t = strtok(sa, ","); t; t = strtok(nullptr, ",")) g_ret_picks[fa].push_back(spec_int(t, kw));
            } else {
                /* ret ADDR V (one value, = V V; ignored before 2026-10-11) or ret ADDR LO HI */
                g_ret_specs[fa] = {spec_int(sa, kw), spec_int(m == 3 ? sb : sa, kw)};
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
        if (!strcmp(kw, "fretf64") || !strcmp(kw, "fretf64!")) {
            /* fretf64[!] ADDR v1[,v2,...]: a mocked callee's f1 result is one of these exact doubles (ps0 = ps1,
             * never narrowed); v as in argf64 (decimal, or 0x + 16 hex digits of raw IEEE-754 bits). Without `!`
             * 1 call in 8 keeps the generated result. */
            static char sa[16384]; char extra[8];
            int m = sscanf(line, "%*s %31s %16383s %7s", fn, sa, extra);
            char* fe; uint32_t fa = (uint32_t)strtoul(fn, &fe, 16);
            if (m != 2 || *fe || fn[0] == '*') { fprintf(stderr, "invalid %s directive (ADDR v1[,v2,...]): %s", kw, line); exit(2); }
            std::vector<double> vals;
            for (char* t = strtok(sa, ","); t; t = strtok(nullptr, ",")) {
                double v; char* e;
                if ((t[0] == '0' && (t[1] == 'x' || t[1] == 'X')) && strlen(t) == 18) {
                    uint64_t bits = strtoull(t + 2, &e, 16);
                    if (*e) { fprintf(stderr, "invalid %s raw value %s: %s", kw, t, line); exit(2); }
                    memcpy(&v, &bits, 8);
                } else {
                    v = strtod(t, &e);
                    if (e == t || *e || (t[0] == '0' && (t[1] == 'x' || t[1] == 'X'))) {
                        fprintf(stderr, "invalid %s value %s (decimal, or 0x + 16 hex digits): %s", kw, t, line); exit(2);
                    }
                }
                vals.push_back(v);
            }
            if (vals.empty()) { fprintf(stderr, "invalid %s directive (no values): %s", kw, line); exit(2); }
            g_fret64_picks[fa] = vals;
            if (kw[7] == '!') g_fret64_strict.insert(fa);
            continue;
        }
        if (!strcmp(kw, "argf64") || !strcmp(kw, "argf64!")) {
            /* argf64[!] FUNC|* fN v1[,v2,...]: exact double inputs of an FPR (ps0 = ps1 = v, never narrowed to
             * single). Each v is a decimal double (strtod, e.g. 1.000000000931322574615478515625) or 0x + 16 hex
             * digits of raw IEEE-754 bits (e.g. 0x7FF0000000000001, a signalling NaN). Drawn like an arg pick list. */
            static char sa[16384]; char extra[8];
            int m = sscanf(line, "%*s %31s %63s %16383s %7s", fn, where, sa, extra);
            if (m != 3 || where[0] != 'f' || !where[1]) { fprintf(stderr, "invalid %s directive (FUNC fN v1[,v2,...]): %s", kw, line); exit(2); }
            char* rend; long fr = strtol(where + 1, &rend, 10);
            if (*rend || fr < 0 || fr > 31) { fprintf(stderr, "invalid %s register (f0..f31): %s", kw, line); exit(2); }
            ArgSpec a{};
            a.strict = kw[6] == '!';
            a.exact = true;
            a.is_float = true;
            a.reg = (int)fr;
            a.func = strcmp(fn, "*") ? (uint32_t)strtoul(fn, nullptr, 16) : 0;
            for (char* t = strtok(sa, ","); t; t = strtok(nullptr, ",")) {
                double v; char* e;
                if ((t[0] == '0' && (t[1] == 'x' || t[1] == 'X')) && strlen(t) == 18) {
                    uint64_t bits = strtoull(t + 2, &e, 16);
                    if (*e) { fprintf(stderr, "invalid %s raw value %s: %s", kw, t, line); exit(2); }
                    memcpy(&v, &bits, 8);
                } else {
                    v = strtod(t, &e);
                    if (e == t || *e || (t[0] == '0' && (t[1] == 'x' || t[1] == 'X'))) {
                        fprintf(stderr, "invalid %s value %s (decimal, or 0x + 16 hex digits): %s", kw, t, line); exit(2);
                    }
                }
                a.pick.push_back(v);
            }
            if (a.pick.empty()) { fprintf(stderr, "invalid %s directive (no values): %s", kw, line); exit(2); }
            g_arg_specs.push_back(a);
            continue;
        }
        if ((!strcmp(kw, "arg") || !strcmp(kw, "arg!")) && n >= 4) { /* arg[!] FUNC rN|fN LO [HI]  or  v1,v2,... */
            ArgSpec a{};
            a.strict = kw[3] == '!'; /* arg!: always applied (no unsteered inputs) */
            a.func = spec_func(fn, kw);
            a.is_float = where[0] == 'f';
            if ((where[0] != 'r' && where[0] != 'f') || !where[1]) spec_bad_token(kw, where, "register");
            a.reg = (int)spec_uint(where + 1, 10, kw, "register");
            static char sa[16384]; char sb[64];
            int m = sscanf(line, "%*s %*s %*s %16383s %63s", sa, sb);
            if (strchr(sa, ',')) {
                for (char* t = strtok(sa, ","); t; t = strtok(nullptr, ","))
                    a.pick.push_back(spec_num(t, a.is_float, kw));
            } else {
                a.lo = spec_num(sa, a.is_float, kw);
                a.hi = m >= 2 ? spec_num(sb, a.is_float, kw) : a.lo;
            }
            g_arg_specs.push_back(a);
            continue;
        }
        if (strcmp(kw, "field")) continue;
        if (n < 4) { fprintf(stderr, "%s:%d: invalid field directive (FUNC|* ADDR|rN+OFF TYPE LO [HI] | v1,v2,...): %s",
                             g_spec_path, g_spec_lineno, g_spec_line); exit(2); }
        if (n < 5) { /* the value did not even start like a number (e.g. `*0x...`): ignored before 2026-10-27 */
            char v0[64] = "";
            if (sscanf(line, "%*s %*s %*s %*s %63s", v0) != 1) {
                fprintf(stderr, "%s:%d: invalid field directive (missing FUNC|* or value: FUNC|* ADDR|rN+OFF TYPE LO [HI] | "
                        "v1,v2,...): %s", g_spec_path, g_spec_lineno, g_spec_line);
                exit(2);
            }
            spec_bad_token(kw, v0, "value");
        }
        if (n == 5) hi = lo;
        FieldSpec s{};
        s.func = spec_func(fn, kw);
        if (where[0] == 'r') {
            char* plus;
            s.base_reg = (int)strtol(where + 1, &plus, 10);
            if (plus == where + 1 || (*plus && *plus != '+')) spec_bad_token(kw, where, "address");
            s.off = *plus == '+' ? (uint32_t)spec_uint(plus + 1, 0, kw, "offset") : 0;
        } else {
            s.base_reg = -1;
            s.off = (uint32_t)spec_uint(where, 16, kw, "address");
        }
        s.is_float = ty[0] == 'f';
        s.size = atoi(ty + 1) / 8;
        s.lo = lo;
        s.hi = hi;
        {
            static char sv[16384];
            if (sscanf(line, "%*s %*s %*s %*s %16383s", sv) == 1 && strchr(sv, ',')) {
                for (char* t = strtok(sv, ","); t; t = strtok(nullptr, ","))
                    s.pick.push_back(spec_num(t, s.is_float, kw));
            } else if (sscanf(line, "%*s %*s %*s %*s %16383s", sv) == 1) {
                s.lo = spec_num(sv, s.is_float, kw);
                char sh[64];
                s.hi = sscanf(line, "%*s %*s %*s %*s %*s %63s", sh) == 1 ? spec_num(sh, s.is_float, kw) : s.lo;
            }
        }
        g_specs.push_back(s);
    }
    fclose(f);
    if (osm::pad_sched_seen && !(osm::parts & osm::P_PAD)) {
        fprintf(stderr, "%s: padsched needs `osmem ... pad`\n", g_spec_path);
        exit(2);
    }
    if (osm::parts) { /* `osmem`: its targets take no other mock directive; then its initial words */
        std::set<uint32_t> other;
        for (auto& kv : g_ret_specs) other.insert(kv.first);
        for (auto& kv : g_ret_picks) other.insert(kv.first);
        for (auto& kv : g_alloc_specs) other.insert(kv.first);
        for (auto& kv : g_nestedret) other.insert(kv.first);
        for (auto& kv : g_retarg) other.insert(kv.first);
        for (auto& kv : g_set_specs) other.insert(kv.first);
        for (auto& kv : g_zero_specs) other.insert(kv.first);
        for (auto& kv : g_move_specs) other.insert(kv.first);
        for (auto& kv : g_lookup_specs) other.insert(kv.first);
        for (auto& kv : g_tls_role) other.insert(kv.first);
        for (auto& kv : g_retfield) other.insert(kv.first);
        for (auto& kv : g_retain_specs) other.insert(kv.first);
        for (auto& kv : g_utf16_specs) other.insert(kv.first);
        for (auto& kv : g_mockfield) other.insert(kv.first);
        for (uint32_t a : g_fetch_size) other.insert(a);
        for (uint32_t a : g_fetch_init) other.insert(a);
        for (uint32_t a : {g_dl_begin, g_dl_end, g_dl_getcur}) if (a) other.insert(a);
        for (uint32_t a : other)
            if ((osm::models(a) || ((osm::parts & osm::P_DYNLOAD) && dlm::host_range(a))) && a != osm::S_SURFACE) {
                fprintf(stderr, "%s: osmem models %08X; remove its other mock directive\n", g_spec_path, a);
                exit(2);
            }
        if (osm::parts & osm::P_GX2) g_surface_size.insert(osm::S_SURFACE); /* GX2CalcSurfaceSizeAndAlignment: the mocksurfacesize model */
        if (osm::parts & osm::P_AX) { /* the fixed `ax` host region must not overlap another osmem region or a fixture line */
            const uint64_t lo = gthr::kAxHostLo, hi = gthr::kAxHostHi;
            struct R { const char* what; uint64_t a, b; };
            std::vector<R> rs = {{"the osmem runtime bump region", osm::kBumpBase, osm::kBumpBase + 0xE0000u},
                                 {"the osmem sysinfo block", osm::sysinfo, (uint64_t)osm::sysinfo + 0x20},
                                 {"the main thread's OSThread block (mainthread=)", gthr::main_obj, (uint64_t)gthr::main_obj + gthrm::kSize},
                                 {"the alarm service thread's block and stack", 0x6FFE1800u, 0x6FFFF000u},
                                 {"the main thread's stack window", gthr::kMainStackTop - 0x10000u, gthr::kMainStackTop + 8},
                                 {"the default heap", osm::default_start(), osm::kMem2End},
                                 {"the MEM1 heap", osm::kMem1, (uint64_t)osm::kMem1 + osm::kMem1Size},
                                 {"the foreground heap", osm::kFg, (uint64_t)osm::kFg + osm::kFgSize},
                                 {"the .bss", osm::bss_start, osm::data_end},
                                 {"the data-import slots", 0xC1000000u, 0xC100A000u}};
            for (const FillSpec& f : g_fill_specs)
                if (f.base_reg < 0) rs.push_back({"a `fill` line", f.off, (uint64_t)f.off + f.size});
            for (const CopySpec& c : g_copy_specs)
                if (c.base_reg < 0) rs.push_back({"a `copy`/`blob`/`blobfile` line", c.off, (uint64_t)c.off + c.size});
            for (const R& r : rs)
                if (r.a < hi && lo < r.b) {
                    fprintf(stderr, "%s: osmem ax: its fixed host region %08X..%08X (GamePad aux buffers, AX frame thread) overlaps %s "
                            "(%08llX..%08llX)\n", g_spec_path, gthr::kAxHostLo, gthr::kAxHostHi, r.what, (unsigned long long)r.a,
                            (unsigned long long)r.b);
                    exit(2);
                }
        }
        osm::build_init();
    }
    if ((!capm::map.empty() || capm::call_func) && !capm::present) { fprintf(stderr, "%s: capturemap without a capture line\n", g_spec_path); exit(2); }
    if (capm::present) {
        const char* top = !g_setup_specs.empty() ? "setup" : !g_postsetup_specs.empty() ? "postsetup" : !g_poke_specs.empty() ? "poke"
                          : !g_specs.empty() ? "field" : !g_str_specs.empty() ? "fieldstr" : !g_fill_specs.empty() ? "fill"
                          : !g_copy_specs.empty() ? "copy/blob" : nullptr;
        if (top) { fprintf(stderr, "%s: a capture spec sets nothing by hand (found `%s`)\n", g_spec_path, top); exit(2); }
        if (capm::map.empty()) { fprintf(stderr, "%s: capture without capturemap\n", g_spec_path); exit(2); }
        if (!getenv("WWHD_CAPTURE_READLOG") && !capm::call_func) { fprintf(stderr, "%s: capture without capturecall\n", g_spec_path); exit(2); }
        if (capm::call_func) {
            int n = 0;
            for (const ArgSpec& a : g_arg_specs) {
                if (a.is_float || !a.strict || !a.pick.empty() || a.func != capm::call_func) { fprintf(stderr, "%s: capturecall: every arg line must be an exact arg! of %08X\n", g_spec_path, capm::call_func); exit(2); }
                auto it = capm::call_regs.find(a.reg);
                if (it == capm::call_regs.end() || a.lo != a.hi || (uint32_t)(int64_t)a.lo != it->second) {
                    fprintf(stderr, "%s: capturecall: arg! r%d differs from the captured register %08X (drift)\n", g_spec_path, a.reg, it == capm::call_regs.end() ? 0 : it->second); exit(2);
                }
                n++;
            }
            if (!n) { fprintf(stderr, "%s: capturecall without arg! lines\n", g_spec_path); exit(2); }
        }
        /* keep only the pages a window touches */
        std::vector<uint8_t> kept;
        std::vector<uint32_t> want;
        for (const auto& kv : capm::pages)
            for (const auto& w : capm::map)
                if (kv.first < w.second && w.first < kv.first + 0x1000) { want.push_back(kv.first); break; }
        kept.reserve(want.size() * 0x1000);
        std::unordered_map<uint32_t, const uint8_t*> np;
        for (uint32_t pg : want) { size_t o = kept.size(); kept.insert(kept.end(), capm::pages[pg], capm::pages[pg] + 0x1000); np[pg] = kept.data() + o; }
        capm::file.swap(kept);
        capm::pages.swap(np);
        std::vector<uint8_t>().swap(kept);
    }
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
    NetMap rec_net; /* recorded net writes outside the stack (replay check) */
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
            if (s.exact) c->f[s.reg].ps0 = c->f[s.reg].ps1 = v;
            else if (s.is_float) c->f[s.reg].ps0 = c->f[s.reg].ps1 = (double)(float)v;
            else c->r[s.reg] = (uint32_t)(int64_t)v;
        }
    }
    for (const CrSpec& s : g_cr_specs) {
        if (s.func && s.func != of->addr) continue;
        c->cr[s.bit] = s.pick[hash3(seed, 200 + (uint64_t)s.bit, 0xC5B17) % s.pick.size()];
    }
    /* -fastreset: the leading fill/copy lines (fills, then copies, in file order) with absolute addresses are the same in
     * every input of the function: they go into g_snap_base once (the later lines and the fields stay per input in V.snap,
     * which initial_byte consults first, so they win as before) */
    size_t nbase = 0;
    if (g_fastreset) {
        static std::unordered_map<uint32_t, SnapPage*> base;
        static uint32_t base_func = 0;
        static bool base_ok = false;
        bool open = true;
        for (const FillSpec& s : g_fill_specs)
            if (!(s.func && s.func != of->addr) && open) { if (s.base_reg >= 0) open = false; else nbase++; }
        for (const CopySpec& s : g_copy_specs)
            if (!(s.func && s.func != of->addr) && open) { if (s.base_reg >= 0) open = false; else nbase++; }
        if (!base_ok || base_func != of->addr) {
            for (auto& kv : base) snap_free(kv.second);
            base.clear();
            size_t k = 0;
            for (const FillSpec& s : g_fill_specs)
                if (!(s.func && s.func != of->addr) && k++ < nbase) snap_range(s.off, s.size, nullptr, s.byte, base);
            for (const CopySpec& s : g_copy_specs)
                if (!(s.func && s.func != of->addr) && k++ < nbase) snap_range(s.off, s.size, s.data.data(), 0, base);
            base_func = of->addr;
            base_ok = true;
        }
        g_snap_base = nbase ? &base : nullptr;
    }
    size_t kfc = 0;
    for (const FillSpec& s : g_fill_specs) {
        if (s.func && s.func != of->addr) continue;
        if (kfc++ < nbase) continue; /* in g_snap_base */
        uint32_t a = (s.base_reg >= 0 ? c->r[s.base_reg] : 0) + s.off;
        snap_range(a, s.size, nullptr, s.byte);
    }
    for (const CopySpec& s : g_copy_specs) {
        if (s.func && s.func != of->addr) continue;
        if (kfc++ < nbase) continue;
        uint32_t a = (s.base_reg >= 0 ? c->r[s.base_reg] : 0) + s.off;
        snap_range(a, s.size, s.data.data(), 0);
    }
    for (auto& run : osm::init_runs) snap_range(run.first, (uint32_t)run.second.size(), run.second.data(), 0); /* `osmem` */
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
    std::map<uint32_t, uint8_t> rec_net;
    for (auto& w : writes) {
        uint32_t a = w.first;
        if (a - (sp - 0x10000) < 0x10000 + 8) continue;
        uint8_t fin = shadow[a];
        bool known = snap_has(a) || image_region(a); /* same rule as net_writes() */
        uint8_t init = 0;
        if (known) init = initial_byte(a, &known);
        if (!known || init != fin) rec_net[a] = fin;
    }
    for (auto& kv : rec_net) in->rec_net.push(kv.first, kv.second);
    return ok;
}

/* ------------------------------------------------------------------ runs and comparison */
static bool g_strict_nan = false; /* -strictnan: NaN payloads compared bit for bit (no float-mark tolerance in same_net) */
struct Result {
    uint32_t entry_sp = 0;
    Cpu exit;
    BigVec<CallEv> calls;
    NetMap net;
    /* float marks at exit, ascending and unique (2026-10-26, AGL#18 perf: were std::sets built node by node from the Page
     * marks of every run, the dominant cost when setups leave many marks; same_net reads them only on a net difference
     * without -strictnan) */
    std::vector<uint32_t> dwords; /* complete explicit f64 store starts */
    std::vector<uint32_t> fwords; /* words whose last store was a float store */
    std::vector<uint8_t> final_bytes; /* `final` ranges at exit */
    bool aborted = false;
    bool stopped = false; /* `stopat`: ended at the terminal call */
    bool dsi = false;     /* -nulltrap: ended by a page-0 access (console DSI) */
    bool p0 = false;      /* -p0exclude: this (original) run touched page 0 */
    /* the run was ended by a mock model's domain check (model_reject; `aborted` is also set, `why` names the model):
     * the rejected call's target, nesting depth (0 = a top-level call, the last of `calls`) and innermost `real` callee */
    bool mrej = false;
    uint32_t mrej_target = 0, mrej_in = 0;
    int mrej_depth = 0;
    char dsi_kind = 0;
    uint32_t dsi_ea = 0, dsi_in = 0;
    int dsi_depth = 0;
    std::vector<uint32_t> badfetch; /* -nulltrap, original side: non-code indirect call targets in call order (fetch_check) */
    std::string why;
    uint32_t stray = 0, first_stray = 0;
    int traps = 0;
    /* `osmem ... threads`: the run's guest-thread summary (schedule digest, per-thread call logs, deadlock) and the
     * thread that took a -nulltrap fault (0 = main) */
    gthr::RunInfo thr;
    int dsi_thread = 0;
};

/* ------------------------------------------------------------------ setupsnapshot */
/* `setupsnapshot` (unit file, 2026-10-26, AGL#3: an original ShaderMgr archive setup chain costs >60 s CPU per run and
 * side). The `setup` lines (not pokes, not postsetups) of a function run once, at the start of its inputs, on a clean
 * pre-input state; the complete state they leave is captured, and each input installs it (both sides alike) in place
 * of running them. Everything after (pokes, postsetups, the baseline rebase, the function) runs as before.
 *
 * What a setup run can change, and so what is captured:
 *   - guest memory: every materialized byte (have bit) with its current and initial value, the per-byte write bits and
 *     the f32/f64 float-store marks (Page fw/dw, V.fany), outside the scratch stack window [stack_lo, entry SP + 8)
 *     (2026-10-26, AGL#18: a setup callee that spills or copies uninitialised frame slots leaves generated/`stackfill`
 *     bytes on the setups' own stack 0x8000 below the entry SP, which differ per input and refused the snapshot).
 *     That window is scratch, not baseline state (the net effect excludes it too): it is neither captured, installed
 *     nor compared, so each input materializes it from its own generation as before. Chunk clobber epochs need nothing: setups run at nested depth, where no mocked call opens an epoch, so
 *     V.epoch stays 0 and every captured byte is current at epoch 0 (V.epoch is captured and compared anyway);
 *   - the steered allocators: g_alloc_cursor (alloc/allocfixed/allocnested blocks) and g_alloc_calls (allocfail
 *     call indices);
 *   - V.per_target: nested mocked calls inside the setups advance the per-target counters that later mocks hash;
 *   - the run budgets the setups consumed (V.budget accesses, V.ncalls guest calls), so an input aborts exactly where
 *     it did with the setups run for real (only the CPU-time budget is not charged: that is the point);
 *   - V.traps (trap instructions inside the setups count like the function's own);
 *   - (not V.orig_stack: the stack guard records only bytes inside the excluded scratch window);
 *   - g_retained (written only by top-level mocks, so unchanged; captured and compared for completeness), and the
 *     original-side semantic-effect statistics (mockset/mockzero/mocksurfacesize counts on stderr).
 * Host-side state the setups do not touch: V.calls (nested calls are not recorded), fcopy/dict/coverage/locals
 * (depth 0 only), the mocklookup/mockset/nestedret/retarg tables (read-only specs).
 *
 * The setups normally run after the input's generated memory and registers are in place, so their result may depend
 * on the input. Before the inputs they run twice under two different input seeds (the full per-input path); if the two
 * states differ in any materialized byte value, float mark, counter or cursor, the snapshot is refused and the
 * function keeps per-input setups. Input values are only the generated bytes the setups READ: the initial value of a
 * byte the setups only write is per input but never matters (the post-setup rebase replaces every initial value), so
 * it is not compared. An allocfail list that can fail a call inside the setups is refused as well. Then the first
 * input of the run and every 500th input run the setups for real (both sides) and compare them with the snapshot; a
 * mismatch exits 2. */
struct SetupPage {
    uint32_t pn;
    uint64_t have[64], wr[64], fw[64], dw[64];
    std::vector<std::pair<uint32_t, Chunk>> ch; /* chunks holding a materialized byte (ep unused) */
};
struct SetupState {
    std::vector<SetupPage> pages; /* sorted by page number */
    size_t nbytes = 0;
    bool fany = false;
    std::unordered_map<uint32_t, uint32_t> alloc_cursor, alloc_calls, per_target, retained;
    std::vector<uint32_t> osmem; /* `osmem`: the heap bookkeeping (osm::save) */
    uint64_t budget = 0, ncalls = 0;
    uint32_t epoch = 0;
    int traps = 0;
    std::vector<uint32_t> orig_stack; /* sorted */
    uint64_t set_calls = 0, set_bytes = 0, surface_calls = 0; /* original-side statistics added by the setups */
    std::string thr_live; /* `osmem ... threads`: why the state cannot be a snapshot (guest-thread state left by the setups) */
};
enum SnapMode { SNAP_OFF, SNAP_CAPTURE, SNAP_USE, SNAP_VALIDATE };
static SnapMode g_snap_mode = SNAP_OFF;
static SetupState g_snap;            /* the installed snapshot (SNAP_USE / SNAP_VALIDATE) */
static SetupState* g_snap_out;       /* SNAP_CAPTURE: where the run stores its post-setup state */
static long g_snap_input = -1;       /* SNAP_VALIDATE: the input index, for the error */

static void setup_state_capture(SetupState* st, uint64_t sc0, uint64_t sb0, uint64_t su0) {
    st->pages.clear();
    st->nbytes = 0;
    std::vector<uint32_t> pns;
    for (auto& kv : V.pages) pns.push_back(kv.first);
    std::sort(pns.begin(), pns.end());
    uint64_t win_lo = V.stack_lo, win_hi = (uint64_t)V.entry_sp + 8; /* scratch stack window, excluded */
    for (uint32_t pn : pns) {
        Page* p = V.pages[pn];
        SetupPage sp;
        sp.pn = pn;
        memcpy(sp.have, p->have, sizeof sp.have);
        memcpy(sp.wr, p->wr, sizeof sp.wr);
        memcpy(sp.fw, pfw(p), sizeof sp.fw);
        memcpy(sp.dw, pdw(p), sizeof sp.dw);
        uint64_t lo = std::max<uint64_t>((uint64_t)pn << 12, win_lo), hi = std::min<uint64_t>(((uint64_t)pn + 1) << 12, win_hi);
        for (uint64_t a = lo; a < hi; a++) { /* clear the window's bits on this page */
            uint32_t o = (uint32_t)(a & 0xFFF);
            uint64_t m = ~(1ull << (o & 63));
            sp.have[o >> 6] &= m; sp.wr[o >> 6] &= m; sp.fw[o >> 6] &= m; sp.dw[o >> 6] &= m;
        }
        bool any = false;
        for (int w = 0; w < 64 && !any; w++) any = sp.have[w] || sp.fw[w] || sp.dw[w];
        if (!any) continue;
        for (uint32_t w = 0; w < 64; w++)
            if (sp.have[w]) {
                Chunk k = *p->ch[w];
                k.ep = nullptr;
                sp.ch.emplace_back(w, k);
                st->nbytes += __builtin_popcountll(sp.have[w]);
            }
        st->pages.push_back(std::move(sp));
    }
    st->fany = V.fany;
    st->alloc_cursor = g_alloc_cursor;
    st->alloc_calls = g_alloc_calls;
    st->per_target = V.per_target;
    st->retained = g_retained;
    st->osmem = osm::save();
    st->budget = V.budget;
    st->ncalls = V.ncalls;
    st->epoch = V.epoch;
    st->traps = V.traps;
    st->orig_stack.clear(); /* stack-guard entries outside the window (none today: the guard records only window bytes) */
    for (uint32_t a : V.orig_stack)
        if (a < win_lo || a >= win_hi) st->orig_stack.push_back(a);
    std::sort(st->orig_stack.begin(), st->orig_stack.end());
    st->thr_live = gthr::live_reason();
    st->set_calls = g_set_calls - sc0;
    st->set_bytes = g_set_bytes - sb0;
    st->surface_calls = g_surface_calls - su0;
}

/* first difference between two post-setup states ("" when equal): the lowest differing guest address (materialized
 * or not, current value, float mark), else the first differing counter */
static std::string setup_state_diff(const SetupState& a, const SetupState& b, std::string* detail = nullptr) {
    char buf[96];
    size_t i = 0, j = 0;
    static const SetupPage none = [] { SetupPage p{}; return p; }();
    auto chunk = [](const SetupPage& p, uint32_t w) -> const Chunk* {
        for (auto& c : p.ch) if (c.first == w) return &c.second;
        return nullptr;
    };
    while (i < a.pages.size() || j < b.pages.size()) {
        uint32_t pa = i < a.pages.size() ? a.pages[i].pn : UINT32_MAX, pb = j < b.pages.size() ? b.pages[j].pn : UINT32_MAX;
        uint32_t pn = std::min(pa, pb);
        const SetupPage& x = pa == pn ? a.pages[i] : none;
        const SetupPage& y = pb == pn ? b.pages[j] : none;
        for (uint32_t w = 0; w < 64; w++) {
            uint64_t d = (x.have[w] ^ y.have[w]) | (x.fw[w] ^ y.fw[w]) | (x.dw[w] ^ y.dw[w]);
            uint64_t both = x.have[w] & y.have[w];
            if (both) {
                const Chunk* cx = chunk(x, w);
                const Chunk* cy = chunk(y, w);
                for (uint64_t m = both; m; m &= m - 1) {
                    int bit = __builtin_ctzll(m);
                    if (cx->d[bit] != cy->d[bit]) d |= 1ull << bit;
                }
            }
            if (d) {
                int bit = __builtin_ctzll(d);
                if (detail) { /* -v: what each state holds there */
                    auto desc = [&](const SetupPage& p) -> std::string {
                        if (!(p.have[w] >> bit & 1)) return "not materialized";
                        char t[48];
                        snprintf(t, sizeof t, "%02X%s%s", chunk(p, w)->d[bit], p.fw[w] >> bit & 1 ? " f32-mark" : "",
                                 p.dw[w] >> bit & 1 ? " f64-mark" : "");
                        return t;
                    };
                    *detail = desc(x) + " vs " + desc(y);
                }
                snprintf(buf, sizeof buf, "%08X", pn << 12 | w << 6 | bit);
                return buf;
            }
        }
        if (pa == pn) i++;
        if (pb == pn) j++;
    }
    auto map_diff = [&](const std::unordered_map<uint32_t, uint32_t>& x, const std::unordered_map<uint32_t, uint32_t>& y,
                        const char* what) -> std::string {
        if (x == y) return "";
        uint32_t lo = UINT32_MAX;
        for (auto& kv : x) { auto it = y.find(kv.first); if (it == y.end() || it->second != kv.second) lo = std::min(lo, kv.first); }
        for (auto& kv : y) { auto it = x.find(kv.first); if (it == x.end() || it->second != kv.second) lo = std::min(lo, kv.first); }
        snprintf(buf, sizeof buf, "%s %08X", what, lo);
        return buf;
    };
    std::string r;
    if (!(r = map_diff(a.alloc_cursor, b.alloc_cursor, "the alloc cursor of")).empty()) return r;
    if (!(r = map_diff(a.alloc_calls, b.alloc_calls, "the allocfail call count of")).empty()) return r;
    if (!(r = map_diff(a.per_target, b.per_target, "the nested call count of")).empty()) return r;
    if (!(r = map_diff(a.retained, b.retained, "the mockretain pointer of")).empty()) return r;
    if (a.osmem != b.osmem) return "the osmem heap state";
    if (a.fany != b.fany) return "the float-mark flag";
    if (a.epoch != b.epoch) return "the clobber epoch";
    if (a.budget != b.budget) return "the access count (a different path through the setups)";
    if (a.ncalls != b.ncalls) return "the guest call count";
    if (a.traps != b.traps) return "the trap count";
    if (a.orig_stack != b.orig_stack) return "the setup stack writes";
    if (a.set_calls != b.set_calls || a.set_bytes != b.set_bytes || a.surface_calls != b.surface_calls)
        return "the mockset/mocksurfacesize statistics";
    return "";
}

static void setup_state_install(const SetupState& st, bool memory = true) {
    for (const SetupPage& sp : st.pages) {
        if (!memory) break; /* -fastreset: the template pages already hold it */
        Page* p = page(sp.pn);
        memcpy(p->have, sp.have, sizeof p->have);
        memcpy(p->wr, sp.wr, sizeof p->wr);
        page_set_marks(p, sp.fw, sp.dw);
        for (auto& c : sp.ch) {
            Chunk*& k = p->ch[c.first];
            if (!k) k = (Chunk*)arena_zalloc(sizeof(Chunk));
            memcpy(k->d, c.second.d, sizeof k->d);
            memcpy(k->init, c.second.init, sizeof k->init);
            ep_zero(k); /* current at epoch 0, as after a real setup run */
        }
    }
    V.fany = st.fany;
    g_alloc_cursor = st.alloc_cursor;
    g_alloc_calls = st.alloc_calls;
    V.per_target = st.per_target;
    g_retained = st.retained;
    osm::load(st.osmem);
    V.budget = st.budget;
    V.ncalls = st.ncalls;
    V.epoch = st.epoch;
    V.traps = st.traps;
    if (!V.is_candidate) {
        V.orig_stack.insert(st.orig_stack.begin(), st.orig_stack.end());
        g_set_calls += st.set_calls;
        g_set_bytes += st.set_bytes;
        g_surface_calls += st.surface_calls;
    }
}

/* -fastreset: the template pages of a captured snapshot (V.pages must be empty: call after clear_all()) */
static void tmpl_build(const SetupState& st) {
    for (const SetupPage& sp : st.pages) {
        TPage* t = (TPage*)calloc(1, sizeof(TPage));
        memcpy(t->have, sp.have, sizeof t->have);
        memcpy(t->fw, sp.fw, sizeof t->fw);
        memcpy(t->dw, sp.dw, sizeof t->dw);
        memcpy(t->wr_pre, sp.wr, sizeof t->wr_pre);
        bool marks = false;
        for (int w = 0; w < 64 && !marks; w++) marks = sp.fw[w] | sp.dw[w];
        if (marks) t->fm = (uint64_t*)calloc(128, sizeof(uint64_t));
        for (auto& c : sp.ch) {
            TChunk* tc = (TChunk*)malloc(sizeof(TChunk));
            memcpy(tc->d, c.second.d, 64);
            memcpy(tc->init_pre, c.second.init, 64);
            t->tc[c.first] = tc;
            t->live[c.first] = (Chunk*)calloc(1, sizeof(Chunk));
        }
        Page* p = (Page*)calloc(1, sizeof(Page));
        p->pn = sp.pn;
        p->t = t;
        V.pages[sp.pn] = p;
        g_tmpl_pages.push_back(p);
        tmpl_restore(p);
    }
}
static void tmpl_destroy() {
    if (g_tmpl_pages.empty()) return;
    clear_all(); /* restores the template pages and drops all others */
    for (Page* p : g_tmpl_pages) {
        V.pages.erase(p->pn);
        TPage* t = const_cast<TPage*>(p->t);
        for (int w = 0; w < 64; w++) { free(t->tc[w]); free(t->live[w]); }
        free(t->fm);
        free(t);
        free(p);
    }
    g_tmpl_pages.clear();
    memset(g_pcache, 0, sizeof g_pcache);
}

/* -fastreset, setupsnapshot validation inputs (the first input of a run and every -snapcheck'th): the original side runs
 * the setups for real and checks them against the snapshot, as before. The candidate side used to run them for real a
 * second time on the same input; the setups are original code on the same input state, so they leave the same state
 * (the scratch stack window included, which the snapshot does not hold). With -fastreset the original side's complete
 * post-setup state is kept (ValState) and the candidate side installs it instead (half the validation cost: a setup run
 * of the AGL world costs ~1.7 s CPU). WWHD_VERIFY_FASTRESET_CHECK=1 runs the candidate's setups for real as well and
 * exits 3 if the state differs from the kept one (self-check). */
struct ValChunk {
    uint32_t w;
    uint8_t d[64], init[64];   /* materialized bytes only (the others are zero) */
    std::vector<uint32_t> ep;  /* clobber epochs of the materialized bytes; empty: all 0 (setups run at epoch 0) */
};
struct ValPage { uint32_t pn; uint64_t have[64], wr[64], fw[64], dw[64]; std::vector<ValChunk> ch; };
struct ValState {
    bool valid = false;
    uint64_t serial = 0; /* g_input_serial of the input it was kept for (an original side that did not run, e.g. a
                          * recorded original, leaves no state for the next input's candidate) */
    std::vector<ValPage> pages; /* sorted by page number */
    bool fany = false;
    std::unordered_map<uint32_t, uint32_t> alloc_cursor, alloc_calls, per_target, retained, obs_ord;
    std::vector<uint32_t> osmem; /* `osmem`: the heap bookkeeping (osm::save) */
    uint64_t budget = 0, ncalls = 0;
    uint32_t epoch = 0, dl_start = 0, dl_size = 0, dl_cmds = 0, stray = 0, first_stray = 0;
    int traps = 0, stop_count = 0;
    unsigned nfcopy = 0;
    uint32_t fcopy[8] = {};
    size_t ncalls_rec = 0;
};
static ValState g_valstate;
static bool g_fastreset_check = [] { const char* e = getenv("WWHD_VERIFY_FASTRESET_CHECK"); return e && *e && strcmp(e, "0"); }();
static void val_capture(ValState* st) {
    st->pages.clear();
    for (Page* p : V.touched) {
        ValPage vp;
        vp.pn = p->pn;
        memcpy(vp.have, p->have, sizeof vp.have);
        memcpy(vp.wr, p->wr, sizeof vp.wr);
        memcpy(vp.fw, pfw(p), sizeof vp.fw);
        memcpy(vp.dw, pdw(p), sizeof vp.dw);
        bool any = false;
        for (int w = 0; w < 64; w++) any |= (vp.have[w] | vp.wr[w] | vp.fw[w] | vp.dw[w]) != 0;
        if (!any) continue;
        for (uint32_t w = 0; w < 64; w++) {
            if (!vp.have[w]) continue;
            ValChunk vc{};
            vc.w = w;
            const Chunk* k = p->ch[w];
            for (uint64_t m = vp.have[w]; m; m &= m - 1) { /* only materialized bytes have a value */
                int b = __builtin_ctzll(m);
                vc.d[b] = k->d[b];
                vc.init[b] = k->init[b];
                if (uint32_t e = ep_get(k, b)) {
                    if (vc.ep.empty()) vc.ep.assign(64, 0);
                    vc.ep[b] = e;
                }
            }
            vp.ch.push_back(vc);
        }
        st->pages.push_back(std::move(vp));
    }
    std::sort(st->pages.begin(), st->pages.end(), [](const ValPage& a, const ValPage& b) { return a.pn < b.pn; });
    st->fany = V.fany;
    st->alloc_cursor = g_alloc_cursor;
    st->alloc_calls = g_alloc_calls;
    st->per_target = V.per_target;
    st->retained = g_retained;
    st->obs_ord = V.obs_ord;
    st->osmem = osm::save();
    st->budget = V.budget;
    st->ncalls = V.ncalls;
    st->epoch = V.epoch;
    st->dl_start = g_dl_start;
    st->dl_size = g_dl_size;
    st->dl_cmds = g_dl_cmds;
    st->stray = V.stray;
    st->first_stray = V.first_stray;
    st->traps = V.traps;
    st->stop_count = V.stop_count;
    st->nfcopy = V.nfcopy;
    memcpy(st->fcopy, V.fcopy, sizeof st->fcopy);
    st->ncalls_rec = V.calls.size();
    st->valid = V.calls.empty(); /* (setups record no calls; if they ever do, the candidate runs them for real) */
    if (!gthr::live_reason().empty()) st->valid = false; /* `threads`: guest-thread contexts cannot be installed: run the setups */
    st->serial = g_input_serial;
}
/* the candidate side of a validation input: memory as reset_memory() left it (all bits clear) */
static void val_install(const ValState& st) {
    for (const ValPage& vp : st.pages) {
        Page* p = page(vp.pn);
        memcpy(p->have, vp.have, sizeof p->have);
        memcpy(p->wr, vp.wr, sizeof p->wr);
        page_set_marks(p, vp.fw, vp.dw);
        for (const ValChunk& vc : vp.ch) {
            Chunk*& k = p->ch[vc.w];
            if (!k) k = (Chunk*)arena_zalloc(sizeof(Chunk));
            for (uint64_t m = vp.have[vc.w]; m; m &= m - 1) {
                int b = __builtin_ctzll(m);
                k->d[b] = vc.d[b];
                k->init[b] = vc.init[b];
            }
            /* (ep_set with 0 on a chunk without epochs keeps none: a materialized byte without them is current at epoch 0) */
            for (uint64_t m = vp.have[vc.w]; m; m &= m - 1) { int b = __builtin_ctzll(m); ep_set(k, b, vc.ep.empty() ? 0 : vc.ep[b]); }
        }
    }
    V.fany = st.fany;
    g_alloc_cursor = st.alloc_cursor;
    g_alloc_calls = st.alloc_calls;
    V.per_target = st.per_target;
    g_retained = st.retained;
    V.obs_ord = st.obs_ord;
    osm::load(st.osmem);
    V.budget = st.budget;
    V.ncalls = st.ncalls;
    V.epoch = st.epoch;
    g_dl_start = st.dl_start;
    g_dl_size = st.dl_size;
    g_dl_cmds = st.dl_cmds;
    V.stray = st.stray;
    V.first_stray = st.first_stray;
    V.traps = st.traps;
    V.stop_count = st.stop_count;
    V.nfcopy = st.nfcopy;
    memcpy(V.fcopy, st.fcopy, sizeof V.fcopy);
}
static std::string val_diff(const ValState& a, const ValState& b) { /* "" when equal */
    char buf[128];
    if (a.pages.size() != b.pages.size()) { snprintf(buf, sizeof buf, "page count %zu vs %zu", a.pages.size(), b.pages.size()); return buf; }
    for (size_t i = 0; i < a.pages.size(); i++) {
        const ValPage &x = a.pages[i], &y = b.pages[i];
        if (x.pn != y.pn) { snprintf(buf, sizeof buf, "page %05X vs %05X", x.pn, y.pn); return buf; }
        if (memcmp(x.have, y.have, sizeof x.have) || memcmp(x.wr, y.wr, sizeof x.wr) || memcmp(x.fw, y.fw, sizeof x.fw) ||
            memcmp(x.dw, y.dw, sizeof x.dw) || x.ch.size() != y.ch.size()) {
            snprintf(buf, sizeof buf, "bits of page %05X", x.pn); return buf;
        }
        for (size_t j = 0; j < x.ch.size(); j++)
            if (x.ch[j].w != y.ch[j].w || memcmp(x.ch[j].d, y.ch[j].d, 64) || memcmp(x.ch[j].init, y.ch[j].init, 64) ||
                x.ch[j].ep != y.ch[j].ep) {
                snprintf(buf, sizeof buf, "bytes of %08X+64", x.pn << 12 | x.ch[j].w << 6); return buf;
            }
    }
    /* the first run counter that differs, by name (2026-10-29: "run counters" alone hid which one) */
#define VAL_CNT(f) if (!(a.f == b.f)) return "run counters: " #f;
    VAL_CNT(fany) VAL_CNT(alloc_cursor) VAL_CNT(alloc_calls) VAL_CNT(per_target) VAL_CNT(retained) VAL_CNT(obs_ord) VAL_CNT(osmem)
    VAL_CNT(budget) VAL_CNT(ncalls) VAL_CNT(epoch) VAL_CNT(dl_start) VAL_CNT(dl_size) VAL_CNT(dl_cmds) VAL_CNT(stray)
    VAL_CNT(first_stray) VAL_CNT(traps) VAL_CNT(stop_count) VAL_CNT(nfcopy) VAL_CNT(ncalls_rec)
#undef VAL_CNT
    if (memcmp(a.fcopy, b.fcopy, sizeof a.fcopy)) return "run counters: fcopy";
    return "";
}

/* -snapcache: the post-setup state of one setupsnapshot validation input (the first input of a run
 * and every -snapcheck'th), as the original side's real setup run leaves it: the complete memory state val_capture()
 * keeps (scratch stack window included) and the run counters, plus what only the original side counts (the stack-guard
 * bytes and the semantic-effect statistics). The writing run (mutate.py's baseline) records it on the original side; a
 * reading run installs it on both sides instead of running the setups (see namespace snapc after refc). */
struct SnapRec {
    ValState st;
    std::vector<uint32_t> orig_stack; /* sorted */
    uint64_t stats[6] = {};           /* as refc::Stats: set calls/bytes, surface calls, move calls/bytes, utf16 failures */
    std::vector<uint32_t> utf16;      /* (length, added count) pairs */
    bool ok = false;                  /* writer: the setups completed, recorded no call and made no non-code fetch */
};
static const SnapRec* g_snapc_rec = nullptr; /* reader: this validation input's recorded state (nullptr: run the setups) */
static SnapRec* g_snapc_out = nullptr;       /* writer: the original side's state of this validation input goes here */
static bool g_snapc_check = false;           /* -snapcache-check: run the setups anyway and compare with the record */
static void snapc_stats(uint64_t v[6], std::vector<uint64_t>* u) {
    v[0] = g_set_calls; v[1] = g_set_bytes; v[2] = g_surface_calls; v[3] = g_move_calls; v[4] = g_move_bytes; v[5] = g_utf16_fail;
    u->clear();
    if (!g_utf16_specs.empty()) u->assign(g_utf16_length, g_utf16_length + 4097);
}
/* the state right after a real setup run; v0/u0: the statistics before it */
static void snapc_take(SnapRec* r, bool candidate, const uint64_t v0[6], const std::vector<uint64_t>& u0) {
    val_capture(&r->st);
    r->orig_stack.clear();
    r->utf16.clear();
    memset(r->stats, 0, sizeof r->stats);
    if (!candidate) {
        r->orig_stack.assign(V.orig_stack.begin(), V.orig_stack.end());
        std::sort(r->orig_stack.begin(), r->orig_stack.end());
        uint64_t v1[6];
        std::vector<uint64_t> u1;
        snapc_stats(v1, &u1);
        for (int k = 0; k < 6; k++) r->stats[k] = v1[k] - v0[k];
        for (size_t k = 0; k < u1.size() && k < u0.size(); k++)
            if (u1[k] != u0[k]) { r->utf16.push_back((uint32_t)k); r->utf16.push_back((uint32_t)(u1[k] - u0[k])); }
    }
    /* a non-code fetch inside the setups is part of the original's Result (badfetch) and, on the candidate side, consumes
     * the original's allowance: such an input is not cached (it runs its setups as before) */
    r->ok = r->st.valid && g_badfetch.empty();
}
/* the original side's part of an installed record (the memory and counters go in with val_install) */
static void snapc_apply_orig(const SnapRec& r) {
    V.orig_stack.insert(r.orig_stack.begin(), r.orig_stack.end());
    g_set_calls += r.stats[0]; g_set_bytes += r.stats[1]; g_surface_calls += r.stats[2];
    g_move_calls += r.stats[3]; g_move_bytes += r.stats[4]; g_utf16_fail += r.stats[5];
    for (size_t k = 0; k + 1 < r.utf16.size(); k += 2) g_utf16_length[r.utf16[k]] += r.utf16[k + 1];
}
static std::string snapc_diff(const SnapRec& rec, const SnapRec& now, bool candidate) { /* "" when equal */
    std::string d = val_diff(rec.st, now.st);
    if (!d.empty()) return d;
    if (!now.ok) return "the live setups recorded a call or made a non-code fetch";
    if (candidate) return "";
    if (rec.orig_stack != now.orig_stack) return "the stack-guard bytes";
    if (memcmp(rec.stats, now.stats, sizeof rec.stats) || rec.utf16 != now.utf16) return "the semantic-effect statistics";
    return "";
}

/* static part of the input-independence check: a setup operand `rN=*ADDR`, `rN=*(*ADDR+OFF)` or `fN=*ADDR` whose word at
 * ADDR a per-input steering line of the function can set (an absolute `field` with more than one possible value, a
 * `fieldstr`, or a `poke`: setups must not read poked addresses, they would see the generated pre-poke bytes) refuses
 * the snapshot without trying. Constant `field`, `fill` and `copy` lines are the same in every input. Register-relative
 * steering and memory a setup reads inside its routine are left to the run-twice check. "" when nothing is found. */
static std::string setup_static_dependency(uint32_t func) {
    auto mine = [&](uint32_t f) { return !f || f == func; };
    auto overlap = [](uint32_t a, uint32_t lo, uint32_t n) { return (uint64_t)a + 4 > lo && (uint64_t)lo + n > a; };
    for (const SetupSpec& su : g_setup_specs) {
        if (!mine(su.func)) continue;
        for (const SetupReg& rv : su.regs) {
            if (!rv.deref) continue;
            uint32_t a = rv.value;
            const char* what = nullptr;
            for (const FieldSpec& f : g_specs)
                if (mine(f.func) && f.base_reg < 0 && overlap(a, f.off, (uint32_t)f.size) &&
                    (f.pick.size() > 1 || (f.pick.empty() && f.lo != f.hi)))
                    what = "a per-input `field`";
            for (const StrSpec& f : g_str_specs)
                if (mine(f.func) && f.base_reg < 0) {
                    size_t n = 1;
                    for (auto& p : f.pick) n = std::max(n, p.size() + 1);
                    if (overlap(a, f.off, (uint32_t)n)) what = "a `fieldstr`";
                }
            for (const PokeSpec& pk : g_poke_specs)
                if (mine(pk.func) && pk.base_reg < 0 && overlap(a, pk.off, pk.size)) what = "a `poke`";
            if (what) {
                char b[200];
                snprintf(b, sizeof b, "setup result depends on the input: setup %08X operand %c%d=*%08X reads %s", su.target,
                         rv.fp ? 'f' : 'r', rv.reg, a, what);
                return b;
            }
        }
    }
    return "";
}

static void run(void (*fn)(Cpu*), const Input& in, bool candidate, Result* out, std::vector<uint32_t>* cov) {
    /* -fastreset: a run that installs the snapshot starts from the template pages (only pages touched since are reset) */
    const bool fast = !g_tmpl_pages.empty() && g_snap_mode == SNAP_USE && !in.recorded;
    g_fast_prebase = false;
    if (!candidate) g_valstate.valid = false; /* kept for this input's candidate side only */
    if (fast) fast_reset();
    else reset_memory();
    V.calls.clear();
    /* one block for the run's call records (BigAlloc): its call budget bounds them, so they never move or double; only the
     * pages written become resident, and the block is unmapped when the Result that takes it goes */
    V.calls.reserve(g_call_budget + 1);
    V.per_target.clear();
    V.obs_ord.clear();
    g_badfetch.clear();
    g_alloc_cursor.clear();
    g_alloc_calls.clear();
    if (osm::parts) osm::boot(); /* `osmem`: mem_setup_heaps' boot layout */
    gthr::reset(); /* `osmem ... threads`: the main thread only */
    g_dsi_thread = 0;
    g_in_peek = 0;
    g_dl_start = g_dl_size = g_dl_cmds = 0;
    g_retained.clear();
    V.nfcopy = 0;
    /* the whole ring, not only its count: slots at or past nfcopy are never read (vm_st32), but a
     * stale slot from the previous run made the -fastreset setup-state check (val_diff, run under -watch/-trace or
     * WWHD_VERIFY_FASTRESET_CHECK) report `run counters` when the candidate re-ran the setups */
    memset(V.fcopy, 0, sizeof V.fcopy);
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
    V.npages = 0;
    V.run_serial++; /* page budget: a new set of touched pages; the cache holds only pages touched in this run */
    memset(g_pcache, 0, sizeof g_pcache);
    V.ncalls = 0;
    V.stop_count = 0;
    V.mrej = false;
    V.real_chain.clear();
    V.stray = 0;
    V.p0 = false;
    V.traps = 0;
    g_setup_what = nullptr;
    /* an abort (longjmp) skips the destructors of the candidate's gabi::AutoFrame / Activation, so the frame
     * context and the activation depth of the aborted input would leak into the next run (JPEG-SDK: stale stack
     * objects at 0x1FF0 in an unrelated function after an aborting input in the same shard) */
    gabi::frame_ctx = nullptr;
    gabi::Activation::depth = 0;
    V.cov = cov;
    V.mode = in.recorded ? MODE_REC : MODE_GEN;
    V.rec_calls = &in.calls;
    V.entry_sp = in.entry.r[1];
    V.stack_lo = in.entry.r[1] - 0x10000;
    Cpu c = in.entry;
    gabi::cpu = &c;
    prog::run_begin(V.seed, candidate);
    gthr::tested_reset();
    V.can_abort = true;
    g_run_timed_out = 0;
    int jr = setjmp(V.abort_jmp);
    if (jr == 0) {
        run_timer(true);
        bool prestate = false; /* a setup, poke or postsetup ran for this function: its result is the baseline */
        auto run_setups = [&](const std::vector<SetupSpec>& specs, const char* what) {
            for (const SetupSpec& su : specs) {
                if (su.func && su.func != g_cur_func) continue;
                prestate = true;
                const CalleeInfo* sci = callee_info(su.target);
                if (!sci || !sci->real) {
                    fprintf(stderr, "%s target %08X is not listed `real` in this unit\n", what, su.target);
                    exit(2);
                }
                Cpu s = c;
                s.r[1] = in.entry.r[1] - 0x8000;
                for (const SetupReg& rv : su.regs) {
                    uint32_t v = rv.value;
                    if (rv.fp) { /* the f32 value as the double an lfs would load (both paired-single halves) */
                        if (rv.deref) {
                            V.depth++;
                            v = (uint32_t)peek8(rv.value) << 24 | (uint32_t)peek8(rv.value + 1) << 16 | (uint32_t)peek8(rv.value + 2) << 8 | peek8(rv.value + 3);
                            V.depth--;
                        }
                        float fv; memcpy(&fv, &v, 4);
                        s.f[rv.reg].ps0 = s.f[rv.reg].ps1 = (double)fv;
                        if (g_trace) printf("        %s %08X f%d = %08X\n", what, su.target, rv.reg, v);
                        continue;
                    }
                    if (rv.deref) {
                        auto w32 = [](uint32_t a) { return (uint32_t)peek8(a) << 24 | (uint32_t)peek8(a + 1) << 16 | (uint32_t)peek8(a + 2) << 8 | peek8(a + 3); };
                        V.depth++;
                        v = w32(v);
                        if (rv.deref == 2 && v) v = w32(v + rv.off);
                        V.depth--;
                        if (!v) {
                            fprintf(stderr, "%s %08X: operand r%d=*... reads 0 (the producer did not run or failed)\n", what, su.target, rv.reg);
                            exit(2);
                        }
                    }
                    s.r[rv.reg] = v;
                }
                V.depth++;
                for (const SetupStack& st : su.stack) {
                    for (uint32_t j = 0; j < 4; j++) patch8(s.r[1] + st.off + j, (uint8_t)(st.value >> (24 - 8 * j)));
                    if (g_trace) printf("        %s %08X sp+0x%X <- %08X\n", what, su.target, st.off, st.value);
                }
                V.real_chain.push_back(su.target);
                g_setup_what = what;
                g_setup_target = su.target;
                sci->real(&s);
                g_setup_what = nullptr;
                V.real_chain.pop_back();
                V.depth--;
                if (g_trace) printf("        %s %08X -> r3 %08X\n", what, su.target, s.r[3]);
                if (su.store) {
                    if (!s.r[3]) {
                        fprintf(stderr, "%s %08X: ->%08X stores r3 = 0 (the producer failed)\n", what, su.target, su.store_addr);
                        exit(2);
                    }
                    V.depth++;
                    for (uint32_t j = 0; j < 4; j++) patch8(su.store_addr + j, (uint8_t)(s.r[3] >> (24 - 8 * j)));
                    V.depth--;
                    if (g_trace) printf("        %s %08X ->%08X <- %08X\n", what, su.target, su.store_addr, s.r[3]);
                }
            }
        };
        uint64_t sc0 = g_set_calls, sb0 = g_set_bytes, su0 = g_surface_calls;
        bool snapc_installed = false; /* -snapcache: this validation input's setups came from the record */
        if (g_snap_mode == SNAP_USE && V.mode == MODE_GEN) { /* `setupsnapshot`: the captured state replaces the setups */
            setup_state_install(g_snap, !fast);
            g_fast_prebase = fast;
            prestate = true;
            if (g_trace) printf("        setupsnapshot: installed (%zu bytes)\n", g_snap.nbytes);
        } else if (g_snapc_rec && !g_snapc_check && g_snap_mode == SNAP_VALIDATE && V.mode == MODE_GEN) {
            /* -snapcache: the post-setup state the baseline's real setup run left on this input (both sides, as the
             * candidate side's val_install below does with the original side's state of the same input) */
            val_install(g_snapc_rec->st);
            if (!candidate) snapc_apply_orig(*g_snapc_rec);
            prestate = true;
            snapc_installed = true;
        } else if (g_fastreset && g_snap_mode == SNAP_VALIDATE && V.mode == MODE_GEN && candidate && g_valstate.valid &&
                   g_valstate.serial == g_input_serial && !g_fastreset_check && !g_trace && !g_watch_len) { /* (-trace/-watch: the candidate's setup lines as before) */
            val_install(g_valstate); /* -fastreset: the original side's post-setup state of this input */
            prestate = true;
        } else {
            const bool snapc_take_now = g_snap_mode == SNAP_VALIDATE && V.mode == MODE_GEN &&
                                        ((g_snapc_out && !candidate) || (g_snapc_rec && g_snapc_check));
            uint64_t sv0[6];
            std::vector<uint64_t> su0v;
            if (snapc_take_now) snapc_stats(sv0, &su0v);
            run_setups(g_setup_specs, "setup");
            gthr::drain(); /* `threads`: work the setups started is baseline */
            if (snapc_take_now) {
                if (g_snapc_out && !candidate) snapc_take(g_snapc_out, false, sv0, su0v); /* -snapcache-write */
                else { /* -snapcache-check: the live setups against the record */
                    SnapRec now;
                    snapc_take(&now, candidate, sv0, su0v);
                    std::string d = snapc_diff(*g_snapc_rec, now, candidate);
                    if (!d.empty()) {
                        fprintf(stderr, "snapcache: CHECK FAILED %08X input %ld (%s side): the recorded post-setup state differs "
                                "from the live setups (%s)\n", g_cur_func, g_snap_input, candidate ? "candidate" : "original", d.c_str());
                        exit(98);
                    }
                }
            }
            if (g_fastreset && g_snap_mode == SNAP_VALIDATE && V.mode == MODE_GEN) {
                if (!candidate) val_capture(&g_valstate);
                else if (g_valstate.valid && g_valstate.serial == g_input_serial) { /* WWHD_VERIFY_FASTRESET_CHECK */
                    ValState now;
                    val_capture(&now);
                    std::string d = val_diff(g_valstate, now);
                    if (!d.empty()) {
                        fprintf(stderr, "fastreset check: the candidate side's setups left a different state than the original "
                                "side's (%s), input %s\n", d.c_str(), in.label.c_str());
                        exit(3);
                    }
                }
            }
        }
        if (g_snap_mode == SNAP_CAPTURE) { /* the state after the setups: stop the run here */
            setup_state_capture(g_snap_out, sc0, sb0, su0);
            longjmp(V.abort_jmp, 3);
        }
        if (g_snap_mode == SNAP_VALIDATE && V.mode == MODE_GEN && !candidate && !snapc_installed) { /* periodic check against the snapshot
                                                                                       * (-snapcache: the record passed it in the baseline) */
            SetupState now;
            setup_state_capture(&now, sc0, sb0, su0);
            std::string detail;
            std::string d = setup_state_diff(g_snap, now, &detail);
            if (!d.empty()) {
                fprintf(stderr, "setupsnapshot: validation failed at input %ld (%s): the setups' state differs from the snapshot, "
                        "first difference at %s%s%s%s\n", g_snap_input, in.label.c_str(), d.c_str(), detail.empty() ? "" : " (snapshot ",
                        detail.c_str(), detail.empty() ? "" : " here)");
                exit(2);
            }
            if (g_trace) printf("        setupsnapshot: validated (%zu bytes)\n", now.nbytes);
        }
        for (size_t pi = 0; pi < g_poke_specs.size(); pi++) {
            const PokeSpec& pk = g_poke_specs[pi];
            if (pk.func && pk.func != g_cur_func) continue;
            prestate = true;
            uint32_t a = (pk.base_reg >= 0 ? c.r[pk.base_reg] : 0) + pk.off;
            uint64_t ph = V.seed * 0x9E3779B97F4A7C15ull + (pi + 1) * 0xBF58476D1CE4E5B9ull;
            ph ^= ph >> 31; ph *= 0x94D049BB133111EBull; ph ^= ph >> 29;
            uint32_t value = pk.values[ph % pk.values.size()]; /* per input, independent of the side */
            V.depth++;
            for (uint32_t j = 0; j < pk.size; j++) patch8(a + j, (uint8_t)(value >> (8 * (pk.size - 1 - j))));
            V.depth--;
            if (g_trace) printf("        poke %08X <- %0*X\n", a, (int)pk.size * 2, value);
        }
        run_setups(g_postsetup_specs, "postsetup");
        gthr::drain();
        if (V.mode == MODE_GEN && prestate)
            /* the state after the setups, pokes and postsetups is the baseline (2026-10-25, AGL#18 speed; 2026-10-26: every
             * materialized byte, not only the setups' own writes, so poked bytes and mock effects inside setups count too,
             * AGL#18 027CDE3C): it is identical on both sides (the same original setups and pokes on the same input), so the
             * net effect is only what the tested function changes afterwards. Generated inputs only: recorded inputs compare
             * against the game's recorded write set, whose baseline is the entry snapshot */
            for (Page* p : V.touched)
                for (int w = 0; w < 64; w++) {
                    /* whole chunk at once (2026-10-26, AGL#18 perf: was byte by byte over every materialized byte, ~0.3 MB
                     * per run and side after a large setup): a byte that is not materialized has no observable init (net_writes
                     * reads init only for written, hence materialized, bytes; materialize sets d and init together) */
                    if (p->have[w]) memcpy(p->ch[w]->init, p->ch[w]->d, sizeof p->ch[w]->init);
                    p->wr[w] = 0;
                }
        g_fast_prebase = false; /* -fastreset: the untouched template pages are in exactly this rebased state */
        gthr::tested_starts(); /* `threads`: candidate code in guest threads starts here */
        fn(&c);
        gthr::drain(); /* `threads`: the other threads finish what the tested call started (until all of them wait) */
        prog::run_t0.store(0); /* finished: the watchdog's hang limit no longer applies to this run */
    } else if (jr == 2) {
        out->stopped = true;
    } else if (jr == 3) {
        /* SNAP_CAPTURE: stopped after the setups */
    } else if (jr == 4) { /* -nulltrap: a page-0 access (DSI) ended this side's run */
        out->dsi = true;
        out->dsi_kind = g_dsi.kind;
        out->dsi_ea = g_dsi.ea;
        out->dsi_depth = g_dsi.depth;
        out->dsi_in = g_dsi.in;
        out->dsi_thread = g_dsi_thread;
        if (g_trace)
            printf("        DSI (%s side): %s %s %08X at depth %d in=%08X\n", candidate ? "candidate" : "original",
                   g_dsi.kind == 'X' ? "page-0/invalid" : "page-0", dsi_what(g_dsi.kind), g_dsi.ea, g_dsi.depth, g_dsi.in);
    } else {
        if (g_run_timed_out) {
            g_run_timed_out = 0;
            char why[96];
            snprintf(why, sizeof why, "time budget exceeded (%d s CPU in one run)", kRunCpuSeconds);
            V.abort_reason = why;
        } else if (prog::wall_fired) { /* the watchdog's wall-clock hang limit (WWHD_VERIFY_HANG_LIMIT) */
            char why[96];
            snprintf(why, sizeof why, "wall-clock limit exceeded (%.0f s in one run)", prog::hang_limit);
            V.abort_reason = why;
        }
        prog::wall_fired = 0;
        out->aborted = true;
        out->why = V.abort_reason;
        if (V.mrej) { /* model_reject(): which call the model rejected */
            out->mrej = true;
            out->mrej_target = V.mrej_target;
            out->mrej_depth = V.mrej_depth;
            out->mrej_in = V.mrej_in;
        }
        if (g_trace) {
            printf("        aborted (%s side): %s", candidate ? "candidate" : "original", V.abort_reason.c_str());
            if (!V.real_chain.empty()) {
                printf(" [inside real");
                size_t skip = V.real_chain.size() > 16 ? V.real_chain.size() - 16 : 0; /* a deep chain: the innermost 16 */
                if (skip) printf(" (%zu outer levels not shown)", skip);
                for (size_t k = skip; k < V.real_chain.size(); k++) printf(" %08X", V.real_chain[k]);
                printf("]");
            }
            printf("\n");
        }
    }
    run_timer(false);
    V.can_abort = false;
    if (g_fast_prebase) { /* -fastreset: ended before the rebase */
        tmpl_unbaseline();
        g_fast_prebase = false;
    }
    prog::run_end();
    V.cov = nullptr;
    out->exit = c;
    out->entry_sp = in.entry.r[1];
    out->calls = std::move(V.calls); /* moved, not copied */
    V.calls.clear();
    out->badfetch = std::move(g_badfetch);
    g_badfetch.clear();
    if (!candidate) {
        /* rebuilt at this run's exact size (2026-11-02): clear() kept the capacity of the largest original run so
         * far, e.g. 33.5M entries (400 MB) after a dictionary pre-run that made 28M top-level calls, for the rest of the process */
        BigVec<std::pair<uint32_t, int>>().swap(V.orig_seq);
        BigVec<uint32_t>().swap(V.orig_lr);
        V.orig_seq.reserve(out->calls.size());
        V.orig_lr.reserve(out->calls.size());
        for (auto& e : out->calls) { V.orig_seq.push_back({e.target, e.kind}); V.orig_lr.push_back(e.lr); }
    }
    bool net_over = false;
    uint64_t net_over_mb = 0;
    out->net = net_writes(&net_over, &net_over_mb);
    if (net_over) { /* host memory budget while collecting the net effect: the reason names that cause
                     * (2026-11-09); a run that already aborted for another reason (e.g. `access budget exceeded`, AGL#14 #2 at
                     * seed 7) keeps that reason first and gets this one appended, so the receipt shows both. A run the budget
                     * already ended inside the run keeps that reason alone (its cause is the run itself). */
        const bool in_run = out->aborted && !out->why.compare(0, 32, "host memory budget exceeded (RSS");
        if (!in_run) {
            std::string cause = "host memory budget exceeded (RSS " + std::to_string(net_over_mb) + " MB > " +
                                std::to_string(g_hostmem_budget_mb) + " MB in one run, collecting its net effect)";
            out->why = out->aborted ? out->why + "; " + cause : cause;
            out->aborted = true;
            /* also on stderr (the row names an abort's reason only when one side aborted): the first 10 per process */
            static int shown = 0;
            if (shown < 10) {
                shown++;
                fprintf(stderr, "hostmembudget: %08X %s side, gen seed=%llu: %s\n", g_cur_func_fp, candidate ? "candidate" : "original",
                        (unsigned long long)V.seed, out->why.c_str());
            }
        }
        if (g_trace) printf("        net effect (%s side): not collected (%s)\n", candidate ? "candidate" : "original", out->why.c_str());
    }
    if (g_trace) { /* -trace: the run's net memory effect (what the comparison sees), as address ranges, per side */
        printf("        net effect (%s side): %zu bytes\n", candidate ? "candidate" : "original", out->net.size());
        uint32_t start = 0, prev = 0; std::string bytes; bool open = false;
        auto flush = [&]() { if (open) printf("          %08X: %s\n", start, bytes.c_str()); bytes.clear(); open = false; };
        for (auto& kv : out->net) {
            if (!open || kv.first != prev + 1 || bytes.size() >= 64) { flush(); start = kv.first; open = true; }
            char h[3]; snprintf(h, sizeof h, "%02X", kv.second); bytes += h; prev = kv.first;
        }
        flush();
    }
    out->fwords.clear();
    out->dwords.clear();
    if (V.fany && !g_strict_nan) { /* the Result keeps the marks as ascending address lists for same_net's NaN tolerance (not used
                                    * under -strictnan, so not built): pages in address order, bits in order */
        std::vector<std::pair<uint32_t, const Page*>> pg;
        pg.reserve(V.pages.size());
        for (auto& kv : V.pages) pg.push_back({kv.first, kv.second});
        std::sort(pg.begin(), pg.end(), [](const auto& x, const auto& y) { return x.first < y.first; });
        for (auto& [pn, p] : pg)
            for (uint32_t c = 0; c < 64; c++) {
                for (uint64_t m = pfw(p)[c]; m; m &= m - 1) out->fwords.push_back(pn << 12 | c << 6 | __builtin_ctzll(m));
                for (uint64_t m = pdw(p)[c]; m; m &= m - 1) out->dwords.push_back(pn << 12 | c << 6 | __builtin_ctzll(m));
            }
    }
    if (auto fs = g_final_specs.find(g_cur_func); fs != g_final_specs.end() && !out->aborted)
        for (auto& r : fs->second)
            for (uint32_t j = 0; j < r.second; j++) out->final_bytes.push_back(peek8(r.first + j));
    gthr::take(&out->thr);
    if (g_trace && gthr::on && out->thr.used)
        printf("        threads (%s side): schedule digest %016llX, %u thread(s), %llu switch(es), %llu slice preemption(s), %llu event(s)%s\n",
               candidate ? "candidate" : "original", (unsigned long long)out->thr.digest, out->thr.nthreads,
               (unsigned long long)out->thr.nswitch, (unsigned long long)out->thr.npreempt, (unsigned long long)out->thr.events,
               out->thr.deadlock ? ", deadlock" : "");
    out->stray = V.stray;
    out->p0 = V.p0;
    out->first_stray = V.first_stray;
    out->traps = V.traps;
    if (g_memstat) { /* WWHD_MEMSTAT (diagnostic, off by default): per run guest pages, arena bytes, calls, accesses */
        size_t arena = 0, fmarks = 0, dmarks = 0;
        for (auto& ch : g_arena_chunks) arena += ch.second;
        for (auto& kv : V.pages)
            for (int w = 0; w < 64; w++) { fmarks += __builtin_popcountll(pfw(kv.second)[w]); dmarks += __builtin_popcountll(pdw(kv.second)[w]); }
        fprintf(stderr, "memstat %s seed=%llu runpages=%llu pages=%zu arena=%.1f MB snap=%zu calls=%zu budget=%llu fmarks=%zu dmarks=%zu net=%zu\n",
                candidate ? "cand" : "orig", (unsigned long long)V.seed, (unsigned long long)V.npages, V.pages.size(), arena / 1048576.0, V.snap.size(),
                out->calls.size(), (unsigned long long)V.budget, fmarks, dmarks, out->net.size());
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
static bool is_nan32(uint32_t w) { return (w & 0x7F800000u) == 0x7F800000u && (w & 0x007FFFFFu); }
static bool same_f(double a, double b) {
    if (!g_strict_nan && std::isnan(a) && std::isnan(b)) return true;
    return f64_as_u64(a) == f64_as_u64(b);
}

/* net memory effects equal, up to NaN payloads of aligned float words */
static bool same_net(const NetMap& x, const NetMap& y, uint32_t* where,
                     const std::vector<uint32_t>& fx, const std::vector<uint32_t>& fy,
                     const std::vector<uint32_t>& dx, const std::vector<uint32_t>& dy) {
    if (x == y) return true;
    std::set<uint32_t> diff; /* bytes whose final values differ */
    for (auto& kv : x) { auto it = y.find(kv.first); if (it == y.end() || it->second != kv.second) diff.insert(kv.first); }
    for (auto& kv : y) { auto it = x.find(kv.first); if (it == x.end() || it->second != kv.second) diff.insert(kv.first); }
    auto final_word = [&](const NetMap& m, uint32_t a) {
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
        std::vector<uint32_t> doubles; /* dx ∪ dy, ascending (both are ascending and unique) */
        std::set_union(dx.begin(), dx.end(), dy.begin(), dy.end(), std::back_inserter(doubles));
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
        std::vector<uint32_t> fl; /* fx ∪ fy, ascending */
        std::set_union(fx.begin(), fx.end(), fy.begin(), fy.end(), std::back_inserter(fl));
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
/* -nulltrap: inputs of the current function on which both sides faulted at different accesses and the common call prefix
 * matched (counted by compare(), reported per function on stderr) */
static int g_dsi_order = 0;
static bool g_cmp_prefix = false; /* model_compare(): compare_full compares only the common call prefix */
/* `observenested`: the first difference between two nested-call sequences, and both sequences (each call as TARGET or
 * TARGET(span bytes, `null` for an absent span), at most 12 shown) */
typedef std::vector<std::pair<uint32_t, std::vector<uint8_t>>> NestSeq;
static std::string nested_call_str(const std::pair<uint32_t, std::vector<uint8_t>>& e) {
    std::string s;
    char b[16];
    snprintf(b, sizeof b, "%08X", e.first);
    s = b;
    auto ns = g_nested_specs.find(e.first);
    if (ns == g_nested_specs.end() || ns->second.empty()) return s;
    s += "(";
    size_t p = 0;
    for (size_t i = 0; i < ns->second.size() && p < e.second.size(); i++) {
        if (i) s += ",";
        bool have = e.second[p++];
        if (!have) { s += "null"; continue; }
        for (uint32_t j = 0; j < ns->second[i].size && p < e.second.size(); j++, p++) {
            if (j == 8) { s += ".."; p += ns->second[i].size - j; break; }
            snprintf(b, sizeof b, "%02X", e.second[p]);
            s += b;
        }
    }
    return s + ")";
}
static std::string nested_seq_str(const NestSeq& v) {
    std::string s = "[";
    for (size_t i = 0; i < v.size() && i < 12; i++) s += (i ? " " : "") + nested_call_str(v[i]);
    if (v.size() > 12) s += " ...";
    return s + "]";
}
static std::string nested_diff(const NestSeq& x, const NestSeq& y) {
    size_t i = 0;
    while (i < x.size() && i < y.size() && x[i] == y[i]) i++;
    char b[320];
    if (i == x.size() || i == y.size())
        snprintf(b, sizeof b, "nested observed calls differ in number: original %zu candidate %zu", x.size(), y.size());
    else if (x[i].first != y[i].first)
        snprintf(b, sizeof b, "nested observed call #%zu: original calls %s, candidate %s", i, callee_name(x[i].first).c_str(),
                 callee_name(y[i].first).c_str());
    else {
        /* the first differing span */
        std::string where = "bytes";
        auto ns = g_nested_specs.find(x[i].first);
        const auto &xb = x[i].second, &yb = y[i].second;
        size_t p = 0;
        if (ns != g_nested_specs.end())
            for (const ObservePtr& o : ns->second) {
                if (p >= xb.size()) break;
                size_t n = 1 + (xb[p] ? o.size : 0);
                if (xb[p] != yb[p]) { where = "span " + span_name(o) + (xb[p] ? " (null base on the candidate side)" : " (null base on the original side)"); break; }
                if (!std::equal(xb.begin() + p, xb.begin() + p + n, yb.begin() + p)) {
                    size_t d = 1;
                    while (xb[p + d] == yb[p + d]) d++;
                    where = "span " + span_name(o) + " differs at " + span_name(o, (uint32_t)(d - 1));
                    break;
                }
                p += n;
            }
        snprintf(b, sizeof b, "nested observed call #%zu %s: %s", i, callee_name(x[i].first).c_str(), where.c_str());
    }
    return std::string(b) + " (observenested; original " + nested_seq_str(x) + " candidate " + nested_seq_str(y) + ")";
}
static std::string compare_full(const Result& a, const Result& b, gabi::RetKind rk, const Input& in) {
    char buf[512];
    if (!g_cmp_prefix && a.stopped != b.stopped)
        return a.stopped ? "stopat: the original reached the terminal call, the candidate returned" : "stopat: the candidate reached the terminal call, the original returned";
    if (a.stopped || g_cmp_prefix) rk = gabi::RET_VOID; /* no return value at the terminal call */
    if (!g_cmp_prefix && a.dsi != b.dsi) { /* -nulltrap: one side took a page-0 DSI */
        const Result& t = a.dsi ? a : b;
        if (t.dsi_kind == 'X') /* an indirect call or branch to page 0 or a non-code address (ISI) */
            snprintf(buf, sizeof buf, "trap: page-0/invalid instruction fetch by the %s only (target %08X, depth %d%s%s)",
                     a.dsi ? "original" : "candidate", t.dsi_ea, t.dsi_depth, t.dsi_in ? " in " : "", t.dsi_in ? hex32(t.dsi_in).c_str() : "");
        else
            snprintf(buf, sizeof buf, "trap: page-0 DSI by the %s only (%s %08X, depth %d%s%s)", a.dsi ? "original" : "candidate",
                     dsi_what(t.dsi_kind), t.dsi_ea, t.dsi_depth, t.dsi_in ? " in " : "",
                     t.dsi_in ? hex32(t.dsi_in).c_str() : "");
        return buf;
    }
    /* both faulted (no return value): at the same access, calls, traps and net effect up to the fault compare; at different
     * accesses only the common call prefix does (2026-10-26, AGL#13: the input crashes the console either way, the fault order
     * is not an observable difference; the two sides stopped at different points, so neither the faulting access, the trap
     * count nor the net effect is compared) */
    bool prefix_only = g_cmp_prefix || (a.dsi && (a.dsi_kind != b.dsi_kind || a.dsi_ea != b.dsi_ea));
    if (a.dsi) rk = gabi::RET_VOID;
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
            bool ptr = false;
            for (size_t j = 0; j < xe.observed.size() && j < ye.observed.size(); j++)
                if (xe.observed[j] != ye.observed[j]) {
                    uint32_t key = xe.observed[j].first;
                    if (key >= 0xFFFF0000u) ptr = true;
                    else if (key >= 0xFE000000u) {
                        unsigned reg = key >> 16 & 0xFF, off = key & 0xFFFF;
                        const auto &xb = xe.observed[j].second, &yb = ye.observed[j].second;
                        size_t d = 0;
                        while (d < xb.size() && d < yb.size() && xb[d] == yb[d]) d++;
                        snprintf(buf, sizeof buf, "call #%zu %s: argument span r%u+0x%X differs at r%u+0x%zX (observearg)", k,
                                 callee_name(x.target).c_str(), reg, off, reg, off + d);
                        return buf;
                    }
                    break;
                }
            snprintf(buf, sizeof buf, "call #%zu %s: observed %s bytes differ", k, callee_name(x.target).c_str(), ptr ? "pointee (observeptr)" : "global");
            return buf;
        }
        if (!xe.sobserved.empty() || !ye.sobserved.empty()) { /* per-site / conditional observearg */
            const std::vector<ObservePtr>& specs = g_observeptr_specs[x.target];
            if (xe.sobserved.size() != ye.sobserved.size()) { /* cannot happen: same target, same position */
                snprintf(buf, sizeof buf, "call #%zu %s: observearg site records differ in number (%zu vs %zu)", k,
                         callee_name(x.target).c_str(), xe.sobserved.size(), ye.sobserved.size());
                return buf;
            }
            for (size_t j = 0; j < xe.sobserved.size(); j++) {
                const ObservePtr& o = specs[xe.sobserved[j].first];
                if (o.creg && (o.creg == 32 ? x.lr : x.r[o.creg]) != o.cval) continue; /* decided on the original */
                const auto &xb = xe.sobserved[j].second, &yb = ye.sobserved[j].second;
                if (xb == yb) continue;
                size_t d = 0;
                while (d < xb.size() && d < yb.size() && xb[d] == yb[d]) d++;
                char cond[64] = "";
                if (o.site >= 0) snprintf(cond, sizeof cond, " @%d", o.site);
                if (o.creg == 32) snprintf(cond + strlen(cond), sizeof cond - strlen(cond), " if lr=%08X", o.cval);
                else if (o.creg) snprintf(cond + strlen(cond), sizeof cond - strlen(cond), " if r%d=0x%X", o.creg, o.cval);
                if (o.deref && (xb.empty() || yb.empty())) {
                    snprintf(buf, sizeof buf, "call #%zu %s: pointee span %s: the base pointer [r%u+0x%X] is null on the %s side only (observearg%s)",
                             k, callee_name(x.target).c_str(), span_name(o).c_str(), o.reg, o.off, xb.empty() ? "original" : "candidate", cond);
                    return buf;
                }
                snprintf(buf, sizeof buf, "call #%zu %s: %s span %s differs at %s (observearg%s)", k, callee_name(x.target).c_str(),
                         o.deref ? "pointee" : "argument", span_name(o).c_str(), span_name(o, (uint32_t)d).c_str(), cond);
                return buf;
            }
        }
        if (xe.nested != ye.nested) { /* `observenested`: the calls made inside this call */
            std::string m = nested_diff(xe.nested, ye.nested);
            snprintf(buf, sizeof buf, "call #%zu %s: ", k, callee_name(x.target).c_str());
            return buf + m;
        }
        if (xe.crmask && xe.crbits != ye.crbits)
            for (int b = 0; b < 32; b++)
                if ((xe.crbits ^ ye.crbits) >> b & 1) {
                    snprintf(buf, sizeof buf, "call #%zu %s: entry CR bit %d original %u candidate %u (callcr)", k,
                             callee_name(x.target).c_str(), b, xe.crbits >> b & 1, ye.crbits >> b & 1);
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
            } else if (xe.href[i] != ye.href[i]) { /* rN=heapSIZE: same heap pointer, different object contents */
                size_t d = 0;
                while (d < xe.href[i].size() && d < ye.href[i].size() && xe.href[i][d] == ye.href[i][d]) d++;
                snprintf(buf, sizeof buf, "call #%zu %s: r%d -> heap data differs at +0x%zX", k, callee_name(x.target).c_str(), i, d);
                return buf;
            } else if (g_observe_pointees && (xe.pkind[i] != ye.pkind[i] || xe.pbase[i] != ye.pbase[i] || xe.psize[i] != ye.psize[i] ||
                                              xe.pfrom[i] != ye.pfrom[i] || xe.pref[i] != ye.pref[i])) {
                const char* kn = (xe.pkind[i] | ye.pkind[i]) == 2 ? "static object" : "heap block";
                if (xe.pkind[i] != ye.pkind[i] || xe.pbase[i] != ye.pbase[i] || xe.psize[i] != ye.psize[i] || xe.pfrom[i] != ye.pfrom[i]) {
                    snprintf(buf, sizeof buf, "call #%zu %s: r%d=%08X -> %s bounds differ: original %08X size 0x%X, candidate %08X size 0x%X "
                             "(-observe-pointees)", k, callee_name(x.target).c_str(), i, x.r[i], kn, xe.pbase[i], xe.psize[i], ye.pbase[i],
                             ye.psize[i]);
                    return buf;
                }
                size_t d = 0;
                while (d < xe.pref[i].size() && xe.pref[i][d] == ye.pref[i][d]) d++;
                uint32_t at = xe.pfrom[i] + (uint32_t)d;
                snprintf(buf, sizeof buf, "call #%zu %s: r%d=%08X -> %s %08X size 0x%X%s differs at +0x%X (%08X): original %02X candidate %02X "
                         "(-observe-pointees; a declared `callee %08X r%d=...` fact narrows it)", k, callee_name(x.target).c_str(), i, x.r[i],
                         kn, xe.pbase[i], xe.psize[i], xe.pref[i].size() < xe.psize[i] ? " (truncated to the cap)" : "", at - xe.pbase[i], at,
                         xe.pref[i][d], ye.pref[i][d], x.target, i);
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
    if (prefix_only) return "";
    if (a.stopped && b.stopped) { /* `stopat ... live REG...`: registers live past the stop */
        auto st = g_stop_specs.find(g_cur_func);
        if (st != g_stop_specs.end()) {
            for (int i = 0; i < 32; i++)
                if ((st->second.live_r >> i & 1) && a.exit.r[i] != b.exit.r[i]) {
                    snprintf(buf, sizeof buf, "stopat: live r%d at the terminal call: original %08X candidate %08X", i,
                             a.exit.r[i], b.exit.r[i]);
                    return buf;
                }
            for (int i = 0; i < 32; i++)
                if ((st->second.live_f >> i & 1) && (!same_f(a.exit.f[i].ps0, b.exit.f[i].ps0) || !same_f(a.exit.f[i].ps1, b.exit.f[i].ps1))) {
                    snprintf(buf, sizeof buf, "stopat: live f%d at the terminal call: original %.9g/%.9g candidate %.9g/%.9g", i,
                             a.exit.f[i].ps0, a.exit.f[i].ps1, b.exit.f[i].ps0, b.exit.f[i].ps1);
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
    if (a.traps != b.traps) { /* tw/twi taken by the original (vm_trap) vs gabi::trap by the candidate */
        snprintf(buf, sizeof buf, "trap count: original %d, candidate %d", a.traps, b.traps);
        return buf;
    }
    if (a.final_bytes != b.final_bytes) {
        size_t i = 0;
        while (i < a.final_bytes.size() && i < b.final_bytes.size() && a.final_bytes[i] == b.final_bytes[i]) i++;
        snprintf(buf, sizeof buf, "final-state byte #%zu differs (original %02X candidate %02X)", i,
                 i < a.final_bytes.size() ? a.final_bytes[i] : 0, i < b.final_bytes.size() ? b.final_bytes[i] : 0);
        return buf;
    }
    /* both sides took the same page-0 DSI (same kind and address): the console crashed inside the body, so there is no exit and the nonvolatile
     * registers at the fault (the original's body values vs the candidate's not-yet-restored caller values) are not
     * observable, like the dying frames below; framexact call sites before the fault still compare */
    if (g_framexact_funcs.count(g_cur_func) && !a.stopped &&
        !(a.dsi && b.dsi && a.dsi_kind == b.dsi_kind && a.dsi_ea == b.dsi_ea))
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
    /* both sides took the same page-0 DSI (2026-10-26, AGL#4): the console crashed there, so the dying stack frames (the tested
     * function's and its callees', [stack_lo, entry SP); under `footprint` the function's own frame compares otherwise) and the
     * linkage words at entry SP (back chain, LR save word: the original's prologue stores LR there before its body faults, the
     * candidate only at its first guest call) are not observable; the caller's frame from entry SP + 8 (parameter area) on and
     * all other memory still compare. GHS schedules frame stores differently around the faulting access */
    NetMap dna, dnb;
    const NetMap* na = &a.net;
    const NetMap* nb = &b.net;
    if (a.dsi && b.dsi) {
        uint32_t lo = a.entry_sp - 0x10000u, hi = a.entry_sp + 8; /* run(): V.stack_lo = entry SP - 0x10000 */
        for (auto& kv : a.net) if (kv.first - lo >= hi - lo) dna.insert(kv);
        for (auto& kv : b.net) if (kv.first - lo >= hi - lo) dnb.insert(kv);
        na = &dna;
        nb = &dnb;
    }
    if (!same_net(*na, *nb, &wdiff, a.fwords, b.fwords, a.dwords, b.dwords)) {
        uint32_t ea = wdiff;
        for (uint32_t k = 0; k < 4; k++) {
            auto fa = na->find(wdiff + k), fb = nb->find(wdiff + k);
            bool sa = fa != na->end(), sb = fb != nb->end();
            if (sa != sb || (sa && fa->second != fb->second)) { ea = wdiff + k; break; }
        }
        auto fa = na->find(ea), fb = nb->find(ea);
        snprintf(buf, sizeof buf, "memory %08X: original %s, candidate %s (%zu vs %zu bytes changed)", ea,
                 fa == na->end() ? "unchanged" : hex32(fa->second).substr(6).c_str(),
                 fb == nb->end() ? "unchanged" : hex32(fb->second).substr(6).c_str(), na->size(), nb->size());
        return buf;
    }
    (void)in;
    return "";
}

/* a mock model rejected a call on one or both sides. Both results carry the calls up to
 * the rejection (a top-level rejected call is the last one). "" = nothing to report beyond the old abort handling.
 * One side rejected, the other ran on (not aborted): the calls up to and including the rejected one are compared; a
 * difference there is reported as usual (e.g. `call #2 C00060D0 ...: r3 ...`), else the rejection itself is the difference:
 * `call #K TARGET: argument outside the modelled domain on the candidate side only (WHY)`. Both sides rejected: the common
 * call prefix is compared; a difference there is reported, else the input counts as aborted on both sides as before. */
static std::string model_compare(const Result& a, const Result& b, const Input& in) {
    if (!a.mrej && !b.mrej) return "";
    if (a.mrej != b.mrej && (a.mrej ? b.aborted : a.aborted)) return ""; /* the other side aborted for another reason */
    if (a.mrej && b.mrej) {
        g_cmp_prefix = true;
        std::string d = compare_full(a, b, gabi::RET_VOID, in);
        g_cmp_prefix = false;
        return d.empty() ? d : d + " (before mock-model rejections on both sides)";
    }
    const Result& r = a.mrej ? a : b;
    const Result& o = a.mrej ? b : a;
    const char* side = a.mrej ? "original" : "candidate";
    char buf[512];
    if (o.calls.size() < r.calls.size()) {
        snprintf(buf, sizeof buf, "call count: the %s made %zu call(s) up to the one its mock model rejected (%s), the %s %zu",
                 side, r.calls.size(), r.why.c_str(), a.mrej ? "candidate" : "original", o.calls.size());
        return buf;
    }
    g_cmp_prefix = true;
    std::string d = compare_full(a, b, gabi::RET_VOID, in);
    g_cmp_prefix = false;
    if (!d.empty()) return d;
    size_t k = r.calls.empty() ? 0 : r.calls.size() - 1;
    if (r.mrej_depth == 0)
        snprintf(buf, sizeof buf, "call #%zu %s: argument outside the modelled domain on the %s side only (%s)", k,
                 callee_name(r.mrej_target).c_str(), side, r.why.c_str());
    else
        snprintf(buf, sizeof buf, "call #%zu %s: nested call %s (depth %d in %08X) has an argument outside the modelled domain on the "
                 "%s side only (%s)", k, r.calls.empty() ? "-" : callee_name(r.calls.back().target).c_str(),
                 callee_name(r.mrej_target).c_str(), r.mrej_depth, r.mrej_in, side, r.why.c_str());
    return buf;
}

static std::string compare(const Result& a, const Result& b, gabi::RetKind rk, const Input& in) {
    std::string d = compare_full(a, b, rk, in);
    if (a.dsi && b.dsi && (a.dsi_kind != b.dsi_kind || a.dsi_ea != b.dsi_ea)) {
        if (!d.empty()) {
            char w[160];
            snprintf(w, sizeof w, " (before the page-0 fault; the sides faulted at different accesses: orig %s %X, cand %s %X)",
                     dsi_what(a.dsi_kind), a.dsi_ea, dsi_what(b.dsi_kind), b.dsi_ea);
            d += w;
        } else
            g_dsi_order++;
    }
    return d;
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

/* ------------------------------------------------------------------ mutant schemata and reachability */
/* mutate.py (2026-10-26): src/mutschema.h sites call WWHD_MUT(K), which is true for the live mutant
 * (wwhd_mut_active, env WWHD_MUT_ACTIVE) and, while recording (env WWHD_MUT_REC=1), notes that site K ran.
 * -mutrec FILE writes the notes per phase: "G k,..." (static initialisers and start-up), "S ADDR k,..." (the
 * function's setup: setupsnapshot capture and the dictionary pre-runs), "I ADDR INDEX k,..." (one input; generated
 * inputs 0..n-1, recorded inputs n+k in file order). Plain C storage: sites can run during static initialisation. */
extern "C" {
int wwhd_mut_active = -1;
int wwhd_mut_recording = 0;
static int* g_mut_hits;
static size_t g_mut_nhits, g_mut_caphits;
static uint8_t* g_mut_seen;
static size_t g_mut_nseen;
void wwhd_mut_hit(int k) {
    if (k < 0) return;
    if ((size_t)k >= g_mut_nseen) {
        size_t n = (size_t)k + 1 + (size_t)k / 2 + 64;
        g_mut_seen = (uint8_t*)realloc(g_mut_seen, n);
        memset(g_mut_seen + g_mut_nseen, 0, n - g_mut_nseen);
        g_mut_nseen = n;
    }
    if (g_mut_seen[k]) return;
    g_mut_seen[k] = 1;
    if (g_mut_nhits == g_mut_caphits) {
        g_mut_caphits = g_mut_caphits ? 2 * g_mut_caphits : 256;
        g_mut_hits = (int*)realloc(g_mut_hits, g_mut_caphits * sizeof(int));
    }
    g_mut_hits[g_mut_nhits++] = k;
}
}
__attribute__((constructor(101))) static void mut_env() {
    if (const char* e = getenv("WWHD_MUT_ACTIVE")) wwhd_mut_active = atoi(e);
    if (const char* e = getenv("WWHD_MUT_REC")) wwhd_mut_recording = *e == '1';
}
static FILE* g_mutrec;
static void mut_flush(const char* head) {
    if (g_mutrec && g_mut_nhits) {
        fputs(head, g_mutrec);
        for (size_t i = 0; i < g_mut_nhits; i++) fprintf(g_mutrec, i ? ",%d" : " %d", g_mut_hits[i]);
        fputc('\n', g_mutrec);
    }
    for (size_t i = 0; i < g_mut_nhits; i++) g_mut_seen[g_mut_hits[i]] = 0;
    g_mut_nhits = 0;
}
/* -runlist FILE: lines "ADDR all" or "ADDR i,j,..." (input indices as in -mutrec): only those functions and inputs
 * run (mutate.py --reach-skip: the inputs that reach a mutant's site); a row passes when every input it ran passed */
struct RunSel { bool all = false; std::set<long> idx; };
static std::map<uint32_t, RunSel> g_runlist;
static bool g_have_runlist = false;
static void load_runlist(const char* path) {
    FILE* f = fopen(path, "r");
    if (!f) { fprintf(stderr, "cannot read -runlist %s\n", path); exit(2); }
    g_have_runlist = true;
    char* line = nullptr; /* any length (2026-10-27: a -dsiout line lists up to every input index) */
    size_t cap = 0;
    while (getline(&line, &cap, f) > 0) {
        char* e;
        uint32_t a = (uint32_t)strtoul(line, &e, 16);
        if (e == line) continue;
        RunSel& r = g_runlist[a];
        while (*e == ' ') e++;
        if (!strncmp(e, "all", 3)) { r.all = true; continue; }
        while (*e && *e != '\n') {
            char* e2;
            long v = strtol(e, &e2, 10);
            if (e2 == e) break;
            r.idx.insert(v);
            e = *e2 == ',' ? e2 + 1 : e2;
        }
    }
    free(line);
    fclose(f);
}

/* ------------------------------------------------------------------ main */
/* a host crash inside the code under test (e.g. a candidate dereferencing a host pointer): name the function and
 * the side before dying, instead of exiting silently with rc 139 */
static bool is_time_limit(const std::string& w) { /* an abort by the CPU time budget or the wall-clock hang limit */
    return !w.compare(0, 20, "time budget exceeded") || !w.compare(0, 25, "wall-clock limit exceeded");
}
static bool is_env_limit(const std::string& w) { /* ... or by the host memory budget: all depend on the machine and the
                                                * process, not on the input (never cached / snapshotted as a result) */
    return is_time_limit(w) || w.find("host memory budget exceeded (RSS") != std::string::npos; /* also appended */
}

static void crash_handler(int sig, siginfo_t* si, void* uc) {
    char buf[640];
    int n = snprintf(buf, sizeof buf, "\nverify: host signal %d while running %s side of %08X (g_cur_func); input aborted\n",
                     sig, V.is_candidate ? "the candidate" : "the original", g_cur_func);
    if (n > 0) { ssize_t w = write(2, buf, (size_t)n); (void)w; }
    /* 2026-10-29: guest memory is not mapped at PPC_MEM_BASE (every guest access goes
     * through the harness's hooks), so a fault inside the guest window, or below 4 GB, is candidate code that read or
     * wrote guest memory directly: a raw member or array (`u8 x[N]`, `u32 y`) of a guest object instead of a be<> view,
     * or a raw host pointer into guest memory. Name the guest address and the host code address (atos line) */
    const uintptr_t fa = (uintptr_t)(si ? si->si_addr : nullptr);
    uintptr_t pc = 0;
#if defined(__aarch64__)
    if (uc) pc = (uintptr_t)((ucontext_t*)uc)->uc_mcontext->__ss.__pc;
#elif defined(__x86_64__)
    if (uc) pc = (uintptr_t)((ucontext_t*)uc)->uc_mcontext->__ss.__rip;
#endif
    const bool window = fa - (uintptr_t)PPC_MEM_BASE < (1ull << 32);
    if (window || (fa && fa < (1ull << 32))) {
        Dl_info di{};
        bool have = pc && dladdr((void*)pc, &di);
        n = snprintf(buf, sizeof buf, "verify: the fault address %p is %sguest %08X: a direct host access to guest memory "
                     "(a raw member or array of a guest object, e.g. `u8 x[N]`, read or written without a be<> view, or a raw "
                     "host pointer into guest memory); host pc %p%s%s%s",
                     (void*)fa, window ? "" : "NULL-based ", (unsigned)(window ? fa - (uintptr_t)PPC_MEM_BASE : fa), (void*)pc,
                     have && di.dli_sname ? " (" : "", have && di.dli_sname ? di.dli_sname : "", have && di.dli_sname ? ")" : "");
        if (n > 0) { ssize_t w = write(2, buf, (size_t)std::min(n, (int)sizeof buf - 1)); (void)w; }
        if (have && di.dli_fname) {
            n = snprintf(buf, sizeof buf, "; source line: atos -o %s -l %p %p", di.dli_fname, di.dli_fbase, (void*)pc);
            if (n > 0) { ssize_t w = write(2, buf, (size_t)std::min(n, (int)sizeof buf - 1)); (void)w; }
        }
        ssize_t w = write(2, "\n", 1); (void)w;
    }
    signal(sig, SIG_DFL);
    raise(sig);
}

/* gabi.h GABI_GUEST_GUARD (2026-10-27, AGL#2): a be<>/gptr<> accessed through a host object (a host local
 * `gptr<T> x; x.v = a;`, a host static or heap object). ea() would map its host address to an arbitrary guest location
 * (host address - PPC_MEM_BASE, 32 bits) and the store would land there unseen unless that location is compared.
 * gabi.h calls this only for a `this` outside its fast ranges (guest_ref). Such an address is either a host object
 * (inside this thread's stack, a malloc block or the verify executable's own segments: never an input-dependent fault of the
 * original, the candidate's source is wrong: a diagnostic names the function under test and the offending host code
 * address, atos -o EXE -l BASE PC names the source line; inside a candidate run the input ends as a candidate-only abort,
 * a FAIL row, since 2026-10-27; elsewhere the process exits with status 86) or address arithmetic from a NULL guest
 * pointer with a large index (at<T>(0)[i]: host address i * sizeof(T), wrapped to 32 bits by ea() like the console's
 * 0 + i * size; c_bg_s GetTriGrp/GetGrpInf, c_bg_w MakeBlckMinMax, JASDSPInterface, JASOuterParam): returns. */
extern "C" [[gnu::cold, gnu::noinline]] void gmem_host_object(const void* self, unsigned size) {
    const uintptr_t a = (uintptr_t)self;
    pthread_t me = pthread_self();
    const uintptr_t top = (uintptr_t)pthread_get_stackaddr_np(me), ssz = pthread_get_stacksize_np(me);
    const char* what = nullptr;
    Dl_info img{};
    if (a < top && a >= top - ssz) what = "this thread's host stack (a host local)";
    else if (malloc_zone_from_ptr(self)) what = "a host heap block (new/malloc)";
    else if (dladdr(self, &img) && img.dli_fbase == (const void*)_dyld_get_image_header(0))
        what = "the verify executable's data (a host static)"; /* not the dyld shared cache, which dladdr also names */
    if (!what) return; /* NULL-based guest address arithmetic: ea() wraps it like the console */
    void* pc = __builtin_extract_return_addr(__builtin_return_address(0));
    Dl_info di{};
    bool have = dladdr(pc, &di) != 0;
    /* during a candidate run: end that input as a candidate-only abort, a FAIL row whose first
     * difference names the guard, and go on. Exiting here (status 86) lost the rows of every later function, and in a
     * sharded run one shard's results (NW-SND#6 SoundPlayer::remoteVolume 02897688: whether an input's address falls in a
     * host static depends on the load address, so only some runs and shards died). The diagnostic prints once per function */
    static uint32_t s_reported_func = 0;
    const bool in_run = V.can_abort && V.is_candidate && !g_setup_what;
    if (in_run && s_reported_func == g_cur_func) {
        V.abort_reason = std::string("host-local be<>/gptr<> access (guard: ") + what + "; see the gabi: lines on stderr)";
        longjmp(V.abort_jmp, 1);
    }
    fflush(stdout);
    fprintf(stderr, "\ngabi: host-local be<>/gptr<>: be<%u bytes> at host address %p lies in %s, not in guest memory "
            "(ea() would access guest %08X); a guest field/pointer object must live in guest memory "
            "(at<T>(), gabi::Local/FrameLocal, guest statics), keep host temporaries as plain u32/T* values\n"
            "gabi: in function %08X (WWHD_FUNC/VERIFY under test), %s side, seed %llu, depth %d; host pc %p",
            size, self, what, (unsigned)(a - (uintptr_t)PPC_MEM_BASE), g_cur_func,
            V.is_candidate ? "candidate" : "original", (unsigned long long)V.seed, V.depth, pc);
    if (have && di.dli_sname) fprintf(stderr, " (%s+0x%lx)", di.dli_sname, (unsigned long)((uintptr_t)pc - (uintptr_t)di.dli_saddr));
    if (have && di.dli_fname) fprintf(stderr, "; source line: atos -o %s -l %p %p", di.dli_fname, di.dli_fbase, pc);
    fprintf(stderr, "\n");
    fflush(stderr);
    if (in_run) {
        s_reported_func = g_cur_func;
        V.abort_reason = std::string("host-local be<>/gptr<> access (guard: ") + what + "; see the gabi: lines on stderr)";
        longjmp(V.abort_jmp, 1);
    }
    _exit(86); /* outside a candidate run (static initialisers, setups, the original): no row can carry it */
}

/* gabi.h GABI_GUEST_GUARD=2: host-index wrap diagnostic. A be<>/gptr<>
 * access whose host address is neither in the guest window nor NULL-based below 4 GB reaches its guest address only
 * through ea()'s 32-bit wrap: a u32 index on a >= 2-byte element (keys[count - 1] with count 0: host +8 GB for u16),
 * a negative int index below the window, or a NULL-based index past 4 GB. The console computes the same EA, so results
 * are right, but outside guest_ref's fast range (fast = 0: NULL-based past 4 GB, or more than 1 TB away) the guard asks
 * where the host address lies, and whether that is a host object depends on the load address: a false guard trip.
 * Fix: guest EA arithmetic (sead::elementAt, at<T>(base + i * size)). Reported once per function and host pc on stderr
 * (`guardstrict: ...`, with an atos line; a second time when a later access at that site is outside the fast range),
 * counted per function in the closing `guardstrict:` summary; the access then
 * proceeds exactly as with GABI_GUEST_GUARD=1, so rows are unchanged. */
struct WrapSite { uint64_t n = 0; int fast = 1; uint32_t ea = 0; std::string side; };
static std::map<std::pair<uint32_t, uintptr_t>, WrapSite> g_wrap_sites;
static void wrap_summary() {
    if (g_wrap_sites.empty()) { fprintf(stderr, "guardstrict: no host-wrapped be<>/gptr<> access\n"); return; }
    std::map<uint32_t, std::pair<uint64_t, int>> per; /* function -> (accesses, sites) */
    bool slow = false;
    for (auto& kv : g_wrap_sites) {
        auto& x = per[kv.first.first];
        x.first += kv.second.n;
        x.second++;
        slow |= !kv.second.fast;
    }
    for (auto& kv : per)
        fprintf(stderr, "guardstrict: %08X: %llu host-wrapped access(es) at %d site(s)\n", kv.first,
                (unsigned long long)kv.second.first, kv.second.second);
    fprintf(stderr, "guardstrict: %zu function(s), %zu site(s)%s\n", per.size(), g_wrap_sites.size(),
            slow ? "; some outside the guard's fast range (load-address dependent)" : "");
}
/* where a wrapped host address lies. "index wrap" (>= 1 MB past the window) and the NULL-based kinds are the source
 * pattern; "top/bottom wrap" is a small offset past the 32-bit space from a pointer near 0xFFFFFFFF or 0 (usually a
 * generated input pointer plus a member offset, which the console wraps the same way) */
static const char* wrap_kind(uintptr_t a) {
    const uintptr_t b = (uintptr_t)PPC_MEM_BASE;
    if (a >= (1ull << 63)) return "NULL-based negative index";
    if (a < b / 2) return "NULL-based past 4 GB";
    if (a >= b + (1ull << 32) && a < b + (1ull << 32) + (1u << 20)) return "top wrap: small offset past EA FFFFFFFF";
    if (a < b && a >= b - (1u << 20)) return "bottom wrap: small negative offset below EA 0";
    return a >= b ? "index wrap above the guest window" : "index wrap below the guest window";
}
extern "C" [[gnu::cold, gnu::noinline]] void gmem_host_wrap(const void* self, unsigned size, int fast) {
    uintptr_t pc = (uintptr_t)__builtin_extract_return_addr(__builtin_return_address(0));
    WrapSite& w = g_wrap_sites[{g_cur_func, pc}];
    /* once per site, and once more when a site first seen inside the fast range has an occurrence outside it (the
     * load-address-dependent kind: a NULL-based index wrap whose size depends on the input) */
    if (w.n++ && (fast || !w.fast)) return;
    const uintptr_t a = (uintptr_t)self;
    w.fast = fast;
    w.ea = (uint32_t)(a - (uintptr_t)PPC_MEM_BASE);
    w.side = V.is_candidate ? "candidate" : "original";
    Dl_info di{};
    bool have = dladdr((void*)pc, &di) != 0;
    fflush(stdout);
    fprintf(stderr, "guardstrict: %08X %s side: be<%u bytes> at host %p (guest %08X by the 32-bit wrap: %s%s), seed %llu, "
            "depth %d; host pc %p", g_cur_func, w.side.c_str(), size, self, w.ea,
            wrap_kind(a),
            fast ? "" : "; outside the guard's fast range: may trip the guard depending on the load address",
            (unsigned long long)V.seed, V.depth, (void*)pc);
    if (have && di.dli_sname) fprintf(stderr, " (%s+0x%lx)", di.dli_sname, (unsigned long)(pc - (uintptr_t)di.dli_saddr));
    if (have && di.dli_fname) fprintf(stderr, "; source line: atos -o %s -l %p %p", di.dli_fname, di.dli_fbase, (void*)pc);
    fprintf(stderr, "\n");
}

/* ------------------------------------------------------------------ -refcache (mutate.py, 2026-10-28) */
/* The reference side once per input set. Every mutant run of a campaign runs the same original (unit.c, the real
 * callees, the mocks) on the same generated inputs; only the candidate differs. The campaign's unmutated baseline run
 * (-refcache-write DIR) stores, per function and generated input, everything the original's run leaves for the rest of
 * the input: the Result (exit registers, the call records with every observed span, the net memory effect, float
 * marks, `final` bytes, abort/stop/DSI state, strays, traps), the stack-guard bytes (V.orig_stack), the coverage
 * counts it added and the semantic-effect statistics it counted. A mutant run with -refcache DIR loads that record in
 * place of running the original; everything after (the candidate's run, the comparison, the row) is unchanged. A
 * file is written only for a function whose generated inputs all ran (no -runlist subset, no -range, not cut short);
 * an input on which the original hit a time limit (load dependent) is stored as "run it live". The header carries the
 * configuration (-refcache-key from mutate.py: objects, spec, options; plus n, seed, function, block count, the
 * comparison switches) and a reader that finds any difference runs that function live. Recorded (.tap) inputs always
 * run live. -refcache-check runs the original anyway and exits 97 when the live result differs from the record. */
namespace refc {
enum Mode { OFF, WRITE, READ };
static Mode mode = OFF;
static bool check = false;
static std::string dir, key;
static const uint32_t kMagic = 0x52464334u; /* RFC4: zero-run encoded records, dictionary in the header; model rejections */

struct Buf {
    std::string s;
    void raw(const void* p, size_t n) { s.append((const char*)p, n); }
    template <class T> void pod(const T& v) { raw(&v, sizeof v); }
    void vec8(const std::vector<uint8_t>& v) { pod((uint64_t)v.size()); raw(v.data(), v.size()); }
    void vec32(const std::vector<uint32_t>& v) { pod((uint64_t)v.size()); raw(v.data(), v.size() * 4); }
    void str(const std::string& v) { pod((uint64_t)v.size()); raw(v.data(), v.size()); }
};
struct Rd {
    const char* p; const char* e; bool bad = false;
    void raw(void* d, size_t n) { if ((size_t)(e - p) < n) { bad = true; memset(d, 0, n); return; } memcpy(d, p, n); p += n; }
    template <class T> T pod() { T v; raw(&v, sizeof v); return v; }
    uint64_t len(size_t unit) { uint64_t n = pod<uint64_t>(); if (bad || n > (uint64_t)(e - p) / unit) { bad = true; return 0; } return n; }
    void vec8(std::vector<uint8_t>& v) { uint64_t n = len(1); v.resize(n); raw(v.data(), n); }
    void vec32(std::vector<uint32_t>& v) { uint64_t n = len(4); v.resize(n); raw(v.data(), n * 4); }
    void str(std::string& v) { uint64_t n = len(1); v.assign(p, n); p += n; }
};

/* records are zero-run encoded (most of a record is zero: exit FPRs, unused argument slots): a token byte T < 0x80 is
 * followed by T+1 literal bytes, T >= 0x80 stands for T-0x7F zero bytes */
static void zenc(const std::string& in, std::string& out) {
    size_t i = 0, n = in.size();
    while (i < n) {
        size_t z = i;
        while (z < n && z - i < 128 && in[z] == 0) z++;
        if (z - i >= 2) { out.push_back((char)(0x7F + (z - i))); i = z; continue; }
        size_t j = i;
        while (j < n && j - i < 128 && !(in[j] == 0 && j + 1 < n && in[j + 1] == 0)) j++;
        out.push_back((char)(j - i - 1));
        out.append(in, i, j - i);
        i = j;
    }
}
static bool zdec(const char* p, const char* e, std::string& out) {
    out.clear();
    while (p < e) {
        uint8_t t = (uint8_t)*p++;
        if (t >= 0x80) { out.append((size_t)t - 0x7F, '\0'); continue; }
        if ((size_t)(e - p) < (size_t)t + 1) return false;
        out.append(p, (size_t)t + 1);
        p += t + 1;
    }
    return true;
}

static void put_call(Buf& b, const CallEv& c) {
    b.pod(c.target); b.pod(c.kind); b.raw(c.r, sizeof c.r); b.raw(c.f, sizeof c.f); b.pod(c.nint); b.pod(c.nflt);
    b.pod(c.sp); b.pod(c.lr); b.pod(c.notarg);
    uint8_t has = c.extra_ ? 1 : 0;
    b.pod(has);
    if (!has) return;
    const CallEv::Extra& x = *c.extra_;
    b.vec32(x.stk);
    for (auto& v : x.stkref) b.vec8(v);
    for (auto& v : x.sref) b.vec8(v);
    for (auto& v : x.href) b.vec8(v);
    for (auto& v : x.sflt) b.vec8(v);
    b.pod((uint64_t)x.observed.size());
    for (auto& o : x.observed) { b.pod(o.first); b.vec8(o.second); }
    b.pod((uint64_t)x.sobserved.size());
    for (auto& o : x.sobserved) { b.pod(o.first); b.vec8(o.second); }
    b.pod(x.crmask); b.pod(x.crbits); b.pod(x.fx); b.pod(x.lr); b.raw(x.nv, sizeof x.nv); b.raw(x.fnv, sizeof x.fnv);
    b.vec8(x.frame);
    b.raw(x.pbase, sizeof x.pbase); b.raw(x.psize, sizeof x.psize); b.raw(x.pfrom, sizeof x.pfrom); b.raw(x.pkind, sizeof x.pkind);
    for (auto& v : x.pref) b.vec8(v);
    b.pod((uint64_t)x.nested.size());
    for (auto& o : x.nested) { b.pod(o.first); b.vec8(o.second); }
}
static void get_call(Rd& r, CallEv& c) {
    c.target = r.pod<uint32_t>(); c.kind = r.pod<int>(); r.raw(c.r, sizeof c.r); r.raw(c.f, sizeof c.f);
    c.nint = r.pod<int>(); c.nflt = r.pod<int>(); c.sp = r.pod<uint32_t>(); c.lr = r.pod<uint32_t>(); c.notarg = r.pod<uint32_t>();
    if (!r.pod<uint8_t>()) return;
    CallEv::Extra& x = c.ex();
    r.vec32(x.stk);
    for (auto& v : x.stkref) r.vec8(v);
    for (auto& v : x.sref) r.vec8(v);
    for (auto& v : x.href) r.vec8(v);
    for (auto& v : x.sflt) r.vec8(v);
    for (uint64_t n = r.len(12), k = 0; k < n && !r.bad; k++) { x.observed.emplace_back(); x.observed.back().first = r.pod<uint32_t>(); r.vec8(x.observed.back().second); }
    for (uint64_t n = r.len(12), k = 0; k < n && !r.bad; k++) { x.sobserved.emplace_back(); x.sobserved.back().first = r.pod<uint32_t>(); r.vec8(x.sobserved.back().second); }
    x.crmask = r.pod<uint32_t>(); x.crbits = r.pod<uint32_t>(); x.fx = r.pod<bool>(); x.lr = r.pod<uint32_t>();
    r.raw(x.nv, sizeof x.nv); r.raw(x.fnv, sizeof x.fnv);
    r.vec8(x.frame);
    r.raw(x.pbase, sizeof x.pbase); r.raw(x.psize, sizeof x.psize); r.raw(x.pfrom, sizeof x.pfrom); r.raw(x.pkind, sizeof x.pkind);
    for (auto& v : x.pref) r.vec8(v);
    for (uint64_t n = r.len(12), k = 0; k < n && !r.bad; k++) { x.nested.emplace_back(); x.nested.back().first = r.pod<uint32_t>(); r.vec8(x.nested.back().second); }
}

/* the statistics the original side counts (stderr `semantic effects` lines) */
struct Stats {
    uint64_t v[6];
    std::vector<uint64_t> utf16;
    void take() {
        v[0] = g_set_calls; v[1] = g_set_bytes; v[2] = g_surface_calls; v[3] = g_move_calls; v[4] = g_move_bytes; v[5] = g_utf16_fail;
        if (!g_utf16_specs.empty()) utf16.assign(g_utf16_length, g_utf16_length + 4097);
    }
};

/* one input's record: the Result, then the stack-guard bytes, the coverage and statistics the original's run added */
struct Side {
    std::vector<uint32_t> orig_stack; /* sorted */
    std::vector<uint32_t> cov;        /* (block, added count) pairs */
    uint64_t stats[6] = {};
    std::vector<uint32_t> utf16;      /* (length, added count) pairs */
};
static Side side_of_run(const std::vector<uint32_t>& cov0, const std::vector<uint32_t>& cov1, const Stats& s0) {
    Side s;
    s.orig_stack.assign(V.orig_stack.begin(), V.orig_stack.end());
    std::sort(s.orig_stack.begin(), s.orig_stack.end());
    for (size_t k = 0; k < cov1.size(); k++)
        if (cov1[k] != cov0[k]) { s.cov.push_back((uint32_t)k); s.cov.push_back(cov1[k] - cov0[k]); }
    Stats s1; s1.take();
    for (int k = 0; k < 6; k++) s.stats[k] = s1.v[k] - s0.v[k];
    for (size_t k = 0; k < s1.utf16.size() && k < s0.utf16.size(); k++)
        if (s1.utf16[k] != s0.utf16[k]) { s.utf16.push_back((uint32_t)k); s.utf16.push_back((uint32_t)(s1.utf16[k] - s0.utf16[k])); }
    return s;
}
static void put_record(Buf& b, const Result& a, const Side& s) {
    b.pod(a.entry_sp);
    Cpu ex = a.exit;
    ex.thread = nullptr; /* host pointer, not compared */
    b.pod(ex);
    b.pod((uint64_t)a.calls.size());
    for (auto& c : a.calls) put_call(b, c);
    b.pod((uint64_t)a.net.size());
    for (auto& kv : a.net) { b.pod(kv.first); b.pod(kv.second); }
    b.vec32(a.dwords); b.vec32(a.fwords); b.vec8(a.final_bytes);
    b.pod(a.aborted); b.pod(a.stopped); b.pod(a.dsi); b.pod(a.dsi_kind); b.pod(a.dsi_ea); b.pod(a.dsi_in); b.pod(a.dsi_depth);
    b.pod(a.mrej); b.pod(a.mrej_target); b.pod(a.mrej_in); b.pod(a.mrej_depth);
    b.str(a.why); b.pod(a.stray); b.pod(a.first_stray); b.pod(a.traps);
    b.vec32(a.badfetch);
    b.vec32(s.orig_stack); b.vec32(s.cov);
    for (int k = 0; k < 6; k++) b.pod(s.stats[k]);
    b.vec32(s.utf16);
}
static bool get_record(Rd& r, Result* a, Side* s, size_t ncov) {
    a->entry_sp = r.pod<uint32_t>();
    a->exit = r.pod<Cpu>();
    for (uint64_t n = r.len(16), k = 0; k < n && !r.bad; k++) { a->calls.emplace_back(); get_call(r, a->calls.back()); }
    for (uint64_t n = r.len(5), k = 0; k < n && !r.bad; k++) { uint32_t ad = r.pod<uint32_t>(); uint8_t v = r.pod<uint8_t>(); a->net.emplace_hint(a->net.end(), ad, v); }
    r.vec32(a->dwords); r.vec32(a->fwords); r.vec8(a->final_bytes);
    a->aborted = r.pod<bool>(); a->stopped = r.pod<bool>(); a->dsi = r.pod<bool>(); a->dsi_kind = r.pod<char>();
    a->dsi_ea = r.pod<uint32_t>(); a->dsi_in = r.pod<uint32_t>(); a->dsi_depth = r.pod<int>();
    a->mrej = r.pod<bool>(); a->mrej_target = r.pod<uint32_t>(); a->mrej_in = r.pod<uint32_t>(); a->mrej_depth = r.pod<int>();
    r.str(a->why); a->stray = r.pod<uint32_t>(); a->first_stray = r.pod<uint32_t>(); a->traps = r.pod<int>();
    r.vec32(a->badfetch);
    r.vec32(s->orig_stack); r.vec32(s->cov);
    for (int k = 0; k < 6; k++) s->stats[k] = r.pod<uint64_t>();
    r.vec32(s->utf16);
    if (r.bad || s->cov.size() % 2 || s->utf16.size() % 2) return false;
    for (size_t k = 0; k < s->cov.size(); k += 2) if (s->cov[k] >= ncov) return false;
    for (size_t k = 0; k < s->utf16.size(); k += 2) if (s->utf16[k] > 4096) return false;
    return true;
}
/* leave the state the original's run leaves (run() and the statistics counters): the stack guard, the call sequence
 * and return addresses the candidate's LR rule reads, coverage, statistics */
static void apply(const Result& a, const Side& s, std::vector<uint32_t>& cov) {
    V.orig_stack.clear();
    V.orig_stack.insert(s.orig_stack.begin(), s.orig_stack.end());
    V.orig_seq.clear();
    V.orig_lr.clear();
    for (auto& e : a.calls) { V.orig_seq.push_back({e.target, e.kind}); V.orig_lr.push_back(e.lr); }
    for (size_t k = 0; k < s.cov.size(); k += 2) cov[s.cov[k]] += s.cov[k + 1];
    g_set_calls += s.stats[0]; g_set_bytes += s.stats[1]; g_surface_calls += s.stats[2];
    g_move_calls += s.stats[3]; g_move_bytes += s.stats[4]; g_utf16_fail += s.stats[5];
    for (size_t k = 0; k < s.utf16.size(); k += 2) g_utf16_length[s.utf16[k]] += s.utf16[k + 1];
}

/* ---- digest records (-refcache-digest, 2026-10-31, fasttools2 change 4) ----
 * A full record holds the original's whole Result: per input the net memory effect and every call with its observed
 * spans, up to 1-7 GB per function at n10000 (AGL#19 banks/commands/shader), more than WWHD_REFCACHE_MAX_MB, so those
 * functions ran their original live in every mutant run. A digest record holds only what the candidate's run needs
 * (each top-level call's target, kind and return address, V.orig_stack, coverage, statistics) and what the input's
 * outcome reads besides the comparison (exit registers, abort/stop/DSI/model-rejection state, `why`, strays, traps, bad
 * fetch targets), exactly, plus a 128-bit hash of the rest (`heavy`: every call's compared parts under compare_full's
 * argument masks and the original's stack-argument word counts, the net memory effect, the `final` bytes) and those
 * masks and counts. Reading it (mutant run):
 *   - neither side mock-rejected and at least one side aborted: the outcome reads only the exact part (both aborted:
 *     counted as aborted on both sides; one side: `only one side aborted: WHY`);
 *   - otherwise, when the masks the candidate's calls give equal the recorded ones and the candidate's heavy part hashes
 *     to the recorded hash, the original's heavy part IS the candidate's (up to a hash collision): the comparison runs
 *     on the exact part plus the candidate's heavy part and gives what it gives on the original's;
 *   - otherwise (a heavy difference, other masks, a mock-model rejection on either side, -v -v -v) the original runs
 *     live after the candidate (its coverage and statistics already came from the record and are not counted twice) and
 *     the comparison is the full one.
 * put_heavy maps equal bytes to passing checks only (it is stricter than compare_full: stack-object pointers relative,
 * NaN in compared FPRs as one token; everything else raw), so a hash match never hides a difference; a stricter
 * mismatch only costs a live run. -refcache-check runs the original live first and exits 97 when its exact part, side
 * or heavy hash (under the recorded masks) differs from the record. */
static uint64_t dg_hash(const void* p, size_t n, uint64_t h) { /* as snapc::hash64 */
    const uint8_t* bb = (const uint8_t*)p;
    h ^= n * 0x9E3779B97F4A7C15ull;
    size_t i = 0;
    for (; i + 8 <= n; i += 8) {
        uint64_t w;
        memcpy(&w, bb + i, 8);
        h = (h ^ w) * 0xFF51AFD7ED558CCDull;
        h ^= h >> 29;
    }
    for (; i < n; i++) h = (h ^ bb[i]) * 0x100000001B3ull;
    h ^= h >> 31; h *= 0xC4CEB9FE1A85EC53ull; h ^= h >> 33;
    return h;
}
/* compare_full's argument masks for a call: bits 0-7 = r3..r10, bits 8-15 = f1..f8; x = the original's call, y = the
 * candidate's call at the same position (its declared argument counts), nullptr when it made none */
static uint16_t call_mask(const CallEv& x, const CallEv* y) {
    const CalleeInfo* ci = callee_info(x.target);
    uint32_t im = 0, fm = 0;
    if (ci) { im = ci->imask; fm = ci->fmask; }
    if (ci && x.kind == VM_CALL_INDIRECT) im &= ~(x.notarg & 0x7F0u);
    if (y && (!ci || ci->declared)) {
        for (int i = 0; i < y->nint; i++) im |= 1u << (3 + i);
        for (int i = 0; i < y->nflt; i++) fm |= 1u << (1 + i);
    }
    if (!ci && !g_nolivein.count(g_cur_func))
        if (auto li = g_livein.find(x.target); li != g_livein.end()) {
            im |= li->second.first & ~x.notarg & 0x7F0u;
            fm |= li->second.second;
        }
    return (uint16_t)((im >> 3 & 0xFF) | (fm >> 1 & 0xFF) << 8);
}
/* per call: the masks (bits 0-15) and the original's number of stack argument words (bits 16-31: compare_full compares
 * the first min(original, candidate) words, so a candidate may pass more) */
static std::vector<uint32_t> call_masks(const Result& a, const Result& b) {
    std::vector<uint32_t> m(a.calls.size());
    for (size_t k = 0; k < a.calls.size(); k++)
        m[k] = call_mask(a.calls[k], k < b.calls.size() ? &b.calls[k] : nullptr) |
               (uint32_t)std::min<size_t>(a.calls[k].ex().stk.size(), 0xFFFF) << 16;
    return m;
}
/* one call's heavy part; part >= 0 writes only that part (the WWHD_REFCACHE_DIGEST_DEBUG diagnostic): 0 target/sp,
 * 1 observed, 2 sobserved, 3 nested, 4 callcr, 5 framexact, 10+i register ri, 25 stack arguments, 30+i fi */
static void put_heavy_call(Buf& b, const Result& r, const CallEv& c, uint32_t m, int part = -1) {
    const bool spc = g_calleesp_all || footprint_of(g_cur_func_fp) != UINT32_MAX;
    const CallEv::Extra& e = c.ex();
    auto want = [&](int p) { return part < 0 || part == p; };
    if (want(0)) { b.pod(c.target); if (spc) b.pod(c.sp); }
    if (want(1)) { b.pod((uint64_t)e.observed.size()); for (auto& o : e.observed) { b.pod(o.first); b.vec8(o.second); } }
    if (want(2)) { b.pod((uint64_t)e.sobserved.size()); for (auto& o : e.sobserved) { b.pod(o.first); b.vec8(o.second); } }
    if (want(3)) { b.pod((uint64_t)e.nested.size()); for (auto& o : e.nested) { b.pod(o.first); b.vec8(o.second); } }
    if (want(4)) { b.pod(e.crmask); b.pod(e.crmask ? e.crbits : 0u); }
    if (want(5)) {
        b.pod((uint8_t)e.fx);
        if (e.fx) { b.pod(e.lr); b.raw(e.nv, sizeof e.nv); b.raw(e.fnv, sizeof e.fnv); b.vec8(e.frame); }
    }
    {
        for (int i = 3; i <= 10; i++) {
            if (!(m >> (i - 3) & 1) || !want(10 + i)) continue;
            const std::vector<uint8_t>& v = e.sref[i];
            if (!v.empty()) { /* a stack object: pointers into the stack relative to the object (same_stack_obj) */
                b.pod((uint8_t)1);
                b.pod((uint64_t)v.size());
                size_t q = 0;
                for (; q + 4 <= v.size(); q += 4) {
                    uint32_t w = (uint32_t)v[q] << 24 | v[q + 1] << 16 | v[q + 2] << 8 | v[q + 3];
                    if (w - (r.entry_sp - 0x10000u) < 0x10000u) { b.pod((uint8_t)1); b.pod((uint32_t)(w - c.r[i])); }
                    else { b.pod((uint8_t)0); b.pod(w); }
                }
                for (; q < v.size(); q++) b.pod(v[q]);
            } else {
                b.pod((uint8_t)0);
                b.pod(c.r[i]);
                b.vec8(e.href[i]);
                if (g_observe_pointees) { b.pod(e.pkind[i]); b.pod(e.pbase[i]); b.pod(e.psize[i]); b.pod(e.pfrom[i]); b.vec8(e.pref[i]); }
            }
        }
        if (want(25)) { /* the first min(original's count, this side's) words: a side with fewer words differs */
            const size_t ns = std::min<size_t>(e.stk.size(), m >> 16);
            b.pod((uint64_t)ns);
            for (size_t j = 0; j < ns; j++) {
                if (j < 4 && !e.stkref[j].empty()) { b.pod((uint8_t)1); b.vec8(e.stkref[j]); }
                else { b.pod((uint8_t)0); b.pod(e.stk[j]); }
            }
        }
        for (int i = 1; i <= 8; i++) {
            if (!(m >> (7 + i) & 1) || !want(30 + i)) continue;
            if (!g_strict_nan && std::isnan(c.f[i])) b.pod((uint8_t)2);
            else { b.pod((uint8_t)0); b.pod(f64_as_u64(c.f[i])); }
        }
    }
}
/* the heavy part of a result under the masks (one per call; r.calls.size() == masks.size()) */
static void put_heavy(Buf& b, const Result& r, const std::vector<uint32_t>& masks) {
    b.pod((uint64_t)r.calls.size());
    for (size_t k = 0; k < r.calls.size(); k++) put_heavy_call(b, r, r.calls[k], masks[k]);
    b.pod((uint64_t)r.net.size());
    for (auto& kv : r.net) { b.pod(kv.first); b.pod(kv.second); }
    b.vec8(r.final_bytes);
}
static void heavy_hash(const Result& r, const std::vector<uint32_t>& masks, uint64_t h[2]) {
    Buf b;
    put_heavy(b, r, masks);
    h[0] = dg_hash(b.s.data(), b.s.size(), 0x1D1D);
    h[1] = dg_hash(b.s.data(), b.s.size(), 0x2E2E5A5A);
}
/* WWHD_REFCACHE_DIGEST_DEBUG=1: where the heavy parts of a and b first differ (the writer, for inputs the unmutated
 * candidate does not match) */
static std::string heavy_diff(const Result& a, const Result& b, const std::vector<uint32_t>& masks) {
    char w[200];
    if (a.calls.size() != b.calls.size()) { snprintf(w, sizeof w, "call count %zu vs %zu", a.calls.size(), b.calls.size()); return w; }
    static const int parts[] = {0, 1, 2, 3, 4, 5, 13, 14, 15, 16, 17, 18, 19, 20, 25, 31, 32, 33, 34, 35, 36, 37, 38};
    static const char* names[] = {"target/sp", "observed", "sobserved", "nested", "callcr", "framexact", "r3", "r4", "r5",
                                  "r6", "r7", "r8", "r9", "r10", "stack args", "f1", "f2", "f3", "f4", "f5", "f6", "f7", "f8"};
    for (size_t k = 0; k < a.calls.size(); k++)
        for (size_t p = 0; p < sizeof parts / sizeof *parts; p++) {
            Buf x, y;
            put_heavy_call(x, a, a.calls[k], masks[k], parts[p]);
            put_heavy_call(y, b, b.calls[k], masks[k], parts[p]);
            if (x.s != y.s) {
                snprintf(w, sizeof w, "call #%zu %08X: %s (%zu vs %zu bytes)", k, a.calls[k].target, names[p], x.s.size(), y.s.size());
                return w;
            }
        }
    if (a.net != b.net) {
        uint32_t at = 0;
        for (auto& kv : a.net) { auto it = b.net.find(kv.first); if (it == b.net.end() || it->second != kv.second) { at = kv.first; break; } }
        snprintf(w, sizeof w, "net effect (%zu vs %zu bytes, first difference at or after %08X)", a.net.size(), b.net.size(), at);
        return w;
    }
    if (a.final_bytes != b.final_bytes) return "final bytes";
    return "none";
}
/* the exact part: everything of the Result but the calls' compared parts, the net effect and the `final` bytes */
static void put_lite(Buf& b, const Result& a) {
    b.pod(a.entry_sp);
    Cpu ex = a.exit;
    ex.thread = nullptr; /* host pointer, not compared */
    b.pod(ex);
    b.pod(a.aborted); b.pod(a.stopped); b.pod(a.dsi); b.pod(a.dsi_kind); b.pod(a.dsi_ea); b.pod(a.dsi_in); b.pod(a.dsi_depth);
    b.pod(a.mrej); b.pod(a.mrej_target); b.pod(a.mrej_in); b.pod(a.mrej_depth);
    b.str(a.why); b.pod(a.stray); b.pod(a.first_stray); b.pod(a.traps);
    b.vec32(a.badfetch);
    b.pod((uint64_t)a.calls.size());
    for (auto& c : a.calls) { b.pod(c.target); b.pod(c.kind); b.pod(c.lr); b.pod(c.notarg); }
}
static void put_side(Buf& b, const Side& s) {
    std::vector<uint32_t> rg; /* V.orig_stack as runs (start, length): mostly whole frames */
    for (size_t k = 0; k < s.orig_stack.size();) {
        size_t j = k + 1;
        while (j < s.orig_stack.size() && s.orig_stack[j] == s.orig_stack[j - 1] + 1) j++;
        rg.push_back(s.orig_stack[k]);
        rg.push_back((uint32_t)(j - k));
        k = j;
    }
    b.vec32(rg); b.vec32(s.cov);
    for (int k = 0; k < 6; k++) b.pod(s.stats[k]);
    b.vec32(s.utf16);
}
static void put_digest(Buf& b, const Result& a, const Side& s, const std::vector<uint32_t>& masks, const uint64_t h[2]) {
    put_lite(b, a);
    b.vec32(masks);
    b.pod(h[0]); b.pod(h[1]);
    put_side(b, s);
}
static bool get_digest(Rd& r, Result* a, Side* s, std::vector<uint32_t>* masks, uint64_t h[2], size_t ncov) {
    a->entry_sp = r.pod<uint32_t>();
    a->exit = r.pod<Cpu>();
    a->aborted = r.pod<bool>(); a->stopped = r.pod<bool>(); a->dsi = r.pod<bool>(); a->dsi_kind = r.pod<char>();
    a->dsi_ea = r.pod<uint32_t>(); a->dsi_in = r.pod<uint32_t>(); a->dsi_depth = r.pod<int>();
    a->mrej = r.pod<bool>(); a->mrej_target = r.pod<uint32_t>(); a->mrej_in = r.pod<uint32_t>(); a->mrej_depth = r.pod<int>();
    r.str(a->why); a->stray = r.pod<uint32_t>(); a->first_stray = r.pod<uint32_t>(); a->traps = r.pod<int>();
    r.vec32(a->badfetch);
    for (uint64_t n = r.len(16), k = 0; k < n && !r.bad; k++) {
        a->calls.emplace_back();
        CallEv& c = a->calls.back();
        c.target = r.pod<uint32_t>(); c.kind = r.pod<int>(); c.lr = r.pod<uint32_t>(); c.notarg = r.pod<uint32_t>();
        memset(c.r, 0, sizeof c.r); memset(c.f, 0, sizeof c.f); c.nint = c.nflt = 0;
    }
    r.vec32(*masks);
    if (masks->size() != a->calls.size()) return false;
    h[0] = r.pod<uint64_t>(); h[1] = r.pod<uint64_t>();
    std::vector<uint32_t> rg;
    r.vec32(rg); r.vec32(s->cov);
    for (int k = 0; k < 6; k++) s->stats[k] = r.pod<uint64_t>();
    r.vec32(s->utf16);
    if (r.bad || rg.size() % 2 || s->cov.size() % 2 || s->utf16.size() % 2) return false;
    s->orig_stack.clear();
    for (size_t k = 0; k < rg.size(); k += 2) {
        if (rg[k + 1] > 0x100000u) return false;
        for (uint32_t j = 0; j < rg[k + 1]; j++) s->orig_stack.push_back(rg[k] + j);
    }
    for (size_t k = 0; k < s->cov.size(); k += 2) if (s->cov[k] >= ncov) return false;
    for (size_t k = 0; k < s->utf16.size(); k += 2) if (s->utf16[k] > 4096) return false;
    return true;
}
static void copy_call(CallEv& d, const CallEv& c) {
    d.target = c.target; d.kind = c.kind; memcpy(d.r, c.r, sizeof d.r); memcpy(d.f, c.f, sizeof d.f);
    d.nint = c.nint; d.nflt = c.nflt; d.sp = c.sp; d.lr = c.lr; d.notarg = c.notarg;
    d.extra_.reset(c.extra_ ? new CallEv::Extra(*c.extra_) : nullptr);
}
/* the statistics counters set back to s (the digest reader's live fallback: the record already counted the original) */
static void restore_stats(const Stats& s) {
    g_set_calls = s.v[0]; g_set_bytes = s.v[1]; g_surface_calls = s.v[2]; g_move_calls = s.v[3]; g_move_bytes = s.v[4];
    g_utf16_fail = s.v[5];
    if (!s.utf16.empty()) memcpy(g_utf16_length, s.utf16.data(), s.utf16.size() * sizeof(uint64_t));
}

/* the configuration a record is valid for */
static const uint32_t kMagicDigest = 0x52464431u; /* RFD1: digest records (-refcache-digest, 2026-10-31) */
static bool digest = false;      /* -refcache-digest: the writer stores digest records (below) */
static bool digest_miss = false; /* -refcache-digest-miss (self-test): every digest comparison counts as a difference */
static std::string header(uint32_t addr, int ngen, uint64_t seed0, uint32_t nblocks, uint32_t magic = kMagic) {
    Buf b;
    b.pod(magic);
    b.str(key);
    b.pod(addr); b.pod(ngen); b.pod(seed0); b.pod(nblocks);
    uint32_t sw = (g_nulltrap ? 1 : 0) | (g_strict_nan ? 2 : 0) | (V.clobber ? 4 : 0) | (g_calleesp_all ? 8 : 0) | (g_footprint_all ? 16 : 0) |
                  (g_observe_pointees ? 32 : 0);
    b.pod(sw);
    b.pod(g_obs_cap);
    b.pod((uint32_t)sizeof(Cpu)); b.pod((uint32_t)sizeof(CallEv));
    /* the function's constant dictionary (built by the pre-runs before begin/open): the generated inputs depend on it,
     * so a reader whose dictionary differs runs the original live instead of comparing against other inputs */
    b.vec32(V.dict);
    return b.s;
}
static std::string path(uint32_t addr) { char n[32]; snprintf(n, sizeof n, "/%08X.ref", addr); return dir + n; }

/* one function's file. Layout: u64 header length, header; the records (u32 length + zero-run encoded seed and record);
 * a u64 offset per input (0 = run it live); the u64 offset of that table. */
struct Writer {
    bool on = false;
    uint32_t addr = 0;
    int ngen = 0;
    Buf body;
    std::vector<uint64_t> off;
    size_t cap = 0;
    int lo = 0, hi = 0; /* the generated inputs this run covers: [0, n) writes ADDR.ref, a -range shard ADDR.ref.LO-HI */
    void begin(uint32_t a, int n, uint64_t seed0, uint32_t nblocks, int glo, int ghi) {
        on = mode == WRITE;
        if (!on) return;
        addr = a; ngen = n; lo = glo; hi = ghi;
        std::string h = header(a, n, seed0, nblocks, digest ? kMagicDigest : kMagic);
        body.s.clear();
        body.pod((uint64_t)h.size());
        body.raw(h.data(), h.size());
        off.assign(n, 0);
        const char* e = getenv("WWHD_REFCACHE_MAX_MB");
        cap = (size_t)(e ? atof(e) : 1024.0) * (1u << 20);
    }
    uint64_t dg_inputs = 0, dg_self = 0; /* -refcache-digest: inputs recorded, and those the unmutated candidate matches */
    void add(int idx, uint64_t seed, bool live, const Result& a, const Side& s, const Result* b = nullptr) {
        if (!on) return;
        if (idx < 0 || idx >= ngen) { on = false; return; }
        if (live) return; /* offset 0: the readers run this input live */
        off[idx] = body.s.size();
        Buf rec;
        rec.pod(seed);
        if (digest) { /* b: the candidate's run of this input (its declared argument counts give the masks) */
            std::vector<uint32_t> m = b ? call_masks(a, *b) : std::vector<uint32_t>(a.calls.size(), 0);
            uint64_t h[2], hb[2] = {0, 0};
            heavy_hash(a, m, h);
            if (b && b->calls.size() == a.calls.size() && !a.mrej && !b->mrej && !(a.aborted || b->aborted)) {
                bool same = true;
                for (size_t k = 0; k < a.calls.size() && same; k++) same = call_mask(a.calls[k], &b->calls[k]) == (m[k] & 0xFFFF);
                if (same) heavy_hash(*b, m, hb);
            }
            dg_inputs++;
            if (hb[0] != h[0] && b && !(a.aborted || b->aborted) && getenv("WWHD_REFCACHE_DIGEST_DEBUG"))
                fprintf(stderr, "refcache: digest debug %08X input %d: the unmutated candidate differs: %s\n", addr, idx,
                        a.mrej || b->mrej ? "model rejection" : b->calls.size() != a.calls.size() ? heavy_diff(a, *b, m).c_str() :
                        [&] { for (size_t k = 0; k < a.calls.size(); k++) if (call_mask(a.calls[k], &b->calls[k]) != (m[k] & 0xFFFF)) return false; return true; }()
                            ? heavy_diff(a, *b, m).c_str() : "masks");
            dg_self += (a.aborted || (b && b->aborted)) && !a.mrej && !(b && b->mrej) ? 1 : (hb[0] == h[0] && hb[1] == h[1]);
            put_digest(rec, a, s, m, h);
        } else
            put_record(rec, a, s);
        std::string enc;
        zenc(rec.s, enc);
        body.pod((uint32_t)enc.size());
        body.raw(enc.data(), enc.size());
        if (body.s.size() > cap) {
            fprintf(stderr, "refcache: %08X: the records pass WWHD_REFCACHE_MAX_MB (%zu MB); this function runs its original live\n",
                    addr, cap >> 20);
            on = false;
            body.s.clear();
            body.s.shrink_to_fit();
        }
    }
    void finish(bool complete) {
        if (!on) return;
        on = false;
        if (!complete) { body.s.clear(); return; }
        uint64_t tab = body.s.size();
        body.raw(off.data(), off.size() * 8);
        body.pod(tab);
        std::string fin = path(addr);
        if (lo != 0 || hi != ngen) fin += "." + std::to_string(lo) + "-" + std::to_string(hi); /* a shard's part: verify.py
            * joins the parts of [0, n) into ADDR.ref (refcache.py); a reader never opens a part */
        std::string tmp = fin + ".tmp" + std::to_string(getpid());
        FILE* f = fopen(tmp.c_str(), "wb");
        bool ok = f && fwrite(body.s.data(), 1, body.s.size(), f) == body.s.size();
        if (f && fclose(f)) ok = false;
        if (!ok || rename(tmp.c_str(), fin.c_str())) {
            fprintf(stderr, "refcache: cannot write %s (%s); mutant runs run this function's original live\n", fin.c_str(), strerror(errno));
            unlink(tmp.c_str());
        }
        body.s.clear();
        body.s.shrink_to_fit();
    }
};

struct Reader {
    bool ok = false;
    bool digest = false; /* the open file holds digest records */
    uint64_t dmatch = 0, dlite = 0, dlive = 0; /* digest inputs: heavy hash matched, outcome from the exact part, live */
    std::string data;
    const uint64_t* off = nullptr;
    int ngen = 0;
    uint64_t hits = 0, live = 0;
    void open(uint32_t addr, int n, uint64_t seed0, uint32_t nblocks) {
        ok = false;
        data.clear();
        if (mode != READ) return;
        FILE* f = fopen(path(addr).c_str(), "rb");
        if (!f) { fprintf(stderr, "refcache: no record for %08X; its original runs live\n", addr); return; }
        fseek(f, 0, SEEK_END);
        long sz = ftell(f);
        fseek(f, 0, SEEK_SET);
        data.resize(sz > 0 ? sz : 0);
        bool rd = sz > 16 && fread(&data[0], 1, sz, f) == (size_t)sz;
        fclose(f);
        std::string h = header(addr, n, seed0, nblocks), hd = header(addr, n, seed0, nblocks, kMagicDigest);
        uint64_t hl, tab;
        if (rd) { memcpy(&hl, data.data(), 8); memcpy(&tab, data.data() + sz - 8, 8); }
        digest = rd && hl == hd.size() && data.compare(8, hl, hd) == 0;
        if (digest) h = hd;
        if (!rd || hl != h.size() || data.compare(8, hl, h) != 0 || tab + (uint64_t)n * 8 + 8 != (uint64_t)sz) {
            fprintf(stderr, "refcache: the record for %08X does not match this run's configuration; its original runs live\n", addr);
            data.clear();
            return;
        }
        off = (const uint64_t*)(data.data() + tab);
        ngen = n;
        ok = true;
    }
    /* the record of input idx, decoded into buf, as a reader positioned after its seed, or false (run live) */
    bool find(int idx, uint64_t seed, Rd* r, std::string& buf) {
        if (!ok || idx < 0 || idx >= ngen) return false;
        uint64_t o;
        memcpy(&o, off + idx, 8);
        if (!o) { live++; return false; }
        uint32_t len;
        const char* end = (const char*)off;
        if (o + 4 > (uint64_t)(end - data.data())) { live++; return false; }
        memcpy(&len, data.data() + o, 4);
        if (o + 4 + len > (uint64_t)(end - data.data()) || !zdec(data.data() + o + 4, data.data() + o + 4 + len, buf)) {
            live++;
            return false;
        }
        r->p = buf.data();
        r->e = buf.data() + buf.size();
        r->bad = false;
        if (r->pod<uint64_t>() != seed || r->bad) { live++; return false; }
        hits++;
        return true;
    }
};
}  // namespace refc

/* ------------------------------------------------------------------ -snapcache */
/* The setup side once per campaign. With `setupsnapshot` every process used to run each function's world setups for real
 * four times or more: the two capture runs (two input seeds) before the inputs, and the first input of the run plus
 * every -snapcheck'th input (validation). On world specs that was ~90% of a mutant run. The setups run only the
 * original's code (unit.c real_X and the harness's mocks; a candidate function is reached only from the top-level
 * call of its own row, mutscope.py), so their result is the same for every mutant of a campaign. The campaign's
 * unmutated baseline (-snapcache-write DIR) stores per function:
 *   ADDR-TAG.snap    (pointing to the content-addressed world-HASH.blob) the captured snapshot (g_snap, the state every non-validation input installs) once the two capture
 *                    runs accepted it (a refused snapshot writes nothing: readers refuse it live, as before);
 *   ADDR-TAG-sSEED.val per validation input (delta-encoded, see Predict) of the run (input index), the complete post-setup state the original side's
 *                    real setup run left (SnapRec: memory with the scratch stack window, run counters, stack-guard bytes,
 *                    statistics), recorded after the run's own check against the snapshot passed.
 * A reading run (-snapcache DIR, mutant runs) takes the snapshot from the file instead of the capture runs, and on a
 * validation input with a record installs it on both sides instead of running the setups (a validation input without
 * a record, e.g. the first input of a -runlist subset, runs them as before). Key: -snapcache-key (mutate.py: sha256 of
 * unit.c/FP runtime/harness/support objects, the expanded spec, the harness options without -n/-seed, the recordings)
 * plus a hash of the guest state after start-up (image, copy/blob bytes, fills, the unit's function and callee tables)
 * and the comparison switches; the .val header adds the input seed, the function's constant dictionary (the generated
 * inputs depend on it) and the hash of the snapshot it was checked against. Any mismatch, a missing or unreadable file:
 * the function runs its setups live (stderr note). Each file carries a hash of its payload. Only with -fastreset (the
 * default), without -trace/-watch/-dsiout/-mutrec/WWHD_VERIFY_FASTRESET_CHECK. -snapcache-check runs everything live
 * and compares: the capture against the cached snapshot, each live validation setup (either side) against its record;
 * a difference, or a file that does not decode, exits 98. */
namespace snapc {
enum Mode { OFF, WRITE, READ };
static Mode mode = OFF;
static bool check = false;
static std::string dir, key;
static uint64_t startup = 0; /* hash of the guest state after start-up */
static const uint32_t kSnapMagic = 0x534E4331u /* SNC1 */, kValMagic = 0x53564132u /* SVA2: delta-encoded records */;
static uint64_t fn_cached = 0, fn_live = 0, val_cached = 0, val_live = 0, wrote_snap = 0, wrote_val = 0;

static uint64_t hash64(const void* p, size_t n, uint64_t h) {
    const uint8_t* b = (const uint8_t*)p;
    h ^= n * 0x9E3779B97F4A7C15ull;
    size_t i = 0;
    for (; i + 8 <= n; i += 8) {
        uint64_t w;
        memcpy(&w, b + i, 8);
        h = (h ^ w) * 0xFF51AFD7ED558CCDull;
        h ^= h >> 29;
    }
    for (; i < n; i++) h = (h ^ b[i]) * 0x100000001B3ull;
    h ^= h >> 31; h *= 0xC4CEB9FE1A85EC53ull; h ^= h >> 33;
    return h;
}
template <class T> static uint64_t hpod(const T& v, uint64_t h) { return hash64(&v, sizeof v, h); }

/* everything the setups can read that is not in the spec text: the loaded image, the copy/blob bytes, the fills, and
 * the unit's tables (which originals exist, which callees run real code, their facts) */
static uint64_t startup_hash() {
    uint64_t h = 0x534E4331ull;
    for (const Region& r : V.image) { h = hpod(r.addr, h); h = hpod(r.size, h); h = hash64(r.data.data(), r.data.size(), h); }
    for (const CopySpec& s : g_copy_specs) {
        h = hpod(s.func, h); h = hpod(s.base_reg, h); h = hpod(s.off, h); h = hpod(s.src, h); h = hpod(s.size, h); h = hpod(s.blob, h);
        h = hash64(s.data.data(), s.data.size(), h);
    }
    for (const FillSpec& s : g_fill_specs) { h = hpod(s.func, h); h = hpod(s.base_reg, h); h = hpod(s.off, h); h = hpod(s.size, h); h = hpod(s.byte, h); }
    for (unsigned i = 0; i < unit_nfuncs; i++) {
        const OrigFunc& f = unit_funcs[i];
        h = hpod(f.addr, h); h = hpod(f.nblocks, h); h = hash64(f.argtype, sizeof f.argtype, h); h = hpod(f.nflt, h);
        h = hpod(f.livein_r, h); h = hpod(f.livein_f, h); h = hpod(f.retregs, h); h = hpod(f.retbits, h);
    }
    for (unsigned i = 0; i < unit_ncallees; i++) {
        const CalleeInfo& c = unit_callees[i];
        h = hpod(c.addr, h); h = hpod(c.imask, h); h = hpod(c.fmask, h); h = hpod(c.defr, h); h = hpod(c.deff, h);
        h = hash64(c.ptrsz, sizeof c.ptrsz, h); h = hash64(c.outp, sizeof c.outp, h); h = hpod((uint8_t)(c.real != nullptr), h);
        h = hpod(c.declared, h); h = hpod(c.retkind, h); h = hpod(c.retbits, h); h = hpod(c.retconst, h); h = hpod(c.nstack, h);
        h = hpod(c.outonly, h); h = hash64(c.ext, sizeof c.ext, h); h = hpod(c.fill, h); h = hpod(c.heapref, h);
        h = hash64(c.dynsrc, sizeof c.dynsrc, h); h = hash64(c.dynmul, sizeof c.dynmul, h);
    }
    for (unsigned i = 0; i < unit_nimport_slots; i++) h = hpod(unit_import_slots[i], h);
    return h;
}

static void put_map(refc::Buf& b, const std::unordered_map<uint32_t, uint32_t>& m) {
    std::vector<std::pair<uint32_t, uint32_t>> v(m.begin(), m.end());
    std::sort(v.begin(), v.end());
    b.pod((uint64_t)v.size());
    for (auto& kv : v) { b.pod(kv.first); b.pod(kv.second); }
}
static void get_map(refc::Rd& r, std::unordered_map<uint32_t, uint32_t>& m) {
    m.clear();
    for (uint64_t n = r.len(8), k = 0; k < n && !r.bad; k++) { uint32_t a = r.pod<uint32_t>(); m[a] = r.pod<uint32_t>(); }
}
/* chunk bytes: only materialized bytes have a value (materialize() overwrites the others), so the others are stored as 0;
 * the initial values as XOR with the current ones (0 for every byte nothing wrote: the zero runs compress) */
static void put_bytes(refc::Buf& b, uint64_t have, const uint8_t* d, const uint8_t* init) {
    uint8_t x[64], y[64];
    for (int k = 0; k < 64; k++) {
        bool h = have >> k & 1;
        x[k] = h ? d[k] : 0;
        y[k] = h ? (uint8_t)(d[k] ^ init[k]) : 0;
    }
    b.raw(x, 64);
    b.raw(y, 64);
}
static void get_bytes(refc::Rd& r, uint8_t* d, uint8_t* init) {
    uint8_t y[64];
    r.raw(d, 64);
    r.raw(y, 64);
    for (int k = 0; k < 64; k++) init[k] = d[k] ^ y[k];
}
static void put_bits(refc::Buf& b, const uint64_t* have, const uint64_t* wr, const uint64_t* fw, const uint64_t* dw) {
    b.raw(have, 512); b.raw(wr, 512); b.raw(fw, 512); b.raw(dw, 512);
}
static void get_bits(refc::Rd& r, uint64_t* have, uint64_t* wr, uint64_t* fw, uint64_t* dw) {
    r.raw(have, 512); r.raw(wr, 512); r.raw(fw, 512); r.raw(dw, 512);
}

static void put_snap(refc::Buf& b, const SetupState& s) {
    b.pod((uint64_t)s.nbytes); b.pod(s.fany);
    put_map(b, s.alloc_cursor); put_map(b, s.alloc_calls); put_map(b, s.per_target); put_map(b, s.retained);
    b.vec32(s.osmem);
    b.pod(s.budget); b.pod(s.ncalls); b.pod(s.epoch); b.pod(s.traps);
    b.vec32(s.orig_stack);
    b.pod(s.set_calls); b.pod(s.set_bytes); b.pod(s.surface_calls);
    b.pod((uint64_t)s.pages.size());
    for (const SetupPage& p : s.pages) {
        b.pod(p.pn);
        put_bits(b, p.have, p.wr, p.fw, p.dw);
        b.pod((uint64_t)p.ch.size());
        for (auto& c : p.ch) { b.pod(c.first); put_bytes(b, p.have[c.first], c.second.d, c.second.init); }
    }
}
static bool get_snap(refc::Rd& r, SetupState* s) {
    *s = SetupState();
    s->nbytes = (size_t)r.pod<uint64_t>(); s->fany = r.pod<bool>();
    get_map(r, s->alloc_cursor); get_map(r, s->alloc_calls); get_map(r, s->per_target); get_map(r, s->retained);
    r.vec32(s->osmem);
    s->budget = r.pod<uint64_t>(); s->ncalls = r.pod<uint64_t>(); s->epoch = r.pod<uint32_t>(); s->traps = r.pod<int>();
    r.vec32(s->orig_stack);
    s->set_calls = r.pod<uint64_t>(); s->set_bytes = r.pod<uint64_t>(); s->surface_calls = r.pod<uint64_t>();
    for (uint64_t n = r.len(2048 + 12), k = 0; k < n && !r.bad; k++) {
        s->pages.emplace_back();
        SetupPage& p = s->pages.back();
        p.pn = r.pod<uint32_t>();
        get_bits(r, p.have, p.wr, p.fw, p.dw);
        for (uint64_t m = r.len(132), j = 0; j < m && !r.bad; j++) {
            uint32_t w = r.pod<uint32_t>();
            Chunk c{};
            get_bytes(r, c.d, c.init);
            if (w >= 64) r.bad = true;
            p.ch.emplace_back(w, c);
        }
    }
    return !r.bad && r.p == r.e;
}
/* delta encoding of a validation state (2026-10-30): nearly all of it is the snapshot again (current values) and the
 * input's own generated bytes (initial values of what the setups read or overwrote), ~27 MB per record for the agl
 * world. With a predictor, each current byte is stored XOR the snapshot's current byte there (0 where the snapshot has
 * none) and each initial value XOR initial_byte() of this input, so the zero runs compress to a few hundred KB. The
 * prediction must be the same on both ends (same snapshot, same input set up); the record carries the hash of its plain
 * encoding, and a reader whose reconstruction does not hash to it treats the record as unreadable. */
struct Predict {
    std::unordered_map<uint32_t, std::pair<const SetupPage*, std::array<const Chunk*, 64>>> pg;
    explicit Predict(const SetupState& snap) {
        for (const SetupPage& p : snap.pages) {
            auto& e = pg[p.pn];
            e.first = &p;
            e.second.fill(nullptr);
            for (auto& c : p.ch) e.second[c.first] = &c.second;
        }
    }
    /* the prediction of a whole page: the snapshot's page (bits, chunks, current bytes), the input's initial values */
    bool make_page(ValPage* v) const {
        auto it = pg.find(v->pn);
        if (it == pg.end()) return false;
        const SetupPage& p = *it->second.first;
        memcpy(v->have, p.have, sizeof v->have); memcpy(v->wr, p.wr, sizeof v->wr);
        memcpy(v->fw, p.fw, sizeof v->fw); memcpy(v->dw, p.dw, sizeof v->dw);
        v->ch.clear();
        for (uint32_t w = 0; w < 64; w++) {
            if (!p.have[w]) continue;
            ValChunk c{};
            c.w = w;
            chunk(v->pn, w, p.have[w], c.d, c.init);
            v->ch.push_back(c);
        }
        return true;
    }
    bool same_page(const ValPage& v) const {
        ValPage q{};
        q.pn = v.pn;
        if (!make_page(&q) || memcmp(q.have, v.have, sizeof q.have) || memcmp(q.wr, v.wr, sizeof q.wr) ||
            memcmp(q.fw, v.fw, sizeof q.fw) || memcmp(q.dw, v.dw, sizeof q.dw) || q.ch.size() != v.ch.size())
            return false;
        for (size_t k = 0; k < q.ch.size(); k++)
            if (q.ch[k].w != v.ch[k].w || !v.ch[k].ep.empty() || memcmp(q.ch[k].d, v.ch[k].d, 64) || memcmp(q.ch[k].init, v.ch[k].init, 64))
                return false;
        return true;
    }
    /* the snapshot's bits of page pn (zero where it has none), XORed into the four bit maps */
    void bits(uint32_t pn, uint64_t* have, uint64_t* wr, uint64_t* fw, uint64_t* dw) const {
        auto it = pg.find(pn);
        if (it == pg.end()) return;
        const SetupPage& p = *it->second.first;
        for (int w = 0; w < 64; w++) { have[w] ^= p.have[w]; wr[w] ^= p.wr[w]; fw[w] ^= p.fw[w]; dw[w] ^= p.dw[w]; }
    }
    /* predicted current and initial bytes of chunk (pn, w), for the have bits given */
    void chunk(uint32_t pn, uint32_t w, uint64_t have, uint8_t* d, uint8_t* init) const {
        memset(d, 0, 64);
        auto it = pg.find(pn);
        if (it != pg.end())
            if (const Chunk* c = it->second.second[w])
                for (int k = 0; k < 64; k++)
                    if (it->second.first->have[w] >> k & 1) d[k] = c->d[k];
        for (int k = 0; k < 64; k++) {
            bool known;
            init[k] = have >> k & 1 ? initial_byte(pn << 12 | w << 6 | k, &known) : 0;
        }
    }
};
static void put_rec(refc::Buf& b, const SnapRec& x, const Predict* pr = nullptr) {
    const ValState& s = x.st;
    b.pod((uint64_t)s.pages.size());
    for (const ValPage& p : s.pages) {
        b.pod(p.pn);
        if (pr) { /* u8 1: the page is exactly the prediction (the snapshot's page, the input's own initial values) */
            bool same = pr->same_page(p);
            b.pod((uint8_t)same);
            if (same) continue;
        }
        if (pr) {
            uint64_t h[64], wr[64], fw[64], dw[64];
            memcpy(h, p.have, sizeof h); memcpy(wr, p.wr, sizeof wr); memcpy(fw, p.fw, sizeof fw); memcpy(dw, p.dw, sizeof dw);
            pr->bits(p.pn, h, wr, fw, dw);
            put_bits(b, h, wr, fw, dw);
        } else
            put_bits(b, p.have, p.wr, p.fw, p.dw);
        b.pod((uint64_t)p.ch.size());
        for (const ValChunk& c : p.ch) {
            b.pod(c.w);
            if (pr) {
                uint8_t pd[64], pi[64], xd[64], xi[64];
                pr->chunk(p.pn, c.w, p.have[c.w], pd, pi);
                for (int k = 0; k < 64; k++) {
                    bool h = p.have[c.w] >> k & 1;
                    xd[k] = h ? (uint8_t)(c.d[k] ^ pd[k]) : 0;
                    xi[k] = h ? (uint8_t)(c.init[k] ^ pi[k]) : 0;
                }
                b.raw(xd, 64);
                b.raw(xi, 64);
            } else
                put_bytes(b, p.have[c.w], c.d, c.init);
            b.pod((uint8_t)!c.ep.empty());
            if (!c.ep.empty()) b.raw(c.ep.data(), 64 * 4);
        }
    }
    b.pod(s.fany);
    put_map(b, s.alloc_cursor); put_map(b, s.alloc_calls); put_map(b, s.per_target); put_map(b, s.retained); put_map(b, s.obs_ord);
    b.vec32(s.osmem);
    b.pod(s.budget); b.pod(s.ncalls); b.pod(s.epoch); b.pod(s.dl_start); b.pod(s.dl_size); b.pod(s.dl_cmds); b.pod(s.stray);
    b.pod(s.first_stray); b.pod(s.traps); b.pod(s.stop_count); b.pod(s.nfcopy); b.raw(s.fcopy, sizeof s.fcopy);
    b.pod((uint64_t)s.ncalls_rec); b.pod(s.valid);
    b.vec32(x.orig_stack);
    for (int k = 0; k < 6; k++) b.pod(x.stats[k]);
    b.vec32(x.utf16);
}
static bool get_rec(refc::Rd& r, SnapRec* x, const Predict* pr = nullptr) {
    *x = SnapRec();
    ValState& s = x->st;
    for (uint64_t n = r.len(pr ? 5 : 2048 + 12), k = 0; k < n && !r.bad; k++) {
        s.pages.emplace_back();
        ValPage& p = s.pages.back();
        p.pn = r.pod<uint32_t>();
        if (pr && r.pod<uint8_t>()) {
            if (!pr->make_page(&p)) r.bad = true;
            continue;
        }
        get_bits(r, p.have, p.wr, p.fw, p.dw);
        if (pr) pr->bits(p.pn, p.have, p.wr, p.fw, p.dw);
        for (uint64_t m = r.len(133), j = 0; j < m && !r.bad; j++) {
            p.ch.emplace_back();
            ValChunk& c = p.ch.back();
            c.w = r.pod<uint32_t>();
            if (c.w >= 64) { r.bad = true; break; }
            if (pr) {
                uint8_t pd[64], pi[64];
                r.raw(c.d, 64);
                r.raw(c.init, 64);
                pr->chunk(p.pn, c.w, p.have[c.w], pd, pi);
                for (int k = 0; k < 64; k++)
                    if (p.have[c.w] >> k & 1) { c.d[k] ^= pd[k]; c.init[k] ^= pi[k]; }
            } else
                get_bytes(r, c.d, c.init);
            if (r.pod<uint8_t>()) { c.ep.resize(64); r.raw(c.ep.data(), 64 * 4); }
        }
    }
    s.fany = r.pod<bool>();
    get_map(r, s.alloc_cursor); get_map(r, s.alloc_calls); get_map(r, s.per_target); get_map(r, s.retained); get_map(r, s.obs_ord);
    r.vec32(s.osmem);
    s.budget = r.pod<uint64_t>(); s.ncalls = r.pod<uint64_t>(); s.epoch = r.pod<uint32_t>(); s.dl_start = r.pod<uint32_t>();
    s.dl_size = r.pod<uint32_t>(); s.dl_cmds = r.pod<uint32_t>(); s.stray = r.pod<uint32_t>(); s.first_stray = r.pod<uint32_t>();
    s.traps = r.pod<int>(); s.stop_count = r.pod<int>(); s.nfcopy = r.pod<unsigned>(); r.raw(s.fcopy, sizeof s.fcopy);
    s.ncalls_rec = (size_t)r.pod<uint64_t>(); s.valid = r.pod<bool>();
    r.vec32(x->orig_stack);
    for (int k = 0; k < 6; k++) x->stats[k] = r.pod<uint64_t>();
    r.vec32(x->utf16);
    x->ok = true;
    if (x->utf16.size() % 2) return false;
    for (size_t k = 0; k < x->utf16.size(); k += 2) if (x->utf16[k] > 4096) return false;
    return !r.bad && r.p == r.e && s.valid;
}

static uint32_t switches() {
    return (g_nulltrap ? 1 : 0) | (g_strict_nan ? 2 : 0) | (V.clobber ? 4 : 0) | (g_calleesp_all ? 8 : 0) | (g_footprint_all ? 16 : 0) |
           (g_observe_pointees ? 32 : 0);
}
static std::string snap_header(uint32_t addr) {
    refc::Buf b;
    b.pod(kSnapMagic); b.str(key); b.pod(startup); b.pod(addr); b.pod(switches()); b.pod(g_obs_cap);
    b.pod(g_access_budget); b.pod(g_call_budget);
    return b.s;
}
static std::string val_header(uint32_t addr, uint64_t seed0, uint64_t snap_hash) {
    refc::Buf b;
    b.pod(kValMagic); b.str(key); b.pod(startup); b.pod(addr); b.pod(switches()); b.pod(g_obs_cap);
    b.pod(g_access_budget); b.pod(g_call_budget);
    b.pod(seed0); b.pod(snap_hash);
    b.vec32(V.dict); /* the generated inputs (hence each input's state) depend on the dictionary */
    return b.s;
}
/* file names carry a tag of the configuration (the snapshot header: key, start-up state, function, switches), so caches
 * of several configurations (plain and -nulltrap, several specs) share one directory without replacing each other */
static std::string tag(uint32_t addr) {
    std::string hd = snap_header(addr);
    char t[32];
    snprintf(t, sizeof t, "%08X-%08llx", addr, (unsigned long long)(hash64(hd.data(), hd.size(), 0x7A) & 0xFFFFFFFFull));
    return t;
}
static std::string snap_path(uint32_t addr) { return dir + "/" + tag(addr) + ".snap"; }
static std::string val_path(uint32_t addr, uint64_t seed0) {
    char n[32]; snprintf(n, sizeof n, "-s%llu.val", (unsigned long long)seed0); return dir + "/" + tag(addr) + n;
}
/* the snapshot itself, content-addressed: functions (and specs) with the same world share one blob */
static std::string blob_path(uint64_t h) { char n[48]; snprintf(n, sizeof n, "/world-%016llx.blob", (unsigned long long)h); return dir + n; }
static size_t cap_bytes() {
    const char* e = getenv("WWHD_SNAPCACHE_MAX_MB");
    return (size_t)(e ? atof(e) : 1024.0) * (1u << 20);
}
static bool write_file(const std::string& fin, const std::string& data) { /* atomically (rename), as refcache */
    std::string tmp = fin + ".tmp" + std::to_string(getpid());
    FILE* f = fopen(tmp.c_str(), "wb");
    bool ok = f && fwrite(data.data(), 1, data.size(), f) == data.size();
    if (f && fclose(f)) ok = false;
    if (!ok || rename(tmp.c_str(), fin.c_str())) {
        fprintf(stderr, "snapcache: cannot write %s (%s); readers run these setups live\n", fin.c_str(), strerror(errno));
        unlink(tmp.c_str());
        return false;
    }
    return true;
}
static bool read_file(const std::string& p, std::string* data) {
    FILE* f = fopen(p.c_str(), "rb");
    if (!f) return false;
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    data->resize(sz > 0 ? sz : 0);
    bool ok = sz > 0 && fread(&(*data)[0], 1, sz, f) == (size_t)sz;
    fclose(f);
    return ok;
}
/* one encoded blob: u64 hash of the decoded payload, u64 length, zero-run encoded payload */
static void put_blob(refc::Buf& out, const std::string& payload) {
    std::string enc;
    refc::zenc(payload, enc);
    out.pod(hash64(payload.data(), payload.size(), 0x5A));
    out.pod((uint64_t)enc.size());
    out.raw(enc.data(), enc.size());
}
static bool get_blob(refc::Rd& r, std::string* payload, uint64_t* h = nullptr) {
    uint64_t hh = r.pod<uint64_t>();
    uint64_t n = r.len(1);
    if (r.bad || !refc::zdec(r.p, r.p + n, *payload)) return false;
    r.p += n;
    if (h) *h = hh;
    return hash64(payload->data(), payload->size(), 0x5A) == hh;
}

/* the writer's snapshot file; returns the payload hash the function's .val header names */
static uint64_t write_snap(uint32_t addr, const SetupState& s, uint64_t seed0) {
    refc::Buf pay;
    pay.pod(seed0); /* the input seed of the capture runs (-snapcache-check compares byte for byte when it is the same) */
    put_snap(pay, s);
    uint64_t h = hash64(pay.s.data(), pay.s.size(), 0x5A);
    struct stat sb;
    if (stat(blob_path(h).c_str(), &sb) != 0) { /* (a blob of that name holds this payload: readers verify the hash) */
        refc::Buf blob;
        put_blob(blob, pay.s);
        if (blob.s.size() > cap_bytes()) {
            fprintf(stderr, "snapcache: %08X: the snapshot passes WWHD_SNAPCACHE_MAX_MB (%zu MB); readers run these setups live\n", addr,
                    cap_bytes() >> 20);
            return 0;
        }
        if (!write_file(blob_path(h), blob.s)) return 0;
    }
    std::string hd = snap_header(addr);
    refc::Buf out; /* the function's pointer: header, payload hash */
    out.pod((uint64_t)hd.size());
    out.raw(hd.data(), hd.size());
    out.pod(h);
    std::string have;
    if (!(read_file(snap_path(addr), &have) && have == out.s) && !write_file(snap_path(addr), out.s)) return 0; /* (an
                                                                       * identical pointer, e.g. from another seed, stays) */
    wrote_snap++;
    return h;
}
enum Load { MISSING, CONFIG, CORRUPT, OK };
/* the reader's snapshot: OK (s, *cap_seed, *h set), MISSING, CONFIG (written for another configuration) or CORRUPT */
static Load load_snap(uint32_t addr, SetupState* s, uint64_t* cap_seed, uint64_t* h) {
    std::string data;
    if (!read_file(snap_path(addr), &data)) return MISSING;
    std::string hd = snap_header(addr);
    uint64_t hl = 0;
    if (data.size() >= 8) memcpy(&hl, data.data(), 8);
    if (data.size() < 8 || hl != hd.size() || data.size() < 8 + hl || data.compare(8, hl, hd) != 0) return CONFIG;
    if (data.size() != 8 + hl + 8) return CORRUPT;
    uint64_t want;
    memcpy(&want, data.data() + 8 + hl, 8);
    std::string blob;
    if (!read_file(blob_path(want), &blob)) return CORRUPT;
    refc::Rd r{blob.data(), blob.data() + blob.size()};
    std::string pay;
    if (!get_blob(r, &pay, h) || r.p != r.e || *h != want) return CORRUPT;
    refc::Rd q{pay.data(), pay.data() + pay.size()};
    *cap_seed = q.pod<uint64_t>();
    if (!get_snap(q, s)) return CORRUPT;
    return OK;
}
static std::string snap_bytes(const SetupState& s, uint64_t seed0) {
    refc::Buf pay;
    pay.pod(seed0);
    put_snap(pay, s);
    return pay.s;
}

/* the writer's validation records of one function: (input index, encoded record) */
struct ValWriter {
    bool on = false;
    uint32_t addr = 0;
    uint64_t seed0 = 0, snap_hash = 0;
    refc::Buf body;
    uint64_t n = 0;
    int lo = 0, hi = 0, ngen = 0; /* the generated inputs this run covers: a -range shard writes its own part (below) */
    void begin(uint32_t a, uint64_t s0, uint64_t sh, int glo, int ghi, int n_all) {
        on = mode == WRITE && sh;
        pr.reset(on ? new Predict(g_snap) : nullptr);
        addr = a; seed0 = s0; snap_hash = sh; n = 0;
        lo = glo; hi = ghi; ngen = n_all;
        body.s.clear();
    }
    std::unique_ptr<Predict> pr;
    void add(int idx, const SnapRec& rec) { /* (call while the input is still set up: the prediction reads it) */
        if (!on || !rec.ok) return;
        refc::Buf plain, pay;
        put_rec(plain, rec);
        pay.pod(hash64(plain.s.data(), plain.s.size(), 0x5B));
        std::string().swap(plain.s);
        put_rec(pay, rec, pr.get());
        body.pod((uint32_t)idx);
        put_blob(body, pay.s);
        n++;
        if (body.s.size() > cap_bytes()) {
            fprintf(stderr, "snapcache: %08X: the validation states pass WWHD_SNAPCACHE_MAX_MB (%zu MB); readers run these setups live\n",
                    addr, cap_bytes() >> 20);
            on = false;
            std::string().swap(body.s);
        }
    }
    void finish() {
        if (!on) return;
        on = false;
        pr.reset();
        if (n) {
            std::string hd = val_header(addr, seed0, snap_hash);
            refc::Buf out;
            out.pod((uint64_t)hd.size());
            out.raw(hd.data(), hd.size());
            out.pod(n);
            out.raw(body.s.data(), body.s.size());
            /* a -range shard writes ADDR-TAG-sSEED.val.LO-HI (2026-10-31, fasttools2: mutate.py's sharded baseline; the
             * parts of one function would otherwise overwrite each other); refcache.py join_val_parts concatenates them */
            std::string fin = val_path(addr, seed0);
            if (lo != 0 || hi != ngen) fin += "." + std::to_string(lo) + "-" + std::to_string(hi);
            if (write_file(fin, out.s)) wrote_val += n;
        }
        std::string().swap(body.s);
    }
};
/* the reader's validation records of one function (decoded when the input comes) */
struct ValReader {
    std::string data;
    std::map<int, std::pair<size_t, size_t>> at; /* input index -> blob [begin, end) in data */
    SnapRec cur;
    std::unique_ptr<Predict> pr;
    void clear() { std::string().swap(data); at.clear(); cur = SnapRec(); pr.reset(); }
    /* false: a corrupt file (the caller exits under -snapcache-check) */
    bool open(uint32_t addr, uint64_t seed0, uint64_t snap_hash) {
        clear();
        if (!read_file(val_path(addr, seed0), &data)) return true;
        std::string hd = val_header(addr, seed0, snap_hash);
        uint64_t hl = 0;
        if (data.size() >= 8) memcpy(&hl, data.data(), 8);
        if (data.size() < 8 || hl != hd.size() || data.size() < 8 + hl || data.compare(8, hl, hd) != 0) {
            fprintf(stderr, "snapcache: the validation states of %08X do not match this run (configuration, seed or dictionary); "
                    "its validation inputs run their setups live\n", addr);
            clear();
            return true;
        }
        refc::Rd r{data.data() + 8 + hl, data.data() + data.size()};
        uint64_t n = r.pod<uint64_t>();
        for (uint64_t k = 0; k < n && !r.bad; k++) {
            int idx = (int)r.pod<uint32_t>();
            size_t b = r.p - data.data();
            r.pod<uint64_t>();
            uint64_t len = r.len(1);
            r.p += len;
            at[idx] = {b, (size_t)(r.p - data.data())};
        }
        if (r.bad || r.p != r.e) {
            fprintf(stderr, "snapcache: the validation states of %08X are unreadable; its validation inputs run their setups live\n", addr);
            clear();
            return false;
        }
        pr.reset(new Predict(g_snap));
        return true;
    }
    /* the record of input idx (nullptr: none, run the setups); *bad: present but corrupt */
    const SnapRec* find(int idx, bool* bad) {
        *bad = false;
        auto it = at.find(idx);
        if (it == at.end()) return nullptr;
        refc::Rd r{data.data() + it->second.first, data.data() + it->second.second};
        std::string pay;
        if (!get_blob(r, &pay) || r.p != r.e) { *bad = true; return nullptr; }
        refc::Rd q{pay.data(), pay.data() + pay.size()};
        uint64_t want = q.pod<uint64_t>();
        if (!get_rec(q, &cur, pr.get())) { *bad = true; return nullptr; }
        refc::Buf plain; /* the reconstruction is the recorded state (same snapshot, same input) */
        put_rec(plain, cur);
        if (hash64(plain.s.data(), plain.s.size(), 0x5B) != want) { *bad = true; return nullptr; }
        return &cur;
    }
};
}  // namespace snapc

/* -selftest-mem: checks of the compact memory structures without a spec, then exit (0: all pass).
 * Shared fill pages (copy on write), the 16-bit/32-bit clobber epochs, and NetMap. Used by pagerss_test.py. */
static int memory_selftest() {
    int fails = 0;
    auto check = [&](bool ok, const char* what) { printf("%s %s\n", ok ? "PASS" : "FAIL", what); fails += !ok; };
    bool known;
    const uint32_t A = 0x60000000u, B = 0x60001000u, C = 0x60002000u;
    /* (1) a write into a shared fill page copies it first; the other page and the shared page keep the fill byte */
    clear_all();
    snap_range(A, 0x2000, nullptr, 0xAB);
    SnapPage* shared = g_fill_pages[0xAB];
    check(shared && V.snap[A >> 12] == shared && V.snap[B >> 12] == shared, "1a: a whole-page fill points both pages at the shared 0xAB page");
    snap_set(A + 0x10, 0x55);
    check(V.snap[A >> 12] != shared && V.snap[B >> 12] == shared, "1b: snap_set copies the written page only");
    check(initial_byte(A + 0x10, &known) == 0x55 && known, "1c: the written byte reads back");
    check(initial_byte(A + 0x11, &known) == 0xAB && initial_byte(B + 0x10, &known) == 0xAB, "1d: the copy's other bytes and the other page read 0xAB");
    bool intact = true;
    for (int i = 0; i < 4096; i++) intact &= shared->d[i] == 0xAB;
    for (int w = 0; w < 64; w++) intact &= shared->has[w] == ~0ull;
    check(intact, "1e: the shared page itself is unchanged");
    snap_range(B + 0x20, 8, nullptr, 0xCD); /* a partial fill on the shared page */
    check(V.snap[B >> 12] != shared && initial_byte(B + 0x20, &known) == 0xCD && initial_byte(B + 0x28, &known) == 0xAB &&
          initial_byte(B + 0x1F, &known) == 0xAB, "1f: a partial fill copies the page first");
    /* (2) fills with different bytes never share a page */
    clear_all();
    snap_range(A, 0x1000, nullptr, 0x11);
    snap_range(B, 0x1000, nullptr, 0x22);
    snap_range(C, 0x1000, nullptr, 0x11);
    check(V.snap[A >> 12] != V.snap[B >> 12] && V.snap[A >> 12] == V.snap[C >> 12], "2a: 0x11 and 0x22 pages differ; equal fill bytes share");
    check(initial_byte(A + 5, &known) == 0x11 && initial_byte(B + 5, &known) == 0x22 && initial_byte(C + 0xFFF, &known) == 0x11, "2b: each reads its own fill byte");
    snap_range(B, 0x1000, nullptr, 0x11); /* a later whole-page fill replaces the page */
    check(V.snap[B >> 12] == V.snap[A >> 12] && initial_byte(B + 5, &known) == 0x11, "2c: a later whole-page fill wins");
    /* (3) guest writes go to Page chunks, never into the fill page; a reset (as between runs) and a new input give back the fill byte */
    clear_all();
    snap_range(A, 0x1000, nullptr, 0x7E);
    V.run_serial++;
    vm_st8(A + 0x40, 0x01);
    vm_st32(A + 0x80, 0xDEADBEEFu);
    check(vm_ld8(A + 0x40) == 0x01 && vm_ld32(A + 0x80) == 0xDEADBEEFu, "3a: guest writes read back");
    check(g_fill_pages[0x7E]->d[0x40] == 0x7E && initial_byte(A + 0x40, &known) == 0x7E, "3b: the fill page keeps 0x7E");
    NetMap nm = net_writes();
    check(nm.size() == 5 && nm.find(A + 0x40) != nm.end() && nm.at(A + 0x40) == 0x01 && nm.at(A + 0x83) == 0xEF, "3c: the net effect holds the 5 written bytes");
    reset_memory();
    V.run_serial++;
    check(vm_ld8(A + 0x40) == 0x7E && vm_ld32(A + 0x80) == 0x7E7E7E7Eu, "3d: after a reset the bytes read the fill byte again");
    clear_all();
    snap_range(A, 0x1000, nullptr, 0x7E);
    V.run_serial++;
    check(vm_ld8(A + 0x40) == 0x7E, "3e: a new input reads the fill byte");
    clear_all();
    /* (4) clobber epochs: the compact form returns what was stored */
    Chunk k{};
    bool ep_ok = ep_get(&k, 3) == 0;
    ep_set(&k, 3, 0); ep_ok &= k.ep == nullptr;
    ep_set(&k, 3, 70000); ep_set(&k, 4, 70000 + 65534); ep_set(&k, 5, 0);
    ep_ok &= !k.epw && ep_get(&k, 3) == 70000 && ep_get(&k, 4) == 70000 + 65534 && ep_get(&k, 5) == 0;
    ep_set(&k, 6, 70000 + 65535); /* does not fit: widened, every value kept */
    ep_ok &= k.epw && ep_get(&k, 3) == 70000 && ep_get(&k, 4) == 70000 + 65534 && ep_get(&k, 6) == 70000 + 65535;
    Chunk k2{};
    ep_set(&k2, 1, 500); ep_set(&k2, 1, 0); ep_set(&k2, 2, 7); /* below the base, nothing else set: a new base, still 16-bit */
    ep_ok &= !k2.epw && ep_get(&k2, 2) == 7 && ep_get(&k2, 1) == 0;
    ep_set(&k2, 3, 3); /* below the base with another value kept: widened */
    ep_ok &= k2.epw && ep_get(&k2, 2) == 7 && ep_get(&k2, 3) == 3;
    ep_zero(&k2);
    ep_ok &= ep_get(&k2, 2) == 0 && ep_get(&k2, 3) == 0;
    check(ep_ok, "4: clobber epochs (16-bit base, widening, rebase, zero)");
    clear_all();
    /* (5) NetMap: the map operations the comparison uses */
    NetMap x, y;
    x.push(0x100, 1); x.push(0x105, 2); y.push(0x100, 1); y.push(0x105, 2);
    bool nm_ok = x == y && x.count(0x105) && !x.count(0x101) && x.find(0x104) == x.end() && x.at(0x100) == 1;
    y.v.back().second = 3;
    nm_ok &= x != y;
    uint32_t where = 0;
    std::vector<uint32_t> none;
    nm_ok &= !same_net(x, y, &where, none, none, none, none) && where == 0x105;
    check(nm_ok, "5: NetMap find/count/at/==/!= and same_net's first difference");
    printf("memory selftest: %d failure(s)\n", fails);
    return fails ? 1 : 0;
}

/* WWHD_MEMSTAT: one line per tested function at its start (2026-11-02): the process's resident size, footprint and
 * peak, and the sizes of the state that outlives an input, so growth across the functions of one process shows */
static void memstat_func(uint32_t addr) {
    if (!g_memstat) return;
    task_vm_info_data_t vi;
    mach_msg_type_number_t cnt = TASK_VM_INFO_COUNT;
    if (task_info(mach_task_self(), TASK_VM_INFO, (task_info_t)&vi, &cnt) != KERN_SUCCESS) return;
    struct rusage ru;
    getrusage(RUSAGE_SELF, &ru);
    fprintf(stderr, "memstat-func %08X rss=%.1f MB footprint=%.1f MB peak=%.1f MB pages=%zu tmpl=%zu snapstate=%zu valstate=%zu snap=%zu "
            "callscap=%zu origseqcap=%zu origlrcap=%zu origstack=%zu/%zu touchedcap=%zu badfetchcap=%zu\n",
            addr, vi.resident_size / 1048576.0, vi.phys_footprint / 1048576.0, ru.ru_maxrss / 1048576.0, V.pages.size(),
            g_tmpl_pages.size(), g_snap.pages.size(), g_valstate.pages.size(), V.snap.size(), V.calls.capacity(),
            V.orig_seq.capacity(), V.orig_lr.capacity(), V.orig_stack.size(), V.orig_stack.bucket_count(), V.touched.capacity(),
            g_badfetch.capacity());
}

int main(int argc, char** argv) {
#if GABI_GUEST_GUARD >= 2
    atexit(wrap_summary); /* -guardstrict build: the closing summary, also when nothing wrapped */
#endif
    if (argc == 2 && !strcmp(argv[1], "-selftest-mem")) return memory_selftest();
    {
        struct sigaction sa{};
        sa.sa_sigaction = crash_handler;
        sa.sa_flags = SA_SIGINFO;
        sigemptyset(&sa.sa_mask);
        sigaction(SIGSEGV, &sa, nullptr);
        sigaction(SIGBUS, &sa, nullptr);
    }
    signal(SIGVTALRM, run_timeout_handler);
    int ngen = 1000, verbose = 0;
    uint64_t seed0 = 1;
    const char* recdir = nullptr;
    const char* image = "build/verify/image.bin";
    std::set<uint32_t> only;  /* -only ADDR[,ADDR...]: test these functions only (shards of a heavy unit) */
    bool list_only = false;   /* -list: print the unit's candidate addresses and exit */
    bool list_fn = false;     /* -listfn: ADDR and the link-time address of its entry thunk per candidate (vcache.py) */
    int range_lo = 0, range_hi = -1; /* -range A:B: generated inputs A..B-1 only (recorded inputs only when A == 0) */
    bool shardout = false;    /* -shardout: machine-readable per-function results for verify.py -shards to merge */
    bool stopfirst = false;   /* -stopfirst: stop at the first row whose first difference is not an abort (mutate.py) */
    FILE* dsiout = nullptr;   /* -dsiout FILE (with -nulltrap): per function, the inputs a page-0 DSI ended (a -runlist) */
    bool forkserver = false;  /* -forkserver: start once, fork one run per stdin request (mutate.py --fork) */
    /* -trace SEED: off unless given. trace_seed alone used to default to 0, so a run with `-seed 0` and
     * no -trace traced generated input 0 and dictionary pre-run 0 and switched the refcache writer and -snapcache off;
     * `trace_on` is the switch, so every SEED (0 included) is traced only when asked for */
    bool trace_on = false;
    uint64_t trace_seed = 0;
    bool prog_off = false;    /* -noprogress, -progress-interval S, -stuck S, -hanglimit S: override the WWHD_VERIFY_* env */
    const char *prog_interval = nullptr, *prog_stuck = nullptr, *prog_hang = nullptr;
    uint64_t thr_sched_cli = 0;
    bool thr_sched_set = false;
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-n") && i + 1 < argc) ngen = atoi(argv[++i]);
        else if (!strcmp(argv[i], "-seed") && i + 1 < argc) seed0 = strtoull(argv[++i], nullptr, 0);
        else if (!strcmp(argv[i], "-watch") && i + 1 < argc) {
            char* e; g_watch_lo = (uint32_t)strtoul(argv[++i], &e, 16); g_watch_len = 4;
            if (*e == ':') g_watch_len = (uint32_t)strtoul(e + 1, &e, 0);
            if (*e || !g_watch_len) { fprintf(stderr, "invalid -watch ADDR[:LEN]\n"); exit(2); }
        }
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
        else if (!strcmp(argv[i], "-listfn")) list_fn = true;
        else if (!strcmp(argv[i], "-range") && i + 1 < argc) { if (sscanf(argv[++i], "%d:%d", &range_lo, &range_hi) != 2) { fprintf(stderr, "bad -range\n"); return 2; } }
        else if (!strcmp(argv[i], "-shardout")) shardout = true;
        else if (!strcmp(argv[i], "-stopfirst")) stopfirst = true;
        else if (!strcmp(argv[i], "-forkserver")) forkserver = true;
        else if (!strcmp(argv[i], "-refcache-write") && i + 1 < argc) { refc::mode = refc::WRITE; refc::dir = argv[++i]; }
        else if (!strcmp(argv[i], "-refcache") && i + 1 < argc) { refc::mode = refc::READ; refc::dir = argv[++i]; }
        else if (!strcmp(argv[i], "-refcache-key") && i + 1 < argc) refc::key = argv[++i];
        else if (!strcmp(argv[i], "-refcache-check")) refc::check = true;
        else if (!strcmp(argv[i], "-refcache-digest")) refc::digest = true;
        else if (!strcmp(argv[i], "-refcache-digest-miss")) refc::digest_miss = true;
        else if (!strcmp(argv[i], "-snapcache-write") && i + 1 < argc) { snapc::mode = snapc::WRITE; snapc::dir = argv[++i]; }
        else if (!strcmp(argv[i], "-snapcache") && i + 1 < argc) { snapc::mode = snapc::READ; snapc::dir = argv[++i]; }
        else if (!strcmp(argv[i], "-snapcache-key") && i + 1 < argc) snapc::key = argv[++i];
        else if (!strcmp(argv[i], "-snapcache-check")) snapc::check = true;
        else if (!strcmp(argv[i], "-observe-pointees")) g_observe_pointees = true;
        else if (!strcmp(argv[i], "-observe-heap") && i + 1 < argc) {
            uint32_t ha = (uint32_t)strtoul(argv[++i], nullptr, 16);
            if (std::find(g_obs_heaps.begin(), g_obs_heaps.end(), ha) == g_obs_heaps.end()) g_obs_heaps.push_back(ha);
        }
        else if (!strcmp(argv[i], "-observe-pointees-cap") && i + 1 < argc) {
            g_obs_cap = (uint32_t)strtoul(argv[++i], nullptr, 0);
            if (g_obs_cap < 4 || g_obs_cap > (16u << 20)) { fprintf(stderr, "invalid -observe-pointees-cap (4..16 MB)\n"); return 2; }
        }
        else if (!strcmp(argv[i], "-runlist") && i + 1 < argc) load_runlist(argv[++i]);
        else if (!strcmp(argv[i], "-dsiout") && i + 1 < argc) {
            if (!(dsiout = fopen(argv[++i], "a"))) { fprintf(stderr, "cannot write -dsiout %s\n", argv[i]); return 2; }
        }
        else if (!strcmp(argv[i], "-mutrec") && i + 1 < argc) {
            if (!(g_mutrec = fopen(argv[++i], "w"))) { fprintf(stderr, "cannot write -mutrec %s\n", argv[i]); return 2; }
        }
        else if (!strcmp(argv[i], "-spec") && i + 1 < argc) load_specs(argv[++i]);
        else if (!strcmp(argv[i], "-v")) g_verbose = ++verbose;
        else if (!strcmp(argv[i], "-noclobber")) V.clobber = false;
        else if (!strcmp(argv[i], "-strictnan")) g_strict_nan = true;
        else if (!strcmp(argv[i], "-nulltrap")) g_nulltrap = g_nulltrap_cli = true;
        else if (!strcmp(argv[i], "-p0exclude")) g_p0exclude = true;
        else if (!strcmp(argv[i], "-fastreset")) g_fastreset = true;
        else if (!strcmp(argv[i], "-nofastreset")) g_fastreset = false;
        else if (!strcmp(argv[i], "-snapcheck") && i + 1 < argc) {
            char* e; g_snapcheck = strtol(argv[++i], &e, 10);
            if (*e || g_snapcheck < 0) { fprintf(stderr, "invalid -snapcheck N (N >= 0)\n"); return 2; }
        }
        else if (!strcmp(argv[i], "-calleesp")) g_calleesp_all = true;
        else if (!strcmp(argv[i], "-footprint")) { g_footprint_all = true; g_calleesp_all = true; }
        else if (!strcmp(argv[i], "-trace") && i + 1 < argc) { trace_seed = strtoull(argv[++i], nullptr, 0); trace_on = true; }
        else if (!strcmp(argv[i], "-noprogress")) prog_off = true;
        else if (!strcmp(argv[i], "-progress-interval") && i + 1 < argc) prog_interval = argv[++i];
        else if (!strcmp(argv[i], "-stuck") && i + 1 < argc) prog_stuck = argv[++i];
        else if (!strcmp(argv[i], "-hanglimit") && i + 1 < argc) prog_hang = argv[++i];
        else if (!strcmp(argv[i], "-threadsched") && i + 1 < argc) { /* `osmem ... threads`: the perturbation seed (0 = default order) */
            char* e; thr_sched_cli = strtoull(argv[++i], &e, 0); thr_sched_set = true;
            if (*e) { fprintf(stderr, "invalid -threadsched N\n"); return 2; }
        }
        else if (!strcmp(argv[i], "-gthrselftest") && i + 1 < argc) return gthr::selftest(argv[++i]);
        else { fprintf(stderr, "unknown option %s\n", argv[i]); return 2; }
    }
    if (g_observe_pointees) { /* static-object boundaries (verify.py prepare_shared: mkstatics.py) */
        FILE* sf = fopen("build/verify/statics.tsv", "r");
        if (!sf) { fprintf(stderr, "-observe-pointees: build/verify/statics.tsv is missing (verify.py builds it: tools/verify/mkstatics.py)\n"); return 2; }
        unsigned sa;
        while (fscanf(sf, "%x", &sa) == 1) g_statics.push_back(sa);
        fclose(sf);
        std::sort(g_statics.begin(), g_statics.end());
    }
    if (thr_sched_set) gthr::sched_seed = thr_sched_cli; /* -threadsched overrides the spec's threadsched= */
    gthr::announce();
    prog::configure();
    if (prog_off) prog::on = false;
    if (prog_interval) prog::interval = atof(prog_interval);
    if (prog_stuck) prog::stuck = atof(prog_stuck);
    if (prog_hang) { prog::hang_limit = atof(prog_hang); prog::hang_explicit = true; }
    load_image(image);
    for (CopySpec& s : g_copy_specs) { /* copy: take the bytes from the image now that it is loaded */
        if (s.blob) continue;
        const Region* r = image_region(s.src);
        if (!r || s.src - r->addr + (uint64_t)s.size > r->size) {
            fprintf(stderr, "copy: %08X..+0x%X is not inside one image region\n", s.src, s.size);
            return 2;
        }
        s.data.assign(s.size, 0);
        uint32_t o = s.src - r->addr;
        for (uint32_t k = 0; k < s.size; k++) s.data[k] = o + k < r->data.size() ? r->data[o + k] : 0;
    }

    if (g_p0exclude && g_nulltrap && g_nulltrap_unit && !g_nulltrap_cli && refc::mode != refc::READ) {
        /* a unit with the `nulltrap` directive traps page 0 in every run, so nulltrap_compare's plain side is a -nulltrap run
         * too: no page-0 access completes, there is nothing to note, and the option is moot (2026-10-31) */
        fprintf(stderr, "p0exclude: off: the unit's `nulltrap` directive makes every run trap page 0 (no plain run to exclude from)\n");
        g_p0exclude = false;
    }
    if (g_p0exclude && (g_nulltrap || refc::mode == refc::READ)) { /* the note needs the original's live run without traps */
        fprintf(stderr, "-p0exclude needs a run without -nulltrap and without -refcache (the original runs live)\n");
        return 2;
    }
    if (snapc::mode != snapc::OFF) { /* -snapcache: only where the setups' live and cached paths are the same by construction */
        const char* off = !g_fastreset ? "-nofastreset" : g_fastreset_check ? "WWHD_VERIFY_FASTRESET_CHECK" : trace_on ? "-trace"
                        : g_watch_len ? "-watch" : dsiout ? "-dsiout" : g_mutrec ? "-mutrec" : nullptr;
        if (off) {
            fprintf(stderr, "snapcache: off under %s (the setups run live)\n", off);
            snapc::mode = snapc::OFF;
        } else
            snapc::startup = snapc::startup_hash();
    }
    g_snapc_check = snapc::check && snapc::mode == snapc::READ;

    std::vector<gabi::Candidate*> cands;
    for (gabi::Candidate* c = gabi::g_cands; c; c = c->next) cands.push_back(c);
    std::sort(cands.begin(), cands.end(), [](auto* a, auto* b) { return a->addr < b->addr; });
    if (list_only) {
        for (gabi::Candidate* c : cands) printf("%08X\n", c->addr);
        return 0;
    }
    if (list_fn) { /* the entry thunk's address as the linker map gives it (runtime address minus the ASLR slide) */
        intptr_t slide = _dyld_get_image_vmaddr_slide(0);
        for (gabi::Candidate* c : cands) printf("%08X %llX\n", c->addr, (unsigned long long)((uintptr_t)c->fn - (uintptr_t)slide));
        return 0;
    }
    /* WWHD_VERIFY_CACHE_MARKS=1 (verify.py's result cache, vcache.py): each function's stdout block and stderr notes are
     * framed by `@VC< ADDR` / `@VC> ADDR` lines on both streams, so verify.py can attribute them; it removes the lines */
    const bool vc_marks = getenv("WWHD_VERIFY_CACHE_MARKS") && !strcmp(getenv("WWHD_VERIFY_CACHE_MARKS"), "1");
    auto vc_mark = [&](char k, uint32_t a) {
        if (!vc_marks) return;
        fflush(stdout);
        printf("@VC%c %08X\n", k, a);
        fflush(stdout);
        fprintf(stderr, "@VC%c %08X\n", k, a);
    };

    if (forkserver) {
        /* mutate.py --fork (2026-10-26): the start-up above (image, specs, static initialisers) runs once; each stdin
         * line "RUN K OUT RUNLIST" forks a child that makes mutant K live, takes RUNLIST ("-": none) as -runlist and
         * writes OUT.out / OUT.err, then runs the functions as a normal start would. The server answers "PID p" and,
         * when the child has ended, "DONE rc" (exit code, or minus the signal). First line: "READY k,..." - the schema
         * sites the static initialisers ran (env WWHD_MUT_REC=1): those mutants need a fresh process, not this one. */
        printf("READY");
        for (size_t k = 0; k < g_mut_nhits; k++) printf(k ? ",%d" : " %d", g_mut_hits[k]);
        printf("\n");
        fflush(stdout);
        mut_flush("");
        wwhd_mut_recording = 0;
        bool child = false;
        char line[9000];
        while (fgets(line, sizeof line, stdin)) {
            int act;
            char outp[4096], rl[4096];
            if (sscanf(line, "RUN %d %4095s %4095s", &act, outp, rl) != 3) { printf("ERR %s", line); fflush(stdout); continue; }
            fflush(stdout);
            fflush(stderr);
            pid_t pid = fork();
            if (pid == 0) {
                child = true;
                wwhd_mut_active = act;
                g_runlist.clear();
                g_have_runlist = false;
                if (strcmp(rl, "-")) load_runlist(rl);
                std::string o = std::string(outp) + ".out", e = std::string(outp) + ".err";
                int fo = open(o.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644), fe = open(e.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fo < 0 || fe < 0) _exit(3);
                dup2(fo, 1);
                dup2(fe, 2);
                close(fo);
                close(fe);
                int fi = open("/dev/null", O_RDONLY);
                if (fi >= 0) { dup2(fi, 0); close(fi); }
                break;
            }
            if (pid < 0) { printf("DONE -255\n"); fflush(stdout); continue; }
            printf("PID %d\n", (int)pid);
            fflush(stdout);
            int st = 0;
            while (waitpid(pid, &st, 0) < 0 && errno == EINTR) {}
            printf("DONE %d\n", WIFEXITED(st) ? WEXITSTATUS(st) : WIFSIGNALED(st) ? -WTERMSIG(st) : -255);
            fflush(stdout);
        }
        if (!child) return 0;
    }
    int total_fail = 0;
    int shard_rows = 0; /* -shardout: @SHARD lines printed (the closing @SHARDEND line repeats the count) */
    mut_flush("G");
    bool stop_all = false;
    {
        long nf = 0;
        int glo0 = range_lo, ghi0 = range_hi < 0 ? ngen : std::min(range_hi, ngen);
        for (gabi::Candidate* c : cands)
            if ((only.empty() || only.count(c->addr)) && (!g_have_runlist || g_runlist.count(c->addr))) nf++;
        prog::all_total.store(nf * (long)std::max(0, ghi0 - glo0));
        prog::start(nf);
    }
    refc::Writer refc_w;
    refc::Reader refc_r;
    snapc::ValWriter snapc_w;
    snapc::ValReader snapc_r;
    if (!shardout)
        printf("%-8s  %-40s %10s %10s %8s  %s\n", "addr", "function", "generated", "recorded", "coverage", "first difference");
    for (gabi::Candidate* cand : cands) {
        if (!only.empty() && !only.count(cand->addr)) continue;
        if (stop_all) break;
        const RunSel* sel = nullptr;
        if (g_have_runlist) {
            auto it = g_runlist.find(cand->addr);
            if (it == g_runlist.end()) continue;
            sel = &it->second;
        }
        vc_mark('<', cand->addr);
        prog::func.store(cand->addr);
        prog::name.store(cand->name);
        prog::fidx.fetch_add(1);
        prog::done.store(0);
        prog::total.store(std::max(0, (range_hi < 0 ? ngen : std::min(range_hi, ngen)) - range_lo));
        prog::phase.store(0);
        auto selected = [&](long idx) { return !sel || sel->all || sel->idx.count(idx); };
        char mhead[48];
        bool row_done = false; /* -stopfirst: the row's first difference is known (its printed result is fixed) */
        memstat_func(cand->addr);
        const OrigFunc* of = nullptr;
        for (unsigned i = 0; i < unit_nfuncs; i++)
            if (unit_funcs[i].addr == cand->addr) of = &unit_funcs[i];
        if (!of) { printf("%08X  %-40s  no original in unit\n", cand->addr, cand->name); total_fail++; vc_mark('>', cand->addr); continue; }
        std::vector<uint32_t> cov(of->nblocks + 1);
        g_cur_func = cand->addr;
        g_cur_func_fp = cand->addr;
        int gpass = 0, gn = 0, rpass = 0, rn = 0, rbad = 0, aborted = 0;
        int p0x = 0;              /* -p0exclude: differences after the original's page-0 access, excluded */
        std::string p0x_first;
        int orig_dsi = 0;          /* -nulltrap: inputs on which the original itself took a page-0 DSI (a fixture defect) */
        /* `osmem ... threads`: the original side's schedule digests combined over the inputs (receipts), how many inputs
         * created threads, and the inputs where only the original faulted or deadlocked off the main thread (no evidence) */
        uint64_t thr_digest = 0x51ED5EED5EEDull, thr_inputs = 0, thr_switch = 0, thr_preempt = 0;
        uint32_t thr_max = 0;
        std::set<uint32_t> thr_on_stack; /* unit functions on a guest thread's stack at the tested call: original code in this row */
        int thr_noev = 0;
        std::string thr_noev_first;
        std::vector<long> dsi_idx, dsi_pass; /* -dsiout: inputs a DSI ended on either side; those of them that compared equal */
        int prerun_dsi = 0;        /* -dsiout: dictionary pre-runs a DSI ended (the dictionary, hence every input, may differ) */
        bool prerun_same = false;  /* -dsiout: ... but pre-run without -nulltrap they build the same dictionary */
        g_dsi_order = 0;
        std::string orig_dsi_first;
        std::string first;
        bool first_p0 = false; /* the shown first difference is on an input where the original faulted on page 0 */
        long first_idx = -1, cur_idx = 0; /* input index of the first difference (generated i, then ngen + recorded k) */
        /* this input runs its setups for real (SNAP_VALIDATE) only because it is the first input of this run (a -range part:
         * mutate.py's sharded baseline, 2026-10-31; a -runlist subset). Its original side then differs from the run a full
         * process makes of it (the setups' stack bytes enter V.orig_stack): -refcache-write leaves it unrecorded and
         * -refcache runs its original live. Inputs 0 and every -snapcheck'th input validate in every run and are recorded
         * and read as before */
        bool refc_skip = false;
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
            refc::Rd rr;
            std::string rbuf;
            /* (refc_skip: a validation input other runs make a plain input, e.g. the first input of a -runlist subset:
             * its record, made where it was plain, lacks the setups' stack bytes the live original side leaves; run live) */
            const bool cached = !rec && !(refc_skip && refc_r.ok && ++refc_r.live) && refc_r.find(cur_idx, V.seed, &rr, rbuf);
            const bool dgst = cached && refc_r.digest && !refc::check; /* a digest record (-refcache-digest): see refc */
            std::vector<uint32_t> dmasks;
            uint64_t dh[2] = {0, 0};
            bool dpend = false; /* -refcache-write -refcache-digest: the record is added after the candidate's run */
            refc::Side dsd;
            if (cached && !refc::check) { /* -refcache: the original's run on this input, as the baseline recorded it */
                refc::Side sd;
                if (!(dgst ? refc::get_digest(rr, &a, &sd, &dmasks, dh, cov.size()) : refc::get_record(rr, &a, &sd, cov.size()))) {
                    fprintf(stderr, "refcache: %08X input %ld: unreadable record\n", cand->addr, cur_idx);
                    exit(97);
                }
                refc::apply(a, sd, cov);
            } else {
                const bool keep = !rec && (refc_w.on || cached);
                std::vector<uint32_t> cov0;
                refc::Stats st0;
                if (keep) { cov0 = cov; st0.take(); }
                run(of->fn, in, false, &a, &cov);
                if (keep) {
                    refc::Side sd = refc::side_of_run(cov0, cov, st0);
                    const bool live_only = (a.aborted && is_env_limit(a.why)) || refc_skip ||
                                           a.thr.used; /* `threads`: a record holds no guest-thread state: such inputs run live */
                    if (refc::digest && refc_w.on && !live_only) { /* the masks need the candidate's calls */
                        dpend = true;
                        dsd = sd;
                    } else
                        refc_w.add((int)cur_idx, V.seed, live_only, a, sd);
                    if (cached && refc_r.digest) { /* -refcache-check on a digest record: the exact part, the side and the
                                                     * heavy hash under the recorded masks equal the live run's */
                        Result ra;
                        refc::Side rs;
                        refc::Rd r2 = rr;
                        std::vector<uint32_t> rm;
                        uint64_t rh[2], lh[2] = {1, 1};
                        bool dec = refc::get_digest(r2, &ra, &rs, &rm, rh, cov.size()) && rm.size() == a.calls.size();
                        refc::Buf l1, l2, s1, s2;
                        if (dec) {
                            refc::put_lite(l1, a); refc::put_lite(l2, ra); refc::put_side(s1, sd); refc::put_side(s2, rs);
                            refc::heavy_hash(a, rm, lh);
                        }
                        if (!dec || l1.s != l2.s || s1.s != s2.s || lh[0] != rh[0] || lh[1] != rh[1]) {
                            fprintf(stderr, "refcache: CHECK FAILED %08X input %ld: the recorded original (digest) differs from the "
                                    "live run (%s)\n", cand->addr, cur_idx, !dec ? "unreadable" : l1.s != l2.s ? "exact part" :
                                    s1.s != s2.s ? "side" : "heavy hash");
                            exit(97);
                        }
                    } else if (cached) { /* -refcache-check: the record equals the live run, and decodes back to the same bytes */
                        refc::Buf live, again;
                        refc::put_record(live, a, sd);
                        Result ra;
                        refc::Side rs;
                        refc::Rd r2 = rr;
                        bool dec = refc::get_record(r2, &ra, &rs, cov.size());
                        if (dec) refc::put_record(again, ra, rs);
                        if ((size_t)(rr.e - rr.p) != live.s.size() || memcmp(rr.p, live.s.data(), live.s.size()) || !dec ||
                            again.s != live.s) {
                            fprintf(stderr, "refcache: CHECK FAILED %08X input %ld: the recorded original differs from the live run\n",
                                    cand->addr, cur_idx);
                            exit(97);
                        }
                    }
                }
            }
            if (rec) {
                std::string why = replay_check(a, in);
                if (!why.empty()) {
                    rbad++;
                    if (verbose) printf("    unusable %s: %s\n", in.label.c_str(), why.c_str());
                    return;
                }
            }
            if (a.dsi && orig_dsi++ == 0) {
                char db[160];
                snprintf(db, sizeof db, "%s %08X, depth %d in=%08X {%s}", dsi_what(a.dsi_kind), a.dsi_ea, a.dsi_depth,
                         a.dsi_in, in.label.c_str());
                orig_dsi_first = db;
            }
            if (g_nulltrap) { /* the candidate-only fetch rule: the original's non-code targets of this input */
                g_orig_badfetch.clear();
                for (uint32_t t : a.badfetch) g_orig_badfetch[t]++;
                g_orig_badfetch_on = true;
            }
            run(cand->fn, in, true, &b, nullptr);
            g_orig_badfetch_on = false;
            if (dpend) refc_w.add((int)cur_idx, V.seed, false, a, dsd, &b);
            if (dgst && ((a.mrej || b.mrej) || !(a.aborted || b.aborted))) { /* the comparison needs the original's heavy part */
                bool hit = !a.mrej && !b.mrej && verbose <= 2 && !refc::digest_miss && b.calls.size() == dmasks.size();
                for (size_t k = 0; k < dmasks.size() && hit; k++) hit = refc::call_mask(a.calls[k], &b.calls[k]) == (dmasks[k] & 0xFFFF);
                if (hit) {
                    uint64_t hb[2];
                    refc::heavy_hash(b, dmasks, hb);
                    hit = hb[0] == dh[0] && hb[1] == dh[1];
                }
                if (hit) { /* the original's heavy part is the candidate's */
                    refc_r.dmatch++;
                    BigVec<CallEv> cs(b.calls.size());
                    for (size_t k = 0; k < cs.size(); k++) {
                        refc::copy_call(cs[k], b.calls[k]);
                        cs[k].kind = a.calls[k].kind; cs[k].lr = a.calls[k].lr; cs[k].notarg = a.calls[k].notarg;
                        cs[k].nint = cs[k].nflt = 0;
                    }
                    a.calls = std::move(cs);
                    a.net = b.net; a.fwords = b.fwords; a.dwords = b.dwords; a.final_bytes = b.final_bytes;
                } else { /* run the original live now; its coverage and statistics came from the record already */
                    refc_r.dlive++;
                    refc::Stats keep;
                    keep.take();
                    std::vector<uint32_t> cov_tmp = cov;
                    Result al;
                    run(of->fn, in, false, &al, &cov_tmp);
                    refc::restore_stats(keep);
                    a = std::move(al);
                }
            } else if (dgst)
                refc_r.dlite++;
            /* -snapcache-write: an input that hit a time limit (load dependent; the setups' CPU time counts there) keeps
             * no validation state */
            if (g_snapc_out && ((a.aborted && is_env_limit(a.why)) || (b.aborted && is_env_limit(b.why)))) g_snapc_out->ok = false;
            if (prog::on && ((a.aborted && is_time_limit(a.why)) || (b.aborted && is_time_limit(b.why)))) {
                bool ha = a.aborted && is_time_limit(a.why), hb = b.aborted && is_time_limit(b.why);
                prog::hangs.fetch_add(1);
                char hl[1024];
                double la[1] = {0};
                getloadavg(la, 1);
                snprintf(hl, sizeof hl, "[progress] HANG %08X %s %s (input %ld): %s side%s: %s (1-min load %.0f); %s\n", cand->addr,
                         cand->name, in.label.c_str(), cur_idx, ha && hb ? "both" : ha ? "original" : "candidate", ha && hb ? "s" : "",
                         (ha ? a.why : b.why).c_str(), la[0],
                         ha ? "the input drives the original into a loop (domain/fixture issue: steer the inputs)"
                            : "only the candidate loops: a FAIL row");
                prog::say(hl, true);
            }
            const std::string md = model_compare(a, b, in); /* mock-model rejections: compared calls */
            if (gthr::on) {
                if (a.thr.used) {
                    thr_digest = gthr::mixh(thr_digest ^ gthr::mixh(a.thr.digest + (uint64_t)cur_idx));
                    thr_inputs++;
                    thr_switch += a.thr.nswitch;
                    thr_preempt += a.thr.npreempt;
                    thr_max = std::max(thr_max, a.thr.nthreads);
                    thr_on_stack.insert(a.thr.on_stack.begin(), a.thr.on_stack.end());
                }
                /* off the main thread, a fault or deadlock of the original only: the input ends with no evidence */
                const bool odsi = a.dsi && !b.dsi && a.dsi_thread > 0, odl = a.aborted && !b.aborted && a.thr.deadlock;
                if (odsi || odl) {
                    if (thr_noev++ == 0)
                        thr_noev_first = (odsi ? "page-0 fault in guest thread T" + std::to_string(a.dsi_thread) : a.why) + "  {" + in.label + "}";
                    aborted++;
                    return;
                }
            }
            if (a.aborted || b.aborted) {
                if (a.aborted && b.aborted && md.empty()) { aborted++; return; }
            }
            std::string d = !md.empty() ? md
                          : a.aborted != b.aborted ? std::string("only one side aborted: ") + (a.aborted ? a.why : b.why)
                                                   : compare(a, b, eff_ret, in);
            if (gthr::on && d.empty() && !a.aborted && !b.aborted) d = gthr::compare(a.thr, b.thr); /* the guest threads' call logs */
            if (gthr::on && b.dsi && !a.dsi && b.dsi_thread > 0) d += " [in guest thread T" + std::to_string(b.dsi_thread) + "]";
            if (g_p0exclude && a.p0 && !d.empty()) { /* after the original's page-0 access: no evidence (zero-page rule) */
                if (p0x++ == 0) p0x_first = d + "  {" + in.label + "}";
                aborted++;
                return;
            }
            if (a.dsi || b.dsi) {
                dsi_idx.push_back(cur_idx);
                if (d.empty()) dsi_pass.push_back(cur_idx);
            }
            if (!d.empty() && b.stray && rec) d += " [candidate read unrecorded memory]";
            (rec ? rn : gn)++;
            if (d.empty()) (rec ? rpass : gpass)++;
            else {
                /* an input where the original itself took a page-0 DSI never kills (zero-page rule, cascade.py): its
                 * difference is shown only until a clean one turns up, and -stopfirst does not stop on it (2026-11-01:
                 * a pre-fault call difference on such an input ended every run of a mutant, so the clean inputs were
                 * never compared and the mutant stayed alive as difference-page0) */
                const bool orig_fault = a.dsi;
                if (first.empty() || (first_p0 && !orig_fault)) {
                    /* the marker tells cascade.row_kills and the shard merge (verify.py) what the text alone may not: when both
                     * sides fault at the same access, the shown difference has no page-0 wording (2026-11-02) */
                    first = d + (orig_fault ? " [original page-0 fault on this input: no evidence]" : "") + "  {" + in.label + "}";
                    first_idx = cur_idx; first_p0 = orig_fault;
                    if (stopfirst && !orig_fault) {
                        row_done = true;
                        if (a.aborted == b.aborted || !md.empty()) stop_all = true; /* an output difference: the mutant is killed */
                    }
                }
                if (verbose) printf("    FAIL %s: %s\n", in.label.c_str(), d.c_str());
                if (verbose > 2) { /* full net effects of both sides */
                    for (auto& kv : a.net) printf("      orig %08X=%02X%s\n", kv.first, kv.second, b.net.count(kv.first) && b.net.at(kv.first) == kv.second ? "" : "  <");
                    for (auto& kv : b.net) printf("      cand %08X=%02X%s\n", kv.first, kv.second, a.net.count(kv.first) && a.net.at(kv.first) == kv.second ? "" : "  <");
                }
            }
        };
        /* `setupsnapshot`: capture the function's post-setup state (twice, under two input seeds) before its inputs */
        g_snap_mode = SNAP_OFF;
        tmpl_destroy(); /* (none: the previous function destroyed its own) */
        g_snap = SetupState();
        bool snap_ok = false;
        uint64_t snapc_hash = 0; /* -snapcache: the payload hash of the function's snapshot file (0: none) */
        SetupState snapc_loaded;
        uint64_t snapc_seed = 0;
        snapc::Load snapc_st = snapc::MISSING;
        if (g_setup_snapshot && ngen && snapc::mode == snapc::READ &&
            std::any_of(g_setup_specs.begin(), g_setup_specs.end(), [&](const SetupSpec& su) { return !su.func || su.func == cand->addr; })) {
            snapc_st = snapc::load_snap(cand->addr, &snapc_loaded, &snapc_seed, &snapc_hash);
            if (snapc_st != snapc::OK) snapc_hash = 0;
            if (snapc_st == snapc::CORRUPT) {
                fprintf(stderr, "snapcache: the snapshot of %08X is unreadable%s\n", cand->addr,
                        snapc::check ? "" : "; its setups run live");
                if (snapc::check) exit(98);
            } else if (snapc_st == snapc::CONFIG)
                fprintf(stderr, "snapcache: the snapshot of %08X does not match this run's configuration; its setups run live\n", cand->addr);
            else if (snapc_st == snapc::MISSING)
                fprintf(stderr, "snapcache: no snapshot for %08X; its setups run live\n", cand->addr);
        }
        if (snapc_st == snapc::OK && !snapc::check) { /* -snapcache: the baseline's accepted snapshot replaces the capture runs */
            g_snap = std::move(snapc_loaded);
            snap_ok = true;
            snapc::fn_cached++;
            if (g_fastreset) {
                clear_all();
                tmpl_build(g_snap);
            }
            if (verbose) fprintf(stderr, "setupsnapshot: %08X captured (%zu bytes)\n", cand->addr, g_snap.nbytes);
        } else if (g_setup_snapshot && ngen && std::any_of(g_setup_specs.begin(), g_setup_specs.end(),
                                                    [&](const SetupSpec& su) { return !su.func || su.func == cand->addr; })) {
            if (snapc::mode == snapc::READ) snapc::fn_live++;
            uint64_t sc = g_set_calls, sb = g_set_bytes, su = g_surface_calls; /* the check runs leave no statistics */
            g_dict = nullptr;
            SetupState st[2];
            std::string refused = setup_static_dependency(cand->addr);
            for (int k = 0; k < 2 && refused.empty(); k++) {
                clear_all();
                Input in;
                uint64_t sd = seed0 * 1000003ull + 0x80000000ull + (uint64_t)k * 0x9E37ull; /* outside the inputs' seeds */
                make_generated(of, sd, &in);
                V.seed = sd;
                prog::input_begin(1, k);
                V.clobber_now = k % 2 == 0;
                g_snap_mode = SNAP_CAPTURE;
                g_snap_out = &st[k];
                Result a;
                run(of->fn, in, false, &a, nullptr);
                prog::input_end(false);
                g_snap_mode = SNAP_OFF;
                if (a.aborted) refused = "the setups abort: " + a.why;
                else if (!st[k].thr_live.empty()) refused = st[k].thr_live; /* `threads`: contexts are not part of a snapshot */
            }
            if (refused.empty()) {
                std::string detail;
                std::string d = setup_state_diff(st[0], st[1], &detail);
                if (!d.empty()) {
                    refused = "setup result depends on the input: first difference at " + d;
                    if (verbose && !detail.empty()) fprintf(stderr, "setupsnapshot: %08X at %s: %s (two input seeds)\n", cand->addr, d.c_str(), detail.c_str());
                }
            }
            if (refused.empty()) /* an allocfail pick that fails a call made inside the setups is a per-input choice */
                for (auto& kv : st[0].alloc_calls)
                    if (auto af = g_alloc_fail.find(kv.first); af != g_alloc_fail.end())
                        for (uint32_t kk : af->second)
                            if (kk >= 1 && kk <= kv.second && refused.empty()) {
                                char b[160];
                                snprintf(b, sizeof b, "setup result depends on the input: allocfail %08X can fail call %u inside the setups",
                                         kv.first, kk);
                                refused = b;
                            }
            g_set_calls = sc; g_set_bytes = sb; g_surface_calls = su;
            if (snapc_st == snapc::OK) { /* -snapcache-check: the live capture against the cached snapshot */
                std::string d;
                if (!refused.empty()) d = "the live setups refuse the snapshot: " + refused;
                else if (snapc_seed == seed0) { /* same capture seeds: byte for byte */
                    if (snapc::snap_bytes(st[0], seed0) != snapc::snap_bytes(snapc_loaded, snapc_seed)) {
                        std::string dd = setup_state_diff(snapc_loaded, st[0]);
                        d = "the captured state differs" + (dd.empty() ? std::string(" (initial values)") : " at " + dd);
                    }
                } else if (!(d = setup_state_diff(snapc_loaded, st[0])).empty())
                    d = "the captured state differs at " + d;
                if (!d.empty()) {
                    fprintf(stderr, "snapcache: CHECK FAILED %08X: the cached snapshot differs from the live capture (%s)\n", cand->addr,
                            d.c_str());
                    exit(98);
                }
                snapc_loaded = SetupState();
            }
            if (!refused.empty())
                fprintf(stderr, "setupsnapshot: refused (%s)\n", refused.c_str());
            else {
                g_snap = std::move(st[0]);
                snap_ok = true;
                if (snapc::mode == snapc::WRITE) snapc_hash = snapc::write_snap(cand->addr, g_snap, seed0);
                if (g_fastreset) { /* -fastreset: the snapshot becomes the template pages */
                    clear_all();
                    tmpl_build(g_snap);
                }
                if (verbose) fprintf(stderr, "setupsnapshot: %08X captured (%zu bytes)\n", cand->addr, g_snap.nbytes);
            }
        }
        /* dictionary of the original's .rodata constants (pre-runs, not compared) */
        if (snap_ok) g_snap_mode = SNAP_USE;
        auto preruns = [&]() {
            V.dict.clear();
            g_dict = nullptr;
            V.collect = true;
            int dsi = 0;
            for (int i = 0; i < 8 && ngen; i++) {
                clear_all();
                Input in;
                make_generated(of, seed0 * 7777ull + i, &in);
                V.seed = seed0 * 7777ull + i;
                /* fixed, not whatever the process ran last (2026-10-29, AGL#17 027C93A8): the clobber switch was left
                 * by the previous function's last input (its index parity), its setupsnapshot capture, or the
                 * process start (on), so the dictionary, and with it every generated input of this function, depended
                 * on which functions ran before it in the same process (-only scope, -range shards, -runlist, vcache
                 * hits). Off is what nearly every function of a whole-unit run saw before (previous last index odd,
                 * or a snapshot capture) */
                V.clobber_now = false;
                prog::input_begin(2, i);
                Result a;
                const int trace_prev = g_trace; /* -trace SEED also traces the dictionary pre-run with that seed */
                g_trace = trace_on && V.seed == trace_seed;
                if (g_trace) printf("    trace prerun %s (dictionary pre-run %d)\n", in.label.c_str(), i);
                run(of->fn, in, false, &a, nullptr);
                g_trace = trace_prev;
                prog::input_end(false);
                dsi += a.dsi;
            }
            V.collect = false;
            return dsi;
        };
        prerun_dsi = preruns();
        if (dsiout && prerun_dsi) { /* -dsiout: would the run without -nulltrap build the same dictionary (so the same inputs)?
                                     * pre-run it that way too and keep this run's dictionary */
            std::vector<uint32_t> kept = V.dict;
            g_nulltrap = false;
            preruns();
            g_nulltrap = true;
            prerun_same = V.dict == kept;
            V.dict = kept;
        }
        g_dict = &V.dict;
        /* -snapcache: the validation states (keyed by the dictionary the pre-runs just built) */
        snapc_w.begin(cand->addr, seed0, snap_ok ? snapc_hash : 0, range_lo, range_hi < 0 ? ngen : std::min(range_hi, ngen), ngen);
        snapc_r.clear();
        if (snap_ok && snapc_hash && snapc::mode == snapc::READ && !snapc_r.open(cand->addr, seed0, snapc_hash) && snapc::check) exit(98);
        int glo = range_lo, ghi = range_hi < 0 ? ngen : std::min(range_hi, ngen);
        /* -refcache-write: only a run of every generated input (of its -range shard, 2026-10-30: a part file that
         * verify.py joins); -refcache: any selection (records are per input). -dsiout (nulltrap_compare's -nulltrap side,
         * which writes the gate's records for survivor replays) writes but never reads: its rows are a gate */
        if (!g_mutrec) {
            if ((!sel || sel->all) && !trace_on && (glo < ghi || (glo == 0 && ghi == ngen))) refc_w.begin(cand->addr, ngen, seed0, of->nblocks, glo, ghi);
            if (!dsiout) refc_r.open(cand->addr, ngen, seed0, of->nblocks);
        }
        snprintf(mhead, sizeof mhead, "S %08X", cand->addr);
        mut_flush(mhead);
        bool first_run = true;
        for (int i = glo; i < ghi && !row_done; i++) {
            if (!selected(i)) continue;
            cur_idx = i;
            clear_all();
            Input in;
            make_generated(of, seed0 * 1000003ull + i, &in);
            V.seed = seed0 * 1000003ull + i;
            V.clobber_now = i % 2 == 0;
            g_trace = trace_on && V.seed == trace_seed;
            if (g_trace) printf("    trace %s (clobber %d)\n", in.label.c_str(), V.clobber_now);
            refc_skip = false;
            if (snap_ok) { /* the first input of the run and every 500th run the setups for real and check the snapshot */
                g_snap_mode = first_run || (g_snapcheck && i % g_snapcheck == 0) ? SNAP_VALIDATE : SNAP_USE;
                g_snap_input = i;
                refc_skip = g_snap_mode == SNAP_VALIDATE && i != 0 && !(g_snapcheck && i % g_snapcheck == 0);
            }
            first_run = false;
            SnapRec snapc_rec;
            if (snap_ok && g_snap_mode == SNAP_VALIDATE) {
                if (snapc::mode == snapc::READ) {
                    bool bad;
                    V.entry_sp = in.entry.r[1]; /* (as run() sets them: the record's prediction reads initial_byte()) */
                    V.stack_lo = in.entry.r[1] - 0x10000;
                    g_snapc_rec = snapc_r.find(i, &bad);
                    if (bad) {
                        fprintf(stderr, "snapcache: the validation state of %08X input %d is unreadable%s\n", cand->addr, i,
                                snapc::check ? "" : "; its setups run live");
                        if (snapc::check) exit(98);
                    }
                    if (snapc_hash) (g_snapc_rec && !snapc::check ? snapc::val_cached : snapc::val_live)++; /* (-snapcache-check: live, compared) */
                } else if (snapc_w.on)
                    g_snapc_out = &snapc_rec;
            }
            prog::input_begin(3, i);
            one(in, false);
            prog::input_end(true);
            if (g_snapc_out) snapc_w.add(i, snapc_rec);
            g_snapc_out = nullptr;
            g_snapc_rec = nullptr;
            snprintf(mhead, sizeof mhead, "I %08X %d", cand->addr, i);
            mut_flush(mhead);
            g_trace = 0;
        }
        refc_w.finish(!row_done && !stop_all);
        refc_r.ok = false;
        snapc_w.finish(); /* (records are per input: a run cut short keeps the ones it made) */
        snapc_r.clear();
        g_snap_mode = SNAP_OFF;
        tmpl_destroy();
        /* the function's snapshot and kept validation state end with it (2026-11-02): they were held until the next
         * function with a snapshot replaced them (~70 MB on the J3D lighting specs, through every function without one) */
        g_snap = SetupState();
        g_valstate = ValState();
        g_dict = nullptr;
        if (recdir && range_lo == 0) {
            std::string d = std::string(recdir) + "/" + hex32(cand->addr);
            if (DIR* dir = opendir(d.c_str())) {
                std::vector<std::string> files;
                while (dirent* e = readdir(dir))
                    if (strstr(e->d_name, ".tap")) files.push_back(d + "/" + e->d_name);
                closedir(dir);
                std::sort(files.begin(), files.end());
                prog::total.fetch_add((long)files.size());
                prog::all_total.fetch_add((long)files.size());
                long ridx = 0;
                for (auto& fpath : files) {
                    cur_idx = ngen + ridx++;
                    if (row_done) break;
                    if (!selected(cur_idx)) continue;
                    clear_all();
                    Input in;
                    std::string why;
                    V.seed = 77;
                    prog::input_begin(4, cur_idx);
                    if (!load_recorded(fpath.c_str(), &in, &why)) {
                        prog::input_end(false);
                        rbad++;
                        if (verbose) printf("    unusable %s: %s\n", fpath.c_str(), why.c_str());
                        continue;
                    }
                    one(in, true);
                    prog::input_end(true);
                    snprintf(mhead, sizeof mhead, "I %08X %ld", cand->addr, cur_idx);
                    mut_flush(mhead);
                }
            }
        }
        if (g_dsi_order)
            fprintf(stderr, "nulltrap: %08X: %d input(s) ended in page-0 faults on both sides at different accesses (compared up to the "
                    "common call prefix only)\n", cand->addr, g_dsi_order);
        if (p0x) /* -p0exclude: one line per process (shard); nulltrap_compare sums them */
            fprintf(stderr, "p0exclude: %08X: %d difference(s) after the original's page-0 access excluded (zero-page rule); "
                    "first: %s\n", cand->addr, p0x, p0x_first.c_str());
        if (gthr::on && thr_inputs) /* `osmem ... threads`: the schedule receipt (original side) */
            fprintf(stderr, "guestthreads: %08X: schedule digest %016llX over %llu input(s) with guest threads (threadsched %llu, "
                    "up to %u threads, %llu switches, %llu slice preemptions)\n", cand->addr, (unsigned long long)thr_digest,
                    (unsigned long long)thr_inputs, (unsigned long long)gthr::sched_seed, thr_max, (unsigned long long)thr_switch,
                    (unsigned long long)thr_preempt);
        if (!thr_on_stack.empty()) { /* their frames stay original to the end: a survivor of a mutant in them is no evidence from this row */
            std::string l;
            for (uint32_t a : thr_on_stack) { char b[12]; snprintf(b, sizeof b, " %08X", a); l += b; }
            fprintf(stderr, "threads: %08X: unit function(s) on a thread stack at the tested call:%s\n", cand->addr, l.c_str());
        }
        if (thr_noev)
            fprintf(stderr, "guestthreads: %08X: %d input(s) where only the original faulted or deadlocked off the main thread (no "
                    "evidence); first: %s\n", cand->addr, thr_noev, thr_noev_first.c_str());
        if (orig_dsi) /* not hidden by both sides faulting alike: the inputs (fixtures) drive the original into page 0 */
            fprintf(stderr, "nulltrap: %08X: the original took a page-0 DSI on %d input(s); first: %s\n", cand->addr, orig_dsi,
                    orig_dsi_first.c_str());
        if (dsiout) { /* nulltrap_compare.py: a run without -nulltrap needs only these inputs (2026-10-27); `#` lines are
                       * comments to -runlist. Without a page-0 DSI a -nulltrap run is the run without it (only page-0
                       * accesses trap), so an input never listed here gives the same result either way */
            auto list = [&](const std::vector<long>& v) {
                for (size_t k = 0; k < v.size(); k++) fprintf(dsiout, k ? ",%ld" : "%ld", v[k]);
            };
            if (prerun_dsi) fprintf(dsiout, "# %08X prerun %d%s\n", cand->addr, prerun_dsi, prerun_same ? " same" : "");
            if (prerun_dsi && !prerun_same) fprintf(dsiout, "%08X all\n", cand->addr);
            else if (!dsi_idx.empty()) { fprintf(dsiout, "%08X ", cand->addr); list(dsi_idx); fprintf(dsiout, "\n"); }
            if (!dsi_pass.empty()) { fprintf(dsiout, "# %08X pass ", cand->addr); list(dsi_pass); fprintf(dsiout, "\n"); }
            fprintf(dsiout, "# %08X done\n", cand->addr);
            fflush(dsiout);
        }
        uint32_t hit = 0;
        for (uint32_t k = 0; k < of->nblocks; k++) hit += cov[k] > 0;
        if (shardout) { /* @SHARD addr gpass gn rpass rn rbad aborted first_idx nblocks covered-blocks | name | first */
            printf("@SHARD %08X %d %d %d %d %d %d %ld %u ", cand->addr, gpass, gn, rpass, rn, rbad, aborted, first_idx, of->nblocks);
            bool any = false;
            for (uint32_t k = 0; k < of->nblocks; k++) if (cov[k]) { printf(any ? ",%u" : "%u", k); any = true; }
            printf("%s|%s|%s\n", any ? "" : "-", cand->name, first.c_str());
            shard_rows++;
            vc_mark('>', cand->addr);
            fflush(stdout); /* a shard that dies later keeps the rows it finished */
            continue;
        }
        char g[32], r[32], cv[32];
        snprintf(g, sizeof g, "%d/%d", gpass, gn);
        snprintf(r, sizeof r, rbad ? "%d/%d(-%d)" : "%d/%d", rpass, rn, rbad);
        snprintf(cv, sizeof cv, "%u/%u", hit, of->nblocks);
        bool ok = gpass == gn && rpass == rn && ((gn + rn) > 0 || g_have_runlist);
        if (!ok) total_fail++;
        printf("%08X  %-40.40s %10s %10s %8s  %s%s\n", cand->addr, cand->name, g, r, cv, ok ? "ok" : first.c_str(),
               aborted ? " (some inputs aborted on both sides)" : "");
        if (verbose > 1) {
            printf("    uncovered blocks:");
            for (uint32_t k = 0; k < of->nblocks; k++) if (!cov[k]) printf(" %u", k);
            printf("\n");
        }
        vc_mark('>', cand->addr);
    }
    if (shardout) { /* the end marker: verify.py rejects a shard whose output lacks it (a shard that
                     * died or was cut short) instead of merging a partial table; 1 = stopped early on purpose (-stopfirst) */
        printf("@SHARDEND %d %d\n", shard_rows, stop_all ? 1 : 0);
        fflush(stdout);
    }
    prog::phase.store(5);
    prog::finish(); /* before the closing stderr lines: no progress line can land inside them */
    if (!g_set_specs.empty() || !g_surface_size.empty())
        fprintf(stderr, "semantic effects: set calls=%llu bytes=%llu, surface calls=%llu\n", (unsigned long long)g_set_calls,
                (unsigned long long)g_set_bytes, (unsigned long long)g_surface_calls);
    if (!g_move_specs.empty() || !g_utf16_specs.empty()) {
        fprintf(stderr, "semantic effects: move calls=%llu bytes=%llu, utf16 failure=%llu lengths=",
                (unsigned long long)g_move_calls, (unsigned long long)g_move_bytes,
                (unsigned long long)g_utf16_fail);
        for (unsigned i = 0; i <= 4096; i++)
            if (g_utf16_length[i]) fprintf(stderr, "%u:%llu,", i, (unsigned long long)g_utf16_length[i]);
        fprintf(stderr, "\n");
    }
    if (g_observe_pointees)
        fprintf(stderr, "observe-pointees: %llu heap-block and %llu static-object pointer argument(s) observed on the original side "
                "(%llu truncated to the %u-byte cap, %.1f KB compared per run on average)\n", (unsigned long long)g_obs_n[1],
                (unsigned long long)g_obs_n[2], (unsigned long long)g_obs_trunc, g_obs_cap,
                (g_obs_n[1] + g_obs_n[2]) ? g_obs_bytes / 1024.0 : 0.0);
    if (refc::mode == refc::READ) {
        fprintf(stderr, "refcache: %llu input(s) took the recorded original, %llu ran it live\n",
                (unsigned long long)(refc_r.hits - refc_r.dlive), (unsigned long long)(refc_r.live + refc_r.dlive));
        if (refc_r.dmatch + refc_r.dlite + refc_r.dlive)
            fprintf(stderr, "refcache: digest records: %llu input(s) matched the candidate's digest, %llu needed no comparison "
                    "(aborts), %llu ran the original live after a digest difference\n", (unsigned long long)refc_r.dmatch,
                    (unsigned long long)refc_r.dlite, (unsigned long long)refc_r.dlive);
    } else if (refc::mode == refc::WRITE && refc::digest)
        fprintf(stderr, "refcache: wrote digest records of %llu input(s); the unmutated candidate matches %llu of them\n",
                (unsigned long long)refc_w.dg_inputs, (unsigned long long)refc_w.dg_self);
    if (snapc::mode == snapc::READ)
        fprintf(stderr, "snapcache: %llu function(s) took the cached snapshot, %llu captured it live; %llu validation input(s) took "
                "the recorded setups, %llu ran them live\n", (unsigned long long)snapc::fn_cached, (unsigned long long)snapc::fn_live,
                (unsigned long long)snapc::val_cached, (unsigned long long)snapc::val_live);
    else if (snapc::mode == snapc::WRITE)
        fprintf(stderr, "snapcache: wrote %llu snapshot(s) and %llu validation state(s)\n", (unsigned long long)snapc::wrote_snap,
                (unsigned long long)snapc::wrote_val);
    mut_flush("E");
    if (g_mutrec) fclose(g_mutrec);
    return total_fail ? 1 : 0;
}
